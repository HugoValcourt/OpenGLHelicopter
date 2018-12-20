#pragma once

#include "Blit3D.h"

//this macro helps calculate offsets for VBO stuff
//Pass i as the number of bytes for the offset, so be sure to use sizeof() 
//to help calculate bytes accurately.
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class MeshTest
{
public:
	GLuint vbo[2];
	GLuint vao;
	GLuint numIndices; //how many indices to draw our strip with

	GLSLProgram *meshProg;
	Blit3D * b3d;

	glm::vec3 Kd; //diffuse reflectivity
	glm::vec3 Ka; //ambient reflectivity
	glm::vec3 Ks; //Specular reflectivity

	GLfloat Shininess; //Specular shininess factor

	std::string textureName;

	bool stripped = false; //if true, draw as triangle strip, otherwise draw as triangles

	MeshTest(Blit3D *b);
	~MeshTest();
	void LoadData(std::string filename, std::string filepath);
	void Draw(glm::mat4 *modelMatrix);


};
