#ifndef MINECRAFT_TEXTUREPARSER_H
#define MINECRAFT_TEXTUREPARSER_H
#define YAML_CPP_API
#include "yaml-cpp/yaml.h"
#include <fstream>
#include "glm/glm.hpp"
#include <map>

class TextureParser {
public:
    TextureParser(const std::string& path_to_texUVs, const std::string& path_to_block_config);
    std::vector<std::vector<glm::ivec2>> blockTexPositions;
    int numBlocks;
    YAML::Node blocks;
    int getIdByName(const std::string& name);
private:
    YAML::Node texUVs;
    std::map<std::string, int> names;
};

#endif //MINECRAFT_TEXTUREPARSER_H
