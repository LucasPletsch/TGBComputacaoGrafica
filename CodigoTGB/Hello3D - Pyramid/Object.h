#pragma once

#include <stb_image.h>
#include <vector>
#include "Mesh.h"
#include "MtlReader.h"

using namespace std;

class Object
{
public:
	Object() {}
	void initialize(string filePath, Shader* shader, glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1), float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 0.0, 1.0));
	void update();
	void draw();

	//Código TGB: Declaração de variáveis de transformação
	glm::vec3 rotation;
	glm::vec3 translation;
	glm::vec3 position;
	glm::vec3 scale;
	float angle;
	glm::vec3 axis;
	//Código TGB: variável que armazena o resultado da leitura do arquivo mtl
	MtlContent mtlContent;

protected:
	void loadObj(string filePath);
	int generateTexture(string filePath);
	void setMaterialPropertiesOnShader(Shader* shader, const MaterialProps& matProps);
	GLuint generateVAO(vector <GLfloat> vertbuffer, int& nVertices);
	vector <Mesh> grupos;
	Shader* shader;

private:
	std::string getMTLFilePath(const std::string& caminhoObj);


};

