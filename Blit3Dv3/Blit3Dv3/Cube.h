#pragma once
#include "Cube.h"
#include "glslprogram.h"
#include "TextureManager.h"

class Cube
{
public:
	//update variables
	float angle = 0;
	float rotationTimer = 2;
	glm::vec3 pos, axis = glm::vec3(0,0,1);
	//glm::vec4 cubeColor;
	glm::mat4 modelMatrix;
	GLuint vaoID, vboID, eboID, texID;
	GLSLProgram *cubeShader;
	TextureManager* tManager;

	Cube(TextureManager* tManager, GLSLProgram *shaders, std::string filename, GLfloat width, GLfloat height, GLfloat depth);
	~Cube();//vao, vbo
	void Update(float seconds); //Recalculate model matrix
	void Draw();
	void SetAxis(glm::vec3 newAxis);
};