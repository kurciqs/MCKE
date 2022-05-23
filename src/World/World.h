#ifndef MINECRAFT_WORLD_H
#define MINECRAFT_WORLD_H
//#include "Graphics/Texture.h"
#include "World/Chunk.h"
#include "Graphics/Camera.h"
#include "Graphics/TextureParser.h"
#include "Gameplay/Player.h"
#include "glm/gtx/hash.hpp"
#include <thread>
#include <unordered_set>
#include <iostream>
#define maxAmountOfThreads 12

#define READ_CHUNKS 0

class World{
public:
    glm::vec3 worldSpawn = glm::vec3(0.0f, 100.0f, 0.0f);
    World(Window *WIN, uint64_t seed, TextureParser textureParser, std::string savePath);
    void Load();
    void Update(float dt);
    void Render();
    void Delete();
    void GenerateChunks(uint8_t renderDistance);
    void RegenerateChunks(uint8_t renderDistance);
    static void loadChunk(Chunk& chunk);
    glm::vec4 skyColor;
    float time{};
    std::vector<Chunk> chunks;
    std::string savePath;
    uint64_t seed;
private:
    Player player;
    std::shared_ptr<Physics> physicsSystem;
    TextureParser w_TextureParser;
    std::unordered_set<glm::ivec2> loadedChunkPositions;
    uint32_t texture{};
    Shader shader;
    Window* window;
};

#endif //MINECRAFT_WORLD_H
