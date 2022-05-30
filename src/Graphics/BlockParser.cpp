#include "BlockParser.h"

BlockParser::BlockParser(const std::string& path_to_texUVs, const std::string& path_to_block_config)
{
    texUVs = YAML::LoadFile(path_to_texUVs);

    blocks = YAML::LoadFile(path_to_block_config);
    numBlocks = blocks.size() + 1;

    lightSource.reserve(numBlocks);
    opaqueBlocks.reserve(numBlocks);
    transparentBlocks.reserve(numBlocks);
    solidBlocks.reserve(numBlocks);

    ///The null block
    std::vector<glm::ivec2> nullBlockTexPositions;
    nullBlockTexPositions.reserve(6);
    for (int i = 0; i < 6; i++){
        nullBlockTexPositions[i] = (glm::ivec2(0));
    }
    blockTexPositions.emplace_back(nullBlockTexPositions);

    lightSource.emplace_back(false);
    opaqueBlocks.emplace_back(false);
    transparentBlocks.emplace_back(false);
    solidBlocks.emplace_back(false);

    for (int i = 1; i < numBlocks; i++){
        auto block = blocks[i-1];
        glm::ivec2 top, bottom, side;
        top = {texUVs[block["top"].as<std::string>()]["x"].as<int>(), texUVs[block["top"].as<std::string>()]["y"].as<int>()};
        bottom = {texUVs[block["bottom"].as<std::string>()]["x"].as<int>(), texUVs[block["bottom"].as<std::string>()]["y"].as<int>()};
        side = {texUVs[block["side"].as<std::string>()]["x"].as<int>(), texUVs[block["side"].as<std::string>()]["y"].as<int>()};

        int index = block["id"].as<int>();
        if (index >= numBlocks){
            printf("[ERROR]: Reading of %s failed. ID exceeded limit!", path_to_block_config.c_str());
            exit(1);
        }
        else if (index == 0){
            printf("[ERROR]: Reading of %s failed. ID is 0!", path_to_block_config.c_str());
            exit(1);
        }

        std::vector<glm::ivec2> tempTexPositions;
        tempTexPositions.emplace_back(side);
        tempTexPositions.emplace_back(side);
        tempTexPositions.emplace_back(bottom);
        tempTexPositions.emplace_back(top);
        tempTexPositions.emplace_back(side);
        tempTexPositions.emplace_back(side);

        lightSource.emplace_back(block["isLightSource"].as<bool>());
        opaqueBlocks.emplace_back(block["isOpaque"].as<bool>());
        transparentBlocks.emplace_back(block["isTransparent"].as<bool>());
        solidBlocks.emplace_back(block["isSolid"].as<bool>());

        names.insert(std::pair<std::string, int>(block["name"].as<std::string>(), i));
        blockTexPositions.emplace_back(tempTexPositions);
    }
}

int BlockParser::getIdByName(const std::string& name) {
    return names[name];
}

bool BlockParser::isLightSource(const int id) {
    return lightSource[id];
}

bool BlockParser::isOpaque(const int id) {
    return opaqueBlocks[id];
}

bool BlockParser::isTransparent(const int id) {
    return transparentBlocks[id];
}

bool BlockParser::isSolid(const int id) {
    return solidBlocks[id];
}
