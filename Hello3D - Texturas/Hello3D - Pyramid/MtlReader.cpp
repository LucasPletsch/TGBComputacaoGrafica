#include "MtlReader.h"
#include <iostream>
#include <fstream>
#include <sstream>



MtlContent MtlReader::read(const std::string& filepath) {
    MtlContent content;

    std::ifstream file(filepath);
    std::string line;
    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word;
        if (word == "newmtl") {
            content.matProps.push_back(MaterialProps());
        }
        else if (word == "map_Kd") {
            std::string texName;
            iss >> texName;
            content.texNames.push_back(texName);
        }
        else if (word == "Ka" || word == "Kd" || word == "Ks") {
            float x, y, z;
            iss >> x >> y >> z;
            if (word == "Ka")
                content.matProps.back().ka = { x, y, z };
            else if (word == "Kd")
                content.matProps.back().kd = { x, y, z };
            else if (word == "Ks")
                content.matProps.back().ks = { x, y, z };
        }
    }

    return content;
}
