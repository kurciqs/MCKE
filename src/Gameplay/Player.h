#ifndef MINECRAFT_PLAYER_H
#define MINECRAFT_PLAYER_H

#define ZOOM_KEY GLFW_KEY_Z

#include "Graphics/Camera.h"
#include "Gameplay/Physics.h"
#include "Window/Input.h"


class Player {
public:
    explicit Player(glm::vec3 spawnPosition);
    Entity playerEntity{};
    Camera camera;
    void Update(float dt, std::vector<Chunk>& chunks);
    void Load();
    void UpdateCollider() const;
    glm::ivec2 chunkPos{};
    glm::ivec2 prevChunkPos{};
    glm::vec3 size{};
    glm::ivec3 hittenBlock{};
//    void HitBlocks(std::vector<Chunk>& chunks, glm::ivec3 curBlock, glm::ivec3 nextBlock);
    void SetEquippedBlock(uint8_t blockID);
private:
    float flyDebounce = 0.0f;
    float mouseDebounce = 0.0f;
    uint8_t equippedBlock = 1;
    float cameraOffset = 0.9f;
    float speed = 5.317f;
    float sprintSpeed = 8.0f;
    float walkSpeed = 5.317f;
    float jumpHeight = 1.25f;
    bool crouching = false;
    float crouchCameraOffset = 0.5f;
    float normalCameraOffset = 0.9f;
};

#endif //MINECRAFT_PLAYER_H
