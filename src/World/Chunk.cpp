#include "World/Chunk.h"

static SimplexNoise Noise;

Chunk::Chunk(const uint64_t& seed, const int& chunkX, const int& chunkZ, TextureParser textureParser) :seed(seed), chunkPos(glm::ivec2(chunkX, chunkZ)),
m_textParser(std::move(textureParser)){
    texPositions = m_textParser.blockTexPositions;
}

uint32_t Chunk::expan(int x, int y, int z) {
    return (z * CHUNK_WIDTH * CHUNK_HEIGHT) + (y * CHUNK_WIDTH) + x;
}

float map(float X, float A, float B, float C, float D){
    //X is between A and B ----> C and D
    return (X-A)/(B-A) * (D-C) + C;
}

Vertex Chunk::loadVertex(VertexPos vPos, uint8_t face, uint8_t texID, uint8_t UVIndex){
    uint8_t XZ = 0;
    XZ |= (vPos.x << 4);
    XZ |= vPos.z;

    glm::ivec2 readTexPos = texPositions[texID][face];
    glm::ivec2 possibleTexPositions[4] = {readTexPos, readTexPos + glm::ivec2(1, 0), readTexPos + glm::ivec2(0, 1), readTexPos + glm::ivec2(1, 1)};
    glm::ivec2 texPos = possibleTexPositions[UVIndex];
    uint8_t texPos_comp = 0;
    texPos_comp |= (texPos.x << 4);
    texPos_comp |= texPos.y;

    return {XZ, vPos.y, texPos_comp, face};
}

void Chunk::checkFaces(bool face[6], int x, int y, int z, bool cullChunkBorders){
    if (y != CHUNK_HEIGHT - 1) {
        face[Face::TOP] = ChunkData[expan(x, y + 1, z)].id == 0;
    }
    if (y != 0) {
        face[Face::BOTTOM] = ChunkData[expan(x, y - 1, z)].id == 0;
    }
    if (x != CHUNK_WIDTH - 1)
        face[Face::RIGHT] = ChunkData[expan(x + 1, y, z)].id == 0;
    if (x == CHUNK_WIDTH - 1) {
        if (cullChunkBorders && Height(x + 1, z) > y)
            face[Face::RIGHT] = false;
    }
    if (x != 0)
        face[Face::LEFT] = ChunkData[expan(x - 1, y, z)].id == 0;
    if (x == 0) {
        if (cullChunkBorders && Height(x - 1, z) > y)
            face[Face::LEFT] = false;
    }
    if (z != CHUNK_DEPTH - 1)
        face[Face::BACK] = ChunkData[expan(x, y, z + 1)].id == 0;
    if (z == CHUNK_WIDTH - 1) {
        if (cullChunkBorders && Height(x, z + 1) > y)
            face[Face::BACK] = false;
    }
    if (z != 0)
        face[Face::FRONT] = ChunkData[expan(x, y, z - 1)].id == 0;
    if (z == 0) {
        if (cullChunkBorders && Height(x, z - 1) > y)
            face[Face::FRONT] = false;
    }
}

void Chunk::Generate() {
    ChunkData = (BlockData*)malloc(ChunkDataSize);

    /// generating info about individual blocks:
    bool tree = false;
    int treeHeight = 0;
    int maxTreeHeight = 9;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        tree = false;
        for (int z = 0; z < CHUNK_DEPTH; z++) {
            uint8_t maxHeight = Height(x, z);
            uint8_t otherHeight = Height(x * 2.0f, z * -2.0f) - 10.0f;
//            printf("%f\n", maxHeight / otherHeight);
            if (rand() % 1000 < 1) {
                tree = true;
            }
            uint8_t stoneHeight = (maxHeight*maxHeight) / 127.0f;
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                BlockData block{0, false};
                if (y < stoneHeight / 3.0f)
                {
                    if (rand() % 100 < 1)
                        block.id = m_textParser.getIdByName("diamond_ore");
                    else
                        block.id = m_textParser.getIdByName("deepslate");
                    block.isSolid = true;
                }
                else if (y < stoneHeight)
                {
                    block.id = m_textParser.getIdByName("stone");
                    block.isSolid = true;
                }
                else if (y < maxHeight)
                {
                    block.id = m_textParser.getIdByName("dirt");
                    block.isSolid = true;
                }
                else if (y == maxHeight)
                {
                    if (otherHeight > maxHeight) {
                        if (rand() % 100 < 50)
                            block.id = m_textParser.getIdByName("stone");
                        else
                            block.id = m_textParser.getIdByName("sand");
                    }
                    else if (rand() % 100 < 5) {

                        block.id = m_textParser.getIdByName("dirt");
                    }
                    else
                        block.id = m_textParser.getIdByName("grass_block");
                    block.isSolid = true;
                }
                else if (tree) {
                    block.id = m_textParser.getIdByName("oak_log");
                    treeHeight += rand() > 0.5f ? 1 : 3;
                }
                if (tree && treeHeight > maxTreeHeight) {
                    tree = false;
                    treeHeight = 0;
                }
                ChunkData[expan(x, y, z)] = block;
            }
        }
    }
}

void Chunk::UploadToGPU(){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_VERTS*sizeof(Vertex), VertexData, GL_STATIC_DRAW);

    //WARNING: this causes a segfault for some reason on my arm 32 bit raspberry pi!
    // xz -- 8 bit integer
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, xz));
    glEnableVertexAttribArray(0);

    // y -- 8 bit integer
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, y));
    glEnableVertexAttribArray(1);

    // texPOS -- 8bit int
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, texPos));
    glEnableVertexAttribArray(2);

    // face -- 8bit int
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, y));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Chunk::Render() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTS);
    glBindVertexArray(0);
}

void Chunk::DeleteBuffers() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

uint8_t Chunk::Height(int x, int z) const {
    return 60.0f + (map(Noise.fractal(10, (x + chunkPos.x * CHUNK_WIDTH + seed)/100.0f, (z + chunkPos.y * CHUNK_DEPTH + seed)/100.0f), -1.0f, 1.0f, 0.0f, 1.0f) * 30.0f);
}

static std::string get_formatted_filepath(const Chunk& chunk, const std::string& worldFolder){
    return worldFolder + "/" + std::to_string(chunk.chunkPos.x) + "_" + std::to_string(chunk.chunkPos.y) + ".bin";
}

void Chunk::Serialize(const std::string& worldFolder) {
    std::string filepath = get_formatted_filepath(*this, worldFolder);
    FILE* fp = fopen(filepath.c_str(), "wb");
    fwrite(&chunkPos, sizeof(glm::ivec2), 1, fp);
    fwrite(ChunkData, ChunkDataSize, 1, fp);
    fclose(fp);
}

void Chunk::Deserialize(const std::string &worldFolder) {
    std::string filepath = get_formatted_filepath(*this, worldFolder);
    FILE *fp = fopen(filepath.c_str(), "rb");
    fread(&chunkPos, sizeof(glm::ivec2), 1, fp);
    ChunkData = (BlockData *) malloc(ChunkDataSize);
    fread(ChunkData, ChunkDataSize, 1, fp);
    fclose(fp);
}

void Chunk::FreeChunkData() {
    free((void *) ChunkData);
}

void Chunk::FreeVertexData() {
    free((void *) VertexData);
}

void Chunk::GenerateVertexData() {
    VertexData = (Vertex *)malloc(VertexSize);
    NUM_VERTS = 0;
    int VERT_INDEX = 0;

    /// generating actual vertex and index data:
    for (int x = 0; x < CHUNK_WIDTH; x++){
        for (int y = 0; y < CHUNK_HEIGHT; y++){
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                BlockData& curBlock = ChunkData[expan(x, y, z)];

                if (curBlock.id != 0) {
                    curBlock.isSolid = true;
                    glm::u8vec3 pos = glm::u8vec3(x, y, z);
                    VertexPos positions[8];

                    positions[0] = {pos.x, pos.y, pos.z};
                    positions[1] = {static_cast<uint8_t>(pos.x + 1), pos.y, pos.z};
                    positions[2] = {static_cast<uint8_t>(pos.x + 1), static_cast<uint8_t>(pos.y + 1), pos.z};
                    positions[3] = {pos.x, static_cast<uint8_t>(pos.y + 1), pos.z};
                    positions[4] = {positions[0].x, positions[0].y, static_cast<uint8_t>(positions[0].z + 1)};
                    positions[5] = {positions[1].x, positions[1].y, static_cast<uint8_t>(positions[1].z + 1)};
                    positions[6] = {positions[2].x, positions[2].y, static_cast<uint8_t>(positions[2].z + 1)};
                    positions[7] = {positions[3].x, positions[3].y, static_cast<uint8_t>(positions[3].z + 1)};

                    bool face[6] = {true, true, true, true, true, true};
                    checkFaces(face, x, y, z, false);

                    if (face[Face::FRONT]) {
                        int indices[6] =   {2, 1, 0, 3, 2, 0};
                        int UVIndices[6] = {2, 0, 1, 3, 2, 1};
                        for (int i = 0; i < 6; i++) {
                            VertexData[VERT_INDEX++] = loadVertex(positions[indices[i]], Face::FRONT, curBlock.id, UVIndices[i]);
                        }
                        NUM_VERTS += 6;
                    }

                    if (face[Face::BACK]) {
                        int indices[6] =   {5, 6, 7, 4, 5, 7};
                        int UVIndices[6] = {1, 3, 2,0, 1, 2};
                        for (int i = 0; i < 6; i++) {
                            VertexData[VERT_INDEX++] = loadVertex(positions[indices[i]], Face::BACK, curBlock.id, UVIndices[i]);
                        }
                        NUM_VERTS += 6;
                    }

                    if (face[Face::BOTTOM]) {
                        int indices[6] =   {1, 5, 4, 0, 1, 4};
                        int UVIndices[6] = {2, 3, 1, 0, 2, 1};
                        for (int i = 0; i < 6; i++) {
                            VertexData[VERT_INDEX++] = loadVertex(positions[indices[i]], Face::BOTTOM, curBlock.id, UVIndices[i]);
                        }
                        NUM_VERTS += 6;
                    }

                    if (face[Face::TOP]) {
                        int indices[6] = {6, 2, 3, 7, 6, 3};
                        int UVIndices[6] = {2, 3, 1, 0, 2, 1};
                        for (int i = 0; i < 6; i++) {
                            VertexData[VERT_INDEX++] = loadVertex(positions[indices[i]], Face::TOP, curBlock.id, UVIndices[i]);
                        }
                        NUM_VERTS += 6;
                    }

                    if (face[Face::LEFT]) {
                        int indices[6] =   {7, 3, 0, 4, 7, 0};
                        int UVIndices[6] = {2, 3, 1, 0, 2, 1};
                        for (int i = 0; i < 6; i++) {
                            VertexData[VERT_INDEX++] = loadVertex(positions[indices[i]], Face::LEFT, curBlock.id, UVIndices[i]);
                        }
                        NUM_VERTS += 6;
                    }

                    if (face[Face::RIGHT]) {
                        int indices[6] =   {2, 6, 5, 1, 2, 5};
                        int UVIndices[6] = {2, 3, 1, 0, 2, 1};
                        for (int i = 0; i < 6; i++) {
                            VertexData[VERT_INDEX++] = loadVertex(positions[indices[i]], Face::RIGHT, curBlock.id, UVIndices[i]);
                        }
                        NUM_VERTS += 6;
                    }
                }
                else
                    curBlock.isSolid = false;
            }
        }
    }
}

bool Chunk::isSerialized(const std::string& worldFolder) const {
    std::string filename = get_formatted_filepath(*this, worldFolder);
    std::ifstream ifile(filename.c_str());
    return ifile.is_open();
}

BlockData &Chunk::GetBlock(const int &x, const int &y, const int &z) const {
    if (x < 0 || x > CHUNK_WIDTH || y < 0 || y > CHUNK_HEIGHT || z < 0 || z > CHUNK_DEPTH)
        printf("[ERROR]: Chunk::GetBlock(%d, %d, %d) out of bounds\n", x, y, z);
    if (!ChunkData)
        printf("[ERROR]: ChunkData is null\n", x, y, z);
    BlockData& data = ChunkData[expan(x, y, z)];
    return data;
}

// neighboring chunks : 0 = right, 1 = left, 2 = front, 3 = back
void Chunk::setBlock(const int& x, const int& y, const int& z, const uint8_t& id) {
    if (x < 0 || x > CHUNK_WIDTH || y < 0 || y > CHUNK_HEIGHT || z < 0 || z > CHUNK_DEPTH)
        printf("[ERROR]: Chunk::setBlock(%d, %d, %d) out of bounds\n", x, y, z);
    if (!ChunkData)
        printf("[ERROR]: ChunkData is null\n", x, y, z);

    ChunkData[expan(x, y, z)].id = id;
}

void Chunk::setBlockAndUpdate(const int &x, const int &y, const int &z, const uint8_t &id) {
    setBlock(x, y, z, id);
    GenerateVertexData();
    UploadToGPU();
    FreeVertexData();
    isGeneratedCorrectly = true;
}

Chunk& Chunk::getChunkByPos(std::vector<Chunk>& chunks, const glm::vec3& worldPosition)
{
    glm::ivec2 chunkPos = glm::ivec2(glm::floor(worldPosition.x/CHUNK_WIDTH), glm::floor(worldPosition.z/CHUNK_DEPTH));
    for (Chunk& chunk : chunks)
    {
        if (chunk.chunkPos == chunkPos)
        {
            return chunk;
        }
    }
    printf("[ERROR]: Chunk not found!\n");
    return chunks[0];
}

BlockData& Chunk::getBlockByPos(std::vector<Chunk>& chunks, const glm::vec3& pos) {
    Chunk& chunk = getChunkByPos(chunks, pos);
    glm::ivec3 localPosition = glm::ivec3(pos) - glm::ivec3(chunk.chunkPos.x * CHUNK_WIDTH, 0.0f, chunk.chunkPos.y * CHUNK_DEPTH);
    return chunk.GetBlock(localPosition.x, localPosition.y, localPosition.z);
}

Chunk& Chunk::getChunkByChunkPos(std::vector<Chunk>& chunks, glm::ivec2 chunkPos) {
    for (Chunk &chunk: chunks) {
        if (chunk.chunkPos == chunkPos) {
            return chunk;
        }
    }
    printf("[ERROR]: Chunk not found!\n");
    return chunks[0];
}

glm::ivec3 Chunk::localizePos(glm::vec3 worldPosition, glm::ivec2 chunkPos) {
    return {worldPosition.x - (chunkPos.x * CHUNK_WIDTH), worldPosition.y, worldPosition.z - (chunkPos.y * CHUNK_DEPTH)};
}
