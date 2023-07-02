// MtlReader.h
#pragma once
#include <vector>
#include <string>
#include <glm/gtc/type_ptr.hpp>

struct MaterialProps {
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
};



struct MtlContent {
    std::vector<MaterialProps> matProps;
    std::vector<std::string> texNames;
};

class MtlReader {
public:
    MtlReader() = default;
    MtlContent read(const std::string& filepath);
private:
    std::string filepath;
};


