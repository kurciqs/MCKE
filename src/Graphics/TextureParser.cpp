#include "TextureParser.h"

TextureParser::TextureParser(const std::string& path_to_texUVs, const std::string& path_to_block_config)
{
    texUVs = YAML::LoadFile(path_to_texUVs);

    blocks = YAML::LoadFile(path_to_block_config);
    numBlocks = blocks.size() + 1;

    ///The null block
    std::vector<glm::ivec2> nullBlockTexPositions;
    nullBlockTexPositions.reserve(6);
    for (int i = 0; i < 6; i++){
        nullBlockTexPositions[i] = (glm::ivec2(0));
    }
    blockTexPositions.emplace_back(nullBlockTexPositions);
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

        names.insert(std::pair<std::string, int>(block["name"].as<std::string>(), i));
        blockTexPositions.emplace_back(tempTexPositions);
    }
}

int TextureParser::getIdByName(const std::string& name) {
    int index = names[name];
    return index;
}
