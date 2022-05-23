#include "World/World.h"

Coordinator entityCoordinator;

void World::loadChunk(Chunk& chunk){
#if READ_CHUNKS
    if (!chunk.isSerialized(savePath)){
        chunk.Generate();
        chunk.Serialize(savePath);
    }
    else{
        chunk.Deserialize(savePath);
    }
#else
    chunk.Generate();
#endif
    chunk.GenerateVertexData();
    chunk.isGeneratedCorrectly = true;

}

static int getNumberOfThreads(int maxThreads){
    int hardwareCon = std::thread::hardware_concurrency();
    int supportedThreads = hardwareCon == 0 ? 2 : hardwareCon;
    int amountOfThreads = supportedThreads > maxThreads ? maxThreads : supportedThreads;
    return amountOfThreads;
}

static void workOnChunks(const std::vector<Chunk*>& chunksToWorkOn, World* world){
    for (Chunk* chunk : chunksToWorkOn){
        if (chunk->isGeneratedCorrectly)
            continue;
        world->loadChunk(*chunk); /// <---- this can be executed at any time
    }
}

World::World(Window *WIN, uint64_t seed, TextureParser textureParser, std::string savePath)
: shader(Shader("../res/shaders/default.vert", "../res/shaders/default.frag")),
  window(WIN),
  seed(seed),
  skyColor({119.0f / 255.0f, 136.0f / 255.0f, 153.0f / 255.0f, 1.0f}),
  w_TextureParser(std::move(textureParser)),
  savePath(std::move(savePath)),
  player(Player(worldSpawn))
  {}

void World::Load() {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //needed to avoid texture bleeding:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 6);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("../res/textures/Atlas.png", &width, &height, &nrChannels, 4);
    if (data)
    {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "[ERROR]: Failed to load Texture Atlas: " << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data);
    shader.Activate();
    shader.uploadMat4("u_trans", glm::mat4(1.0f));
    player.camera.Upload(shader, "u_cam");
    shader.Deactivate();

    entityCoordinator.Init();

    entityCoordinator.RegisterComponent<RigidBody>();
    entityCoordinator.RegisterComponent<Transform>();
    entityCoordinator.RegisterComponent<AABB>();

    physicsSystem = entityCoordinator.RegisterSystem<Physics>();

    //signature for physics system (consists of RigidBody and Transform and AABB)
    Signature signature;
    signature.set(entityCoordinator.GetComponentType<RigidBody>());
    signature.set(entityCoordinator.GetComponentType<Transform>());
    signature.set(entityCoordinator.GetComponentType<AABB>());
    entityCoordinator.SetSystemSignature<Physics>(signature);

    player.Load(); /// WARNING: call after all Coordinator loading, otherwise Segfault!!!!!
    player.SetEquippedBlock(w_TextureParser.getIdByName("oak_plank"));
    //initial update
}

void World::Render() {
    shader.Activate();
    glBindTexture(GL_TEXTURE_2D, texture);
    for (const Chunk& chunk : chunks) {
        shader.uploadVec2("worldPosition", (chunk.chunkPos) * glm::ivec2(CHUNK_WIDTH, CHUNK_DEPTH));
        chunk.Render();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    shader.Deactivate();
}

void World::Delete() {
    for (Chunk& chunk : chunks) {
        chunk.DeleteBuffers();
        chunk.FreeChunkData();
    }
    shader.Delete();
}
void World::Update(float dt) {
    // update physics and player
    physicsSystem->Update(chunks, dt);
    player.Update(dt, chunks);

    time = (sin(0.1f * glfwGetTime()) + 1.2f)/2.0f;
//    time = 1.0f;

    //shader uniforms
    shader.Activate();
    player.camera.Upload(shader, "u_cam");
    shader.uploadFloat("u_time", time);
    shader.uploadVec2("u_resolution", glm::vec2(window->width, window->height));
    shader.uploadVec4("u_bg", skyColor * time);
    shader.Deactivate();
}

void World::GenerateChunks(uint8_t renderDistance) {
    int amountOfThreads = getNumberOfThreads(maxAmountOfThreads);
    std::vector<std::thread> threads(amountOfThreads - 1);
    std::vector<Chunk*> workerTasks[amountOfThreads - 1];

    for (int i = -renderDistance; i <= renderDistance; i++) {
        for (int j = -renderDistance; j <= renderDistance; j++) {
            auto _chunk = Chunk(seed, i, j, w_TextureParser);
            chunks.emplace_back(_chunk);
            loadedChunkPositions.emplace(_chunk.chunkPos);
        }
    }

    for (int i = 0; i < chunks.size(); i++){
        int workingIndex = i % (amountOfThreads - 1);
        workerTasks[workingIndex].emplace_back(&chunks[i]);
    }

    for (int i = 0; i < threads.size(); i++){
        threads[i] = std::thread(workOnChunks, std::ref(workerTasks[i]), this);
    }

    for (std::thread& thread : threads){
        thread.join();
    }

    for (Chunk& chunk : chunks){
        chunk.UploadToGPU();
        chunk.FreeVertexData();
        chunk.isGeneratedCorrectly = true;
        chunk.isUploadedCorrectly = true;
    }
}

void World::RegenerateChunks(uint8_t renderDistance) {
    if (player.prevChunkPos == player.chunkPos)
        return;

    loadedChunkPositions.clear();
    std::vector<int> indicesToLoad;

    int ind = 0;
    for (Chunk& chunk : chunks){
        glm::ivec2 curChunkPos = chunk.chunkPos;
        glm::ivec2 distToPlayer = curChunkPos - player.chunkPos;
        bool isInBounds = (abs(distToPlayer.x) <= renderDistance) && (abs(distToPlayer.y) <= renderDistance);
        if (isInBounds){
            ind++;
            loadedChunkPositions.emplace(chunk.chunkPos);
            continue;
        }
        else {
            chunk.DeleteBuffers();
            indicesToLoad.emplace_back(ind);
            ind++;
        }
    }

    int amountOfThreads = getNumberOfThreads(maxAmountOfThreads);
    std::vector<std::thread> threads(amountOfThreads - 1);
    std::vector<Chunk*> workerTasks[amountOfThreads - 1];

    ind = 0;
    for (int i = -renderDistance + player.chunkPos.x; i <= renderDistance + player.chunkPos.x; i++){
        for (int j = -renderDistance + player.chunkPos.y; j <= renderDistance + player.chunkPos.y; j++) {
            auto iter = loadedChunkPositions.find({i, j});
            if (iter == loadedChunkPositions.end()) {
                Chunk &chunk = chunks[indicesToLoad[ind]];
                ind++;
                chunk.chunkPos = {i, j};
                chunk.isGeneratedCorrectly = false;
                chunk.isUploadedCorrectly = false;
                loadedChunkPositions.emplace(chunk.chunkPos);
            }
        }
    }

    for (int i = 0; i < chunks.size(); i++){
        int workingIndex = i % (amountOfThreads - 1);
        workerTasks[workingIndex].emplace_back(&chunks[i]);
    }

    for (int i = 0; i < threads.size(); i++){
        threads[i] = std::thread(workOnChunks, std::ref(workerTasks[i]), this);
    }

    for (std::thread& thread : threads){
        thread.join();
    }

    for (Chunk& chunk : chunks){
        if (!chunk.isUploadedCorrectly) {
            chunk.UploadToGPU();
            chunk.FreeVertexData();
            chunk.isUploadedCorrectly = true;
            chunk.isUploadedCorrectly = true;
        }
    }
}