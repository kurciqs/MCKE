#ifndef MINECRAFT_CHUNK_H
#define MINECRAFT_CHUNK_H
#include "Graphics/Vertex.h"
#include "Graphics/Shader.h"
#include "Graphics/BlockParser.h"
#include "noise/SimplexNoise.h"

#define CHUNK_WIDTH 15
#define CHUNK_DEPTH 15
#define CHUNK_HEIGHT 255

enum Face {
    FRONT,
    BACK,
    BOTTOM,
    TOP,
    LEFT,
    RIGHT
};

struct BlockData {
    uint8_t id;
    bool isSolid;
    bool isTransparent;
    uint8_t lightLevel;
    bool isOpaque;
};

struct VertexPos{
    uint8_t x, y, z;
};

class Chunk {
public:
    bool isGeneratedCorrectly = false;
    bool isUploadedCorrectly = false;
    glm::ivec2 chunkPos;
    explicit Chunk(const uint64_t& seed, const int& chunkX, const int& chunkZ, BlockParser textureParser);
    void FreeChunkData();
    void FreeVertexData();
    void Generate();
    void GenerateVertexData();
    void Render() const;
    void DeleteBuffers();
    void UploadToGPU();
    void Serialize(const std::string& worldFolder);
    void Deserialize(const std::string& worldFolder);
    void setBlock(const int& x, const int& y, const int& z, const uint8_t& id);
    void setBlockAndUpdate(const int& x, const int& y, const int& z, const uint8_t& id);
    bool isSerialized(const std::string& worldFolder) const;
    BlockData& GetBlock(const int& x, const int& y, const int& z) const;
    static Chunk& getChunkByPos(std::vector<Chunk>& chunks, const glm::vec3& worldPosition);
    static BlockData& getBlockByPos(std::vector<Chunk>& chunks, const glm::vec3& pos);
    static Chunk& getChunkByChunkPos(std::vector<Chunk> &chunks, glm::ivec2 chunkPos);
    static glm::ivec3 localizePos(glm::vec3 worldPos, glm::ivec2 chunkPos);
    int getSunlight(int x, int y, int z);
    void setSunlight(int x, int y, int z, int val);
    int getTorchlight(int x, int y, int z);
    void setTorchlight(int x, int y, int z, int val);
    static uint32_t expan(int x, int y, int z);
private:
    unsigned char lightMap[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH]; // most significant 4 bits are sunlight, the least significant are voxel light, another 4 bits might be color
    void checkFaces(bool* faces, int x, int y, int z, bool cullChunkBorders);
    Vertex loadVertex(VertexPos vPos, uint8_t face, bool isTransparent, uint8_t lightLevel, uint8_t texID, uint8_t UVIndex);
    int NUM_VERTS = 0;
    std::vector<std::vector<glm::ivec2>> texPositions;
    BlockParser m_blockParser;
    [[nodiscard]] uint8_t Height(int x, int z) const;
    BlockData* ChunkData{};
    Vertex *VertexData{};
    size_t ChunkDataSize = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH * sizeof(BlockData);
    size_t VertexSize = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH * sizeof(Vertex) * 36;
    uint32_t VAO{}, VBO{};
    uint64_t seed;
};

struct LightNode {
    LightNode(glm::ivec3 pos, Chunk* ch) : pos(pos), chunk(ch) {}
    Chunk* chunk;
    glm::ivec3 pos; // local
};

struct LightRemovalNode {
    LightRemovalNode(glm::ivec3 p, short v, Chunk* ch) : pos(p), val(v), chunk(ch) {}
    glm::ivec3 pos; // local
    short val;
    Chunk* chunk;
};

#endif //MINECRAFT_CHUNK_H