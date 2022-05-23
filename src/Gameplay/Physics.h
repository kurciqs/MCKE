#ifndef MINECRAFT_PHYSICS_H
#define MINECRAFT_PHYSICS_H
#include "Gameplay/ECS.h"
#include "Gameplay/Components.h"
#include "World/Chunk.h"
#include "Window/Input.h"
#include "Graphics/Camera.h"

extern Coordinator entityCoordinator;

struct Collision{
    float entryTime;
    glm::vec3 normal;
};

struct Raycast {
    glm::vec3 direction;
    glm::vec3 origin;
    glm::vec3 position;  // point on the tip of the ray
    float distance;
//    bool step(std::vector<Chunk>& chunks, void (*hitCallback)(std::vector<Chunk>& chunks, glm::ivec3 curBlock, glm::ivec3 nextBlock));
//    bool check(std::vector<Chunk>& chunks, void (*hitCallback)(std::vector<Chunk>& chunks, glm::ivec3 curBlock, glm::ivec3 nextBlock), float distance, glm::ivec3 currentBlock, glm::ivec3 nextBlock);
};

struct RaycastResult {
    glm::ivec3 blockPosition;
    glm::ivec3 lastBlockPosition;
    bool hit;
};

static const glm::vec3 gravity = glm::vec3(0.0f, -32.0f, 0.0f);
static const glm::vec3 friction = glm::vec3(5.0f, 5.0f, 5.0f);
static const glm::vec3 drag_fall = glm::vec3(1.8f, 0.4f, 1.8f);
static const glm::vec3 drag_jump = glm::vec3(1.8f, 0.0f, 1.8f);
static const glm::vec3 drag_fly = glm::vec3(5.0f, 5.0f, 5.0f);

static const Collision noCollision {1.0f, glm::vec3(-420.69f)};

class Physics : public System {
public:
    void Update(std::vector<Chunk>& chunks, float dt);
    static void applyForce(Entity entity, glm::vec3 force);
    static void resolveCollision(glm::vec3 playerSize, Entity playerEntity, std::vector<Chunk>& chunks, float dt);
    static Collision collide(AABB& a, AABB& b, const glm::vec3& velocity);
    static void drag(Entity entity, float dt);
    static RaycastResult ShootRay(glm::vec3 origin, glm::vec3 direction, std::vector<Chunk>& chunks);
    static bool RayIntersectsAABB(Raycast& ray, AABB& aabb);
    static bool AABBIntersectsAABB(AABB& a, AABB& b);
};

#endif //MINECRAFT_PHYSICS_H
