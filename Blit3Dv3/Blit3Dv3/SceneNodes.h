#pragma once

#include "SceneManager.h"
#include "Mesh.h"
#include "DieRoller.h"

class Mesh;

class TranslatorNode : public SceneNode
{
public:
	//need a vector to translate by
	glm::vec3 offset;
	//override Update() so that we can translate
	void Update(float seconds);
	TranslatorNode(glm::vec3 offset, SceneNode *parent);
};

//example node that "draws"
class PrintNode : public SceneNode
{
public:
	void Draw();
	PrintNode();
};

class MeshNode : public SceneNode
{
public:
	Mesh *mesh;	
	MeshNode(SceneNode *parent, Mesh *mesh);
	void Draw();
	void Update(float seconds);
};

class RotatorNode : public SceneNode
{
public:
	float speed;
	float startingAngel;
	float angle;
	glm::vec3 axis;

	void Update(float seconds);
	RotatorNode(float speed, float startingAngel, glm::vec3 axis, float angle, SceneNode *parent);
};

class FloaterNode : public SceneNode
{
public:
	bool active = true;

	float angle = 0.f;
	float radius;
	float timeSoFar = 0.f;
	float totalTime;
	float p = 0.f;
	glm::vec3 pos = glm::vec3(0.f);
	glm::vec3 prevPos;
	glm::vec3 newPos;
	glm::vec3 offset;
	glm::vec3 nextOffset;
	glm::vec3 setVec = glm::vec3(0.f, 0.f, 1.f);

	void Update(float seconds);
	FloaterNode(float radius, float totalTime, SceneNode *parent);
};

class CameraNode : public SceneNode
{
public:							 
								 
	glm::vec3 offset;			 
	glm::vec3 focusPosition;	 
	glm::vec3 cameraPostision;	 		 

	glm::vec4 focusPoint;

	glm::mat4 viewMatrix;

	CameraNode(SceneNode *parent);
	void Update(float seconds);
};
float QuadraticEaseInOut(float p);