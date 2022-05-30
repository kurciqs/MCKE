#ifndef MINECRAFT_BLOCKPARSER_H
#define MINECRAFT_BLOCKPARSER_H
#define YAML_CPP_API
#include "yaml-cpp/yaml.h"
#include <fstream>
#include "glm/glm.hpp"
#include <unordered_map>

class BlockParser {
public:
    BlockParser(const std::string& path_to_texUVs, const std::string& path_to_block_config);
    std::vector<std::vector<glm::ivec2>> blockTexPositions;
    int numBlocks;
    YAML::Node blocks;
    int getIdByName(const std::string& name);
    bool isLightSource(const int id);
    bool isOpaque(const int id);
    bool isTransparent(const int id);
    bool isSolid(const int id);
private:
    std::vector<bool> lightSource;
    std::vector<bool> opaqueBlocks;
    std::vector<bool> transparentBlocks;
    std::vector<bool> solidBlocks;
    YAML::Node texUVs;
    std::unordered_map<std::string, int> names;
};

#endif //MINECRAFT_BLOCKPARSER_H
