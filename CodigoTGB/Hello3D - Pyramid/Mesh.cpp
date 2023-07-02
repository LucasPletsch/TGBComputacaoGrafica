#include "Mesh.h"

void Mesh::initialize(GLuint VAO, int nVertices, Shader* shader, GLuint texID)
{
	this->VAO = VAO;
	this->nVertices = nVertices;
	this->shader = shader;
	this->texID = texID;
}

void Mesh::update()
{
	
}

void Mesh::draw()
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, nVertices);
	glBindVertexArray(0);
}
