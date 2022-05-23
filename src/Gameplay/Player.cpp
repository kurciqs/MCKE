#include "Player.h"

Player::Player(glm::vec3 spawnPosition)
: camera(Camera(Input::getWindow()->width, Input::getWindow()->height, spawnPosition)), size(glm::vec3(0.8f, 1.9f, 0.8f))
{}

void Player::Load() {
    playerEntity = entityCoordinator.CreateEntity();
    entityCoordinator.AddComponent(playerEntity, RigidBody{glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, false, false});
    entityCoordinator.AddComponent(playerEntity, Transform{camera.Position, glm::vec3(0.0f), glm::vec3(1.0f)});
    entityCoordinator.AddComponent(playerEntity, AABB{glm::vec3(0.0f, 0.0f, 0.0f), size });
}

static float lerp(float a, float b, float f) {
    return a + f * (b - a);
}

namespace Draw {

}

static void drawAABB(AABB in_aabb, Camera& cam, glm::vec3 color){
    const char* vertexShaderSrc = "#version 330 core\n"
                                  "layout (location = 0) in vec3 aPos;"
                                  "uniform mat4 model;"
                                  "uniform mat4 cam;"
                                  "void main() {"
                                  "	gl_Position = cam * (model * vec4(aPos, 1.0));"
                                  "}\0";

    const char* fragmentShaderSrc = "#version 330 core\n"
                                    "out vec4 fragColor;"
                                    "uniform vec3 color;"
                                    "void main() {"
                                    "	fragColor = vec4(color, 1.0f);"
                                    "}\0";

    int success;
    char infoLog[512];
    unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR:SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    AABB aabb{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)};
    static glm::vec3 verticies[24] = {
        aabb.min, glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z),
        aabb.min, glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z),
        aabb.min, glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z),
        aabb.max, glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z),
        aabb.max, glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z),
        aabb.max, glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z), glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z), glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z), glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z), glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z), glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z), glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z)
    };

    GLuint vbo;
    GLuint vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, in_aabb.min);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "cam"), 1, GL_FALSE, glm::value_ptr(cam.cameraMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), color.x, color.y, color.z);

    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 24);
    glLineWidth(1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Player::Update(float dt, std::vector<Chunk>& chunks) {
    float changeSpeed = 0.5f;
    if (Input::isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        speed = lerp(speed, sprintSpeed, changeSpeed);
    }
    else {
        speed = lerp(speed, walkSpeed, changeSpeed);
    }

    camera.fov = Input::isKeyDown(ZOOM_KEY) ? 20.0f : 45.0f + 8.5f * (speed - walkSpeed) / (sprintSpeed - walkSpeed);

    auto& rigidBody = entityCoordinator.GetComponent<RigidBody>(playerEntity);

    UpdateCollider();
    // Basic push forces:
    glm::vec3 moveForce(0.0f);
    glm::vec3 horizontalOrientation = glm::vec3(camera.Orientation.x, 0.0f, camera.Orientation.z);

    flyDebounce -= dt;
    if (Input::isKeyDown(GLFW_KEY_F) && flyDebounce < 0.0f) {
        rigidBody.isFlying = !rigidBody.isFlying;
        flyDebounce = 0.5f;
    }

    float multiplier = crouching ? speed * 0.25f : (rigidBody.isFlying ? 2.0f : 1.0f) * speed;

    if (Input::isKeyDown(GLFW_KEY_W)){
        moveForce += glm::normalize(horizontalOrientation) * multiplier;
    }
    if (Input::isKeyDown(GLFW_KEY_A)){
        moveForce -= multiplier * glm::normalize(glm::cross(horizontalOrientation, camera.Up));
    }
    if (Input::isKeyDown(GLFW_KEY_S)){
        moveForce -= glm::normalize(horizontalOrientation) * multiplier;
    }
    if (Input::isKeyDown(GLFW_KEY_D)){
        moveForce += multiplier * glm::normalize(glm::cross(horizontalOrientation, camera.Up));
    }

    if (rigidBody.isFlying) {
        if (Input::isKeyDown(GLFW_KEY_SPACE)) {
            moveForce += glm::vec3(0.0f, 1.0f, 0.0f) * multiplier * 1.5f;
        }
        if (Input::isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
            moveForce += glm::vec3(0.0f, -1.0f, 0.0f) * multiplier * 1.5f;
        }
    }

    auto& collider = entityCoordinator.GetComponent<AABB>(playerEntity);
    if (Input::isKeyDown(GLFW_KEY_C) && rigidBody.onGround) {
        cameraOffset = lerp(cameraOffset, crouchCameraOffset, 0.5f);
        crouching = true;
    }
    else {
        cameraOffset = lerp(cameraOffset, normalCameraOffset, 0.5f);
        crouching = false;
    }

    Physics::applyForce(playerEntity, moveForce);

    if (Input::isKeyDown(GLFW_KEY_SPACE) && rigidBody.onGround && !rigidBody.isFlying) {
        rigidBody.velocity.y = glm::sqrt(-2.0f * gravity.y * jumpHeight);
    }

    auto& playerPos = entityCoordinator.GetComponent<Transform>(playerEntity);
    camera.setPos(playerPos.position + glm::vec3(0.0f, cameraOffset, 0.0f));

    UpdateCollider();

    prevChunkPos = chunkPos;
    chunkPos = glm::ivec2(glm::floor(camera.getPos().x/CHUNK_WIDTH), glm::floor(camera.getPos().z/CHUNK_DEPTH));

    camera.setWindowParams(Input::getWindow());
    camera.Input();
    camera.Update();

    if (Input::isKeyDown(GLFW_KEY_Q))
        printf("Player position: < %f %f %f >\n", camera.getPos().x, camera.getPos().y, camera.getPos().z);

    mouseDebounce -= dt;
    float resetMouseDebounce = 0.15f;

    RaycastResult raycastResult = Physics::ShootRay(camera.Position, camera.Orientation, chunks);

    if (raycastResult.hit){
        if (Chunk::getBlockByPos(chunks, raycastResult.blockPosition).isSolid) {
            drawAABB({raycastResult.blockPosition, raycastResult.blockPosition + glm::ivec3(1.0f)}, camera, glm::vec3(0.9f, 0.9f, 0.9f));
        }
        uint8_t replaceWith;
        if (Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_1) && mouseDebounce < 0.0f){
            replaceWith = 0;
            mouseDebounce = resetMouseDebounce;
        }
        else if (Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_2) && mouseDebounce < 0.0f){
            replaceWith = equippedBlock;
            mouseDebounce = resetMouseDebounce;
        }
        else if (Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_MIDDLE) && mouseDebounce < 0.0f){
            equippedBlock = Chunk::getBlockByPos(chunks, raycastResult.blockPosition).id;
            mouseDebounce = resetMouseDebounce;
            return;
        }
        else {
            return;
        }
        if (replaceWith != 0){
            Chunk& chunk = Chunk::getChunkByPos(chunks, raycastResult.lastBlockPosition);

            AABB blockAABB = AABB{raycastResult.lastBlockPosition, raycastResult.lastBlockPosition + glm::ivec3(1.0f)};
            AABB playerAABB = entityCoordinator.GetComponent<AABB>(playerEntity);
            if (Physics::AABBIntersectsAABB(blockAABB, playerAABB)){
                return;
            }

            glm::ivec3 localPos = Chunk::localizePos(raycastResult.lastBlockPosition, chunk.chunkPos);
            chunk.setBlockAndUpdate(localPos.x, localPos.y, localPos.z, replaceWith);
        }
        else {
            Chunk& chunk = Chunk::getChunkByPos(chunks, raycastResult.blockPosition);

            glm::ivec3 localPos = Chunk::localizePos(raycastResult.blockPosition, chunk.chunkPos);
            chunk.setBlockAndUpdate(localPos.x, localPos.y, localPos.z, replaceWith);
        }
    }
}

void Player::UpdateCollider() const {
    auto& playerCollider = entityCoordinator.GetComponent<AABB>(playerEntity);

    glm::vec3 pos = camera.Position - glm::vec3(0.0f, cameraOffset, 0.0f);
    playerCollider.min = glm::vec3(pos.x - size.x/2.0f, pos.y - size.y/2.0f, pos.z - size.z/2.0f);
    playerCollider.max = glm::vec3(pos.x + size.x/2.0f, pos.y + size.y/2.0f, pos.z + size.z/2.0f);

    if (crouching) {
        playerCollider.max.y -= 0.1f;
    }
}

void Player::SetEquippedBlock(uint8_t blockID) {
    equippedBlock = blockID;
}