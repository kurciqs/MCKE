#include "Physics.h"
#include "Graphics/Camera.h"

static glm::vec3 getFriction(Entity entity){
    auto& playerRigidBody = entityCoordinator.GetComponent<RigidBody>(entity);

    glm::vec3 force;
    if (playerRigidBody.isFlying){
        force = drag_fly;
    }
    else if (playerRigidBody.onGround) {
        force = friction;
    }
    else if (playerRigidBody.velocity.y > 0.0f) {
        force = drag_jump;
    }
    else {
        force = drag_fall;
    }

    return force;
}

static glm::vec3 getSizeFromAABB(const AABB& aabb)
{
    return {aabb.max.x - aabb.min.x, aabb.max.y - aabb.min.y, aabb.max.z - aabb.min.z};
}

void Physics::Update(std::vector<Chunk>& chunks, float dt) {
    for (auto const &entity: mEntities) {
        auto &rigidBody = entityCoordinator.GetComponent<RigidBody>(entity);
        auto &transform = entityCoordinator.GetComponent<Transform>(entity);
        auto &aabb = entityCoordinator.GetComponent<AABB>(entity);

        rigidBody.velocity += rigidBody.acceleration * getFriction(entity) * dt;

        rigidBody.acceleration = glm::vec3(0.0f);

        rigidBody.onGround = false;
        resolveCollision(getSizeFromAABB(aabb), entity, chunks, dt);

        transform.position += rigidBody.velocity * dt;

        if (!rigidBody.isFlying) {
            rigidBody.velocity += gravity * dt;
        }

        drag(entity, dt);
    }
}

void Physics::applyForce(Entity entity, glm::vec3 force) {
    if (glm::isnan(force.x) || glm::isnan(force.y) || glm::isnan(force.z)) {
        return;
    }
    auto& rigidBody = entityCoordinator.GetComponent<RigidBody>(entity);
    force /= rigidBody.mass;
    rigidBody.acceleration += force;
}

static Collision findClosestCollision(const std::vector<Collision>& collisions) {
    Collision closestCollision{};
    //find the collision with the lowest entry time
    float smallest = std::numeric_limits<float>::max();
    for (const Collision& collision : collisions) {
        if (collision.entryTime < smallest) {
            closestCollision = collision;
            smallest = collision.entryTime;
        }
    }
    return closestCollision;
}

//https://stackoverflow.com/questions/13152252/is-there-a-compact-equivalent-to-python-range-in-c-stl
template <typename IntType>
std::vector<IntType> range(IntType start, IntType stop, IntType step)
{
    if (step == IntType(0))
    {
        throw std::invalid_argument("[ERROR]: step for range() must be non-zero");
    }

    std::vector<IntType> result;
    IntType i = start;
    while ((step > 0) ? (i <= stop) : (i >= stop))
    {
        result.push_back(i);
        i += step;
    }

    return result;
}

void Physics::resolveCollision(glm::vec3 playerSize, Entity playerEntity, std::vector<Chunk>& chunks, float dt) {
    //by https://www.youtube.com/watch?v=fWkbIOna6RA best shit ever
    auto &playerTransform = entityCoordinator.GetComponent<Transform>(playerEntity);
    auto &playerRigidBody = entityCoordinator.GetComponent<RigidBody>(playerEntity);
    auto &playerAABB = entityCoordinator.GetComponent<AABB>(playerEntity);

    for(int _ = 0; _ < 3; _++) { //trice for x, y, z
        glm::vec3 velocity = playerRigidBody.velocity * dt;
        glm::ivec3 step{};
        step.x = velocity.x > 0.0f ? 1 : -1;
        step.y = velocity.y > 0.0f ? 1 : -1;
        step.z = velocity.z > 0.0f ? 1 : -1;

        glm::ivec3 steps{};
        steps.x = (playerSize.x / 2.0f);
        steps.y = (playerSize.y);
        steps.z = (playerSize.z / 2.0f);

        glm::vec3 position = playerTransform.position;
        glm::ivec3 playerPosInt = glm::ivec3((position.x), (position.y), (position.z));

        glm::ivec3 changePos = glm::ivec3((position.x + velocity.x), (position.y + velocity.y), (position.z + velocity.z));

        std::vector<Collision> potentialCollisions;

        for (auto i: range(playerPosInt.x - step.x * (steps.x + 1), changePos.x + step.x * (steps.x + 2), step.x)) {
            for (auto j: range(playerPosInt.y - step.y * (steps.y + 2), changePos.y + step.y * (steps.y + 3), step.y)) {
                for (auto k: range(playerPosInt.z - step.z * (steps.z + 1), changePos.z + step.z * (steps.z + 2), step.z)) {
                    glm::vec3 blockPos(i, j, k);
                    if (blockPos.y < 0) {
                        continue;
                    }
                    BlockData block = Chunk::getBlockByPos(chunks, blockPos);

                    if (!block.isSolid) continue;

                    glm::vec3 blockSize(1.0f);
                    auto blockPosVec = glm::vec3(blockPos);
                    AABB blockAABB{blockPosVec, blockPosVec + blockSize};

                    //with slabs and stairs use multiple small boxes (future)
                    Collision collision = collide(playerAABB, blockAABB, velocity);
                    if (collision.normal == noCollision.normal && collision.entryTime == noCollision.entryTime) continue;
                    potentialCollisions.push_back(collision);
                }
            }
        }

        if (potentialCollisions.empty()) break;

        Collision closestCollision{};
        closestCollision = findClosestCollision(potentialCollisions);
        closestCollision.entryTime -= 0.001f;

        if (closestCollision.normal.x) {
            playerTransform.position.x += closestCollision.entryTime * velocity.x;
            playerRigidBody.velocity.x = 0.0f;
        }

        if (closestCollision.normal.y) {
            playerTransform.position.y += closestCollision.entryTime * velocity.y;
            playerRigidBody.velocity.y = 0.0f;
            if (closestCollision.normal.y == 1) {
                playerRigidBody.onGround = true;
            }
        }

        if (closestCollision.normal.z) {
            playerTransform.position.z += closestCollision.entryTime * velocity.z;
            playerRigidBody.velocity.z = 0.0f;
        }
    }
}

static float getMax(float a, float b, float c) {
    return (a>b) ? ((a>c) ? a : c) : ((b>c) ? b : c);
}

static float getMaxAbs(float a, float b, float c) {
    return (abs(a)>abs(b)) ? ((abs(a)>abs(c)) ? abs(a) : abs(c)) : ((abs(b)>abs(c)) ? abs(b) : abs(c));
}

static float getMin(float a, float b, float c) {
    return (a<b) ? ((a<c) ? a : c) : ((b<c) ? b : c);
}

Collision Physics::collide(AABB &a, AABB &b, const glm::vec3& velocity) {
    auto time = [](float a, float b) {
        if (b)
            return a / b;
        else
            return a > 0.0f ? -INFINITY : INFINITY;
    };

    float x_entry = time(velocity.x > 0.0f ? b.min.x - a.max.x : b.max.x - a.min.x, velocity.x);
    float x_exit = time(velocity.x > 0.0f ? b.max.x - a.min.x : b.min.x - a.max.x, velocity.x);

    float y_entry = time(velocity.y > 0.0f ? b.min.y - a.max.y : b.max.y - a.min.y, velocity.y);
    float y_exit = time(velocity.y > 0.0f ? b.max.y - a.min.y : b.min.y - a.max.y, velocity.y);

    float z_entry = time(velocity.z > 0.0f ? b.min.z - a.max.z : b.max.z - a.min.z, velocity.z);
    float z_exit = time(velocity.z > 0.0f ? b.max.z - a.min.z : b.min.z - a.max.z, velocity.z);

    if (x_entry < 0.0f && y_entry < 0.0f && z_entry < 0.0f)
        return noCollision;
    if (x_entry > 1.0f || y_entry > 1.0f || z_entry > 1.0f)
        return noCollision;

    float entry = getMax(x_entry, y_entry, z_entry);
    float exit = getMin(x_exit, y_exit, z_exit);

    if(entry > exit) {
        return noCollision;
    }

    glm::vec3 normal;
    normal.x = entry == x_entry ? (velocity.x > 0.0f ? -1.0f: 1.0f) : 0.0f;
    normal.y = entry == y_entry ? (velocity.y > 0.0f ? -1.0f: 1.0f) : 0.0f;
    normal.z = entry == z_entry ? (velocity.z > 0.0f ? -1.0f: 1.0f) : 0.0f;

    return {entry, normal};
}

void Physics::drag(Entity entity, float dt) {
    auto& playerRigidBody = entityCoordinator.GetComponent<RigidBody>(entity);

    playerRigidBody.velocity -= playerRigidBody.velocity * getFriction(entity) * dt;
}

RaycastResult Physics::ShootRay(glm::vec3 origin, glm::vec3 direction, std::vector<Chunk>& chunks) {

    RaycastResult result{};
    result.hit = false;
    result.blockPosition = glm::vec3(0.0f);

    Raycast ray{};
    ray.origin = origin;
    ray.direction = glm::normalize(direction); //make sure
    ray.distance = 0.0f;
    ray.position = origin;

    float maxDistance = 5.0f;

    float step = 0.1f;

    glm::vec3 currentOrigin = origin;

    glm::vec3 nextBlockDirection = glm::round(ray.direction);

    glm::ivec3 lastBlockPosition = glm::ivec3(0.0f);

    for (float i = 0.0f; i < maxDistance; i += step) {

        if (glm::ceil(ray.position) != currentOrigin) {

            currentOrigin = glm::ceil(ray.position);

            if (glm::floor(ray.position).y <= 0.0f) {
                continue;
            }
            BlockData& curBlock = Chunk::getBlockByPos(chunks, glm::floor(ray.position));

            if (curBlock.isSolid){
                AABB aabb = AABB{glm::floor(ray.position), glm::ceil(ray.position)};

                float t1 = (aabb.min.x - ray.origin.x) / direction.x;
                float t2 = (aabb.max.x - ray.origin.x) / direction.x;
                float t3 = (aabb.min.y - ray.origin.y) / direction.y;
                float t4 = (aabb.max.y - ray.origin.y) / direction.y;
                float t5 = (aabb.min.z - ray.origin.z) / direction.z;
                float t6 = (aabb.max.z - ray.origin.z) / direction.z;

                float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
                float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

                if (tmax < 0.0f || tmin > tmax) {
                    // No intersection
                    return result; // return result.hit = false
                }

                float depth;
                if (tmin < 0.0f) {
                    // The ray's origin is inside the AABB
                    depth = tmax;
                }
                else {
                    depth = tmin;
                }

                result.blockPosition = glm::floor(ray.position);
                result.lastBlockPosition = lastBlockPosition;
                result.hit = true;
                return result;
            }

            lastBlockPosition = glm::floor(ray.position);
        }
        ray.position += direction * step;  /// HELLO HERE I AM
    }
    return result;
}

bool Physics::RayIntersectsAABB(Raycast& ray, AABB& aabb) {
    // r.dir is unit direction vector of ray
    glm::vec3 dirfrac{};
    dirfrac.x = 1.0f / ray.direction.x;
    dirfrac.y = 1.0f / ray.direction.y;
    dirfrac.z = 1.0f / ray.direction.z;

    glm::vec3 lb = aabb.min;
    glm::vec3 rt = aabb.max;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    float t1 = (lb.x - ray.origin.x) * dirfrac.x;
    float t2 = (rt.x - ray.origin.x) * dirfrac.x;
    float t3 = (lb.y - ray.origin.y) * dirfrac.y;
    float t4 = (rt.y - ray.origin.y) * dirfrac.y;
    float t5 = (lb.z - ray.origin.z) * dirfrac.z;
    float t6 = (rt.z - ray.origin.z) * dirfrac.z;

    float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
    float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

    if (tmax < 0 || tmin > tmax)
    {
        // No intersection
        return false;
    }

    float depth = 0.0f;
    if (tmin < 0.0f)
    {
        // The ray's origin is inside the AABB
        depth = tmax;
    }
    else
    {
        depth = tmin;
    }
    return true;
}

bool Physics::AABBIntersectsAABB(AABB &a, AABB &b) {
    return !(a.min.x > b.max.x || a.max.x < b.min.x || a.min.y > b.max.y || a.max.y < b.min.y || a.min.z > b.max.z || a.max.z < b.min.z); // github copilot
}
