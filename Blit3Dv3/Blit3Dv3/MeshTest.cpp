#include "MeshTest.h"
#include <iostream>
#include <fstream>

MeshTest::MeshTest(Blit3D *b)
{
	vao = 0;;
	numIndices = 30;
	meshProg = NULL;
	b3d = b;
}

MeshTest::~MeshTest()
{
	//free the vbo's
	glDeleteBuffers(2, vbo);
	//free the vao
	glDeleteVertexArrays(1, &vao);
}

void MeshTest::LoadData(std::string filename, std::string filepath)
{
	int numVerts=0;
	numIndices = 0;
	GLuint *indices;
	//make some verts, in form x,y,z,nx,ny,nz,u,v
	float *points;
	//open our file to read the data
	std::ifstream myfile;
	myfile.open(filename.c_str());

	if(myfile.is_open())
	{
		if(myfile.good())
		{
			myfile >> numVerts;

			points = new float[numVerts * 8]; //make an array of Textured Vertices

			for(int i = 0; i < numVerts * 8; ++i)
			{
				myfile >> points[i];
			}

			myfile >> numIndices;

			indices = new GLuint[numIndices];

			for(int i = 0; i < numIndices; ++i)
			{
				myfile >> indices[i];
			}
			
			myfile >> textureName;
			textureName = filepath + textureName;

			//load the texture via the texture manager
			b3d->tManager->LoadTexture(textureName);
		}
		myfile.close();
	}
	else
	{
		assert("Error reading S3D file");
	}
	
	
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, vbo); //make two vbo's
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //bind first vbo
	glBufferData(GL_ARRAY_BUFFER, numVerts * 8 * sizeof(float), points, GL_STATIC_DRAW); //upload data to vbo #1

	
	glEnableVertexAttribArray(0); //enable position attribute
	glEnableVertexAttribArray(1); //enable normal attribute
	glEnableVertexAttribArray(2); //enable texture uv attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, NULL);//position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, BUFFER_OFFSET(sizeof(float)* 3));//normal
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 8, BUFFER_OFFSET(sizeof(float)* 6));//uv

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]); //bind second vbo
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW); //upload data to vbo #2

	glBindVertexArray(0); // Disable our Vertex Array Object? 
	glBindBuffer(GL_ARRAY_BUFFER, 0);// Disable our Vertex Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);// Disable our Vertex Buffer Object


	Kd = glm::vec3(1.0f, 1.0f, 0.2f); //diffuse reflectivity
	Ka = glm::vec3(0.1f, 0.1f, 0.2f); //ambient reflectivity
	Ks = glm::vec3(1.0f, 1.0f, 1.0f); //Specular reflectivity
	Shininess = 1.0f; //Specular shininess factor

	delete points;
	delete indices;
}

void MeshTest::Draw(glm::mat4 *modelMatrix)
{
	glBindVertexArray(vao);
	b3d->tManager->BindTexture(textureName);
	meshProg = b3d->sManager->UseShader("lighting.vert", "lighting.frag");
	meshProg->setUniform("Kd", Kd);
	meshProg->setUniform("Ka", Ka);
	meshProg->setUniform("Ks", Ks);
	meshProg->setUniform("Shininess", Shininess);	
	meshProg->setUniform("modelMatrix", modelMatrix);//send the modelMatrix to the shader

	// draw indices from the currently bound VAO with current in-use shader
	if(stripped) glDrawElements(GL_TRIANGLE_STRIP, numIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	else glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	glBindVertexArray(0);

}