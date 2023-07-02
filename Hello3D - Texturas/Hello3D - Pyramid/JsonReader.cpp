#include "JsonReader.h"

Scene JsonReader::readJson(const string& filePath) {
    Scene scene;
  
    ifstream i(filePath);
    json j;
    i >> j;

    scene.frustum.fov = j["scene"]["frustum"]["fov"];
    scene.frustum.aspect_ratio = j["scene"]["frustum"]["aspect_ratio"];
    scene.frustum.znear = j["scene"]["frustum"]["near"];
    scene.frustum.zfar = j["scene"]["frustum"]["far"];

    scene.camera.position = glm::vec3(j["scene"]["camera"]["position"]["x"], j["scene"]["camera"]["position"]["y"], j["scene"]["camera"]["position"]["z"]);
    scene.camera.target = glm::vec3(j["scene"]["camera"]["target"]["x"], j["scene"]["camera"]["target"]["y"], j["scene"]["camera"]["target"]["z"]);
    scene.camera.up = glm::vec3(j["scene"]["camera"]["up"]["x"], j["scene"]["camera"]["up"]["y"], j["scene"]["camera"]["up"]["z"]);

    for (auto& object : j["scene"]["objects"]) {
        ObjectConfigs obj;
        obj.file = object["file"];
        obj.initialPosition = glm::vec3(object["initialPosition"][0], object["initialPosition"][1], object["initialPosition"][2]);
        obj.rotation.rotateX = object["rotation"]["rotateX"];
        obj.rotation.rotateY = object["rotation"]["rotateY"];
        obj.rotation.rotateZ = object["rotation"]["rotateZ"];
        scene.OBJs.push_back(obj);
    }

    return scene;
}
