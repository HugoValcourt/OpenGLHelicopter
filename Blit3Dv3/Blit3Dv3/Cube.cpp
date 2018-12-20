#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> //for matrix transformation functions

#include "glslprogram.h"
#include "TextureManager.h"
#include "Cube.h"

#include <stdlib.h>
#include <stdio.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))


Cube::Cube(TextureManager* tManager, GLSLProgram *shaders, std::string filename, GLfloat width, GLfloat height, GLfloat depth)
{
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	cubeShader = shaders;
	//cubeColor = rgba;
	this->tManager = tManager;
	texID = this->tManager->LoadTexture(filename);

	width = width / 2;
	height = height / 2;
	depth = depth / 2;

	// An array of x vectors which represents x vertices
	GLfloat g_vertex_buffer_data[] =
	{
		/// FRONT
		//0 		
		-width, height, depth, 0, 0, 1,//X		Y		Z 	//nX nY nZ			
		//1 		
		-width, -height, depth, 0, 0, 1,//X		Y		Z	//nX nY nZ		
		//2		
		width, height, depth, 0, 0, 1,//X		Y		Z	//nX nY nZ			
		//3		
		width, -height, depth, 0, 0, 1,//X		Y		Z	//nX nY nZ				
		///

		/// BACK
		//4 		
		-width, height, -depth, 0, 0, -1,//X		Y		Z	//nX nY nZ		
		//5 		 
		-width, -height, -depth, 0, 0, -1,//X		Y		Z	//nX nY nZ	
		//6	
		width, height, -depth, 0, 0, -1,//X		Y		Z	//nX nY nZ		
		//7		
		width, -height, -depth, 0, 0, -1,//X		Y		Z	//nX nY nZ		
		///

		/// RIGHT
		//8 		
		width, height, depth, 1, 0, 0,//X		Y		Z	//nX nY nZ
		//9 	
		width, -height, depth, 1, 0, 0,//X		Y		Z	//nX nY nZ			
		//10		
		width, height, -depth, 1, 0, 0,//X		Y		Z	//nX nY nZ		
		//11		
		width, -height, -depth, 1, 0, 0,//X		Y		Z	//nX nY nZ		
		///

		/// LEFT
		//12
		-width, height, depth,-1, 0, 0,//X		Y		Z	//nX nY nZ
		//13		
		-width, -height, depth,-1, 0, 0,//X		Y		Z	//nX nY nZ
		//14		
		-width, height, -depth,-1, 0, 0,//X		Y		Z	//nX nY nZ			
		//15		
		-width, -height, -depth,-1, 0, 0,//X		Y		Z	//nX nY nZ
		///

		/// TOP
		//16 		
		-width, height, -depth,0, 1, 0,//X		Y		Z	//nX nY nZ
		//17		
		-width, height, depth,0, 1, 0,//X		Y		Z	//nX nY nZ			
		//18		
		width, height, depth,0, 1, 0,//X		Y		Z	//nX nY nZ
		//19		
		width, height, -depth,0, 1, 0,//X		Y		Z	//nX nY nZ
		///

		/// BOTTOM
		//20
		-width, -height, -depth,0, -1, 0,//X		Y		Z	//nX nY nZ
		//21
		-width, -height, depth,0, -1, 0,//X		Y		Z	//nX nY nZ
		//22
		width, -height, depth,0, -1, 0,//X		Y		Z	//nX nY nZ	
		//23		
		width, -height, -depth,0, -1, 0,//X		Y		Z	//nX nY nZ
		///
	};

	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vboID);

	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//x,y,z attrib
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(GLfloat),// stride
		(void*)0            // array buffer offset
	);

	//nx, ny, nz attrib
	glVertexAttribPointer(
		1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(GLfloat),// stride
		BUFFER_OFFSET(sizeof(GLfloat) * 3)// array buffer offset
	);


	glEnableVertexAttribArray(0); //channel 0
	glEnableVertexAttribArray(1); //channel 1

								  //indices
	static const GLushort g_indice_buffer_data[] =
	{
		///FRONT
		1,3,0,2,
		0xFF,
		///BACK
		6,4,7,5,
		0xFF,
		///RIGHT
		8,9,10,11,
		0xFF,
		///LEFT
		14,12,15,13,
		0xFF,
		///TOP
		17,18,16,19,
		0xFF,
		///BOTTOM
		23,20,22,21,
		0xFF,
	};

	// Generate 1 buffer, put the resulting identifier in indexbuffer
	glGenBuffers(1, &eboID);//vertex buffered object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_indice_buffer_data), g_indice_buffer_data, GL_STATIC_DRAW);
	//VAO
	glBindVertexArray(0); // Disable our Vertex Array Object? 
						  //VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);// Disable our Array Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);// Disable our Element Object

	return;
}

Cube::~Cube()
{
	// delete VBO when object destroyed
	glDeleteBuffers(1, &vboID);
	glDeleteBuffers(1, &eboID);
	glDeleteVertexArrays(1, &vaoID);
}

void Cube::Update(float seconds)
{
	//if (angle == 0.0f)
	//	rotationTimer -= seconds;

	//if (rotationTimer <= 0)
	//{
	//	angle += seconds * 3;//Recalculate model matrix
	//	
	//	//Only rotate for 1.05 second every 2 seconds
	//	rotationTimer -= seconds;
	//	if (rotationTimer <= -1.05)
	//	{
	//		angle = 0.0f;
	//		rotationTimer = 2;
	//	}
	//}

	//calculate model matrix 
	modelMatrix = glm::mat4(1.f); //identity matrix
								  //note the order of operations here: translate THEN rotate!
	modelMatrix = glm::translate(modelMatrix, pos);
	modelMatrix = glm::rotate(modelMatrix, angle, axis);
}

void Cube::SetAxis(glm::vec3 newAxis)
{
	axis = glm::normalize(newAxis);
}

void Cube::Draw()
{
	//send the modelMatrix to the shader
	cubeShader->setUniform("modelMatrix", modelMatrix);
	//cubeShader->setUniform("cubeColor", cubeColor);
	glBindVertexArray(vaoID);
	// bind the texture
	tManager->BindTexture(texID);

	// Draw the triangles !
	glDrawElements(GL_TRIANGLE_STRIP, 30, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
}