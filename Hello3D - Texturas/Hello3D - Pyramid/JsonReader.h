#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>


using json = nlohmann::json;
using namespace std;
//using namespace glm;

struct Rotation {
    bool rotateX;
    bool rotateY;
    bool rotateZ;
};

struct ObjectConfigs {
    string file;
    glm::vec3 initialPosition;
    Rotation rotation;
};

struct Camera {
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
};

struct Frustum {
    float fov;
    float aspect_ratio;
    float znear;
    float zfar;

};

struct Scene {
    Frustum frustum;
    Camera camera;
    vector<ObjectConfigs> OBJs;
};


class JsonReader {
public:
    Scene readJson(const string& filePath);

};

