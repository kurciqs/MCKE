#ifndef MINECRAFT_CHUNK_H
#define MINECRAFT_CHUNK_H
#include "Graphics/Vertex.h"
#include "Graphics/Shader.h"
#include "Graphics/TextureParser.h"
#include "noise/SimplexNoise.h"

#define CHUNK_WIDTH  15
#define CHUNK_DEPTH  15
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
};

struct VertexPos{
    uint8_t x, y, z;
};


class Chunk {
public:
    explicit Chunk(const uint64_t& seed, const int& chunkX, const int& chunkZ, TextureParser textureParser);
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
    bool isGeneratedCorrectly = false;
    bool isUploadedCorrectly = false;
    glm::ivec2 chunkPos;
    static Chunk& getChunkByPos(std::vector<Chunk>& chunks, const glm::vec3& worldPosition);
    static BlockData& getBlockByPos(std::vector<Chunk>& chunks, const glm::vec3& pos);
    static Chunk& getChunkByChunkPos(std::vector<Chunk> &chunks, glm::ivec2 chunkPos);
    static glm::ivec3 localizePos(glm::vec3 worldPos, glm::ivec2 chunkPos);
private:
    void checkFaces(bool* faces, int x, int y, int z, bool cullChunkBorders);
    Vertex loadVertex(VertexPos vPos, uint8_t face, uint8_t texID, uint8_t UVIndex);
    static uint32_t expan(int x, int y, int z);
    int NUM_VERTS = 0;
    std::vector<std::vector<glm::ivec2>> texPositions;
    TextureParser m_textParser;
    [[nodiscard]] uint8_t Height(int x, int z) const;
    BlockData* ChunkData{};
    Vertex *VertexData{};
    size_t ChunkDataSize = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH * sizeof(BlockData);
    size_t VertexSize = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH * sizeof(Vertex) * 36;
    uint32_t VAO{}, VBO{};
    uint64_t seed;

};
#endif //MINECRAFT_CHUNK_H
