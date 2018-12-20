#define _USE_MATH_DEFINES
#include <math.h>
#include "SceneNodes.h"
#include "DieRoller.h"
#include <glm/gtc/matrix_transform.hpp>

//for the PrintNode
#include <iostream>

TranslatorNode::TranslatorNode(glm::vec3 offset, SceneNode *parent)
{
	typeId = sceneNodeType::TRANSLATE;
	this->offset = offset;
	this->parent = parent;
	this->parent->children.push_back(this);
}

void TranslatorNode::Update(float seconds)
{
	assert(parent != NULL && "parent pointer is NULL");	
	//translate
	snodeMutex.lock();	
	parent->snodeMutex.lock();
	modelMatrix = glm::translate(parent->modelMatrix, offset);
	parent->snodeMutex.unlock();
	snodeMutex.unlock();

	//update all of our children
	for(auto n : children)
	{
		n->Update(seconds);
	}
}

PrintNode::PrintNode()
{
	typeId = sceneNodeType::PRINT;

	modelMatrix = glm::mat4(1.f);
	parent = NULL;
}

void PrintNode::Draw()
{
	//we would normally use our modelMatrix to draw, so let's calculate where we 
	//would be in the world based on it
	glm::vec4 position = glm::vec4(0, 0, 0, 1);
	
	snodeMutex.lock();
	position = modelMatrix * position;
	snodeMutex.unlock();

	std::cout << "Drawing at: " << position.x << ", " << position.y << ", " << position.z << std::endl;
	//draw all of our children
	for(auto n : children)
	{
		n->Draw();
	}
}

MeshNode::MeshNode(SceneNode *parent, Mesh *mesh)
{
	typeId = sceneNodeType::MESH;
	this->parent = parent;
	this->parent->children.push_back(this);
	this->mesh = mesh;
	this->modelMatrix = parent->modelMatrix;
}

void MeshNode::Draw()
{
	mesh->Draw(&this->modelMatrix);

	//draw all of our children
	for (auto n : children)
	{
		n->Draw();
	}
}

void MeshNode::Update(float seconds)
{
	snodeMutex.lock();
	parent->snodeMutex.lock();
	modelMatrix = parent->modelMatrix;
	parent->snodeMutex.unlock();
	snodeMutex.unlock();

	//update all of our children
	for (auto n : children)
	{
		n->Update(seconds);
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   February 22th 2018
//PURPOSE:Constructor for rotating a node on an axis
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
RotatorNode::RotatorNode(float speed, float startingAngel, glm::vec3 axis,  float angle, SceneNode *parent)
{
	typeId = sceneNodeType::ROTATE;

	this->speed = glm::radians(speed);
	this->startingAngel = startingAngel;
	this->axis = axis;
	this->angle = angle;
	this->parent = parent;
	this->parent->children.push_back(this);
	this->modelMatrix = parent->modelMatrix;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   February 22th 2018
//PURPOSE:Rotate a node on an axis
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void RotatorNode::Update(float seconds)
{
	if (speed != 0)
	{
		angle += speed * seconds;
		if(angle > M_PI * 2)
			angle -= M_PI * 2;
		if (angle < -M_PI * 2)
			angle += M_PI * 2;

		//calculate model matrix
		snodeMutex.lock();
		parent->snodeMutex.lock();
		modelMatrix = glm::rotate(parent->modelMatrix, angle, axis);//apply modelMatrix from our parent	
		parent->snodeMutex.unlock();
		snodeMutex.unlock();

	}
	else
	{
		//calculate model matrix
		snodeMutex.lock();
		parent->snodeMutex.lock();
		modelMatrix = glm::rotate(parent->modelMatrix, startingAngel, axis);//apply modelMatrix from our parent	
		parent->snodeMutex.unlock();
		snodeMutex.unlock();
	}
	//update all of our children
	for (auto n : children)
	{
		n->Update(seconds);
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   February 22th 2018
//PURPOSE:Constructor for a floater node that floats in a 
//		  sphere radius
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
FloaterNode::FloaterNode(float radius, float totalTime, SceneNode *parent)
{
	//Initialisation of data
	this->radius = radius;
	this->totalTime = totalTime;
	this->parent = parent;
	this->parent->children.push_back(this);

	//First position to go toward
	newPos.x = RollDNFloat(-180, 180);
	newPos.y = RollDNFloat(-180, 180);
	newPos.z = RollDNFloat(-180, 180);
	newPos = glm::normalize(newPos) * radius; //Scale the vector with the radius
	prevPos = glm::vec3(0.f, 0.f, 0.f);//Set the initial previous position to 0
	offset = newPos - prevPos;//The node will follow this vector to go tawrd the new position
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   February 22th 2018
//PURPOSE:Floater node that floats in a 
//		  sphere radius
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void FloaterNode::Update(float seconds)
{
	snodeMutex.lock();
	parent->snodeMutex.lock();
	modelMatrix = parent->modelMatrix;//get modelMatrix from our parent	
	parent->snodeMutex.unlock();
	snodeMutex.unlock();

	if (active)
	{
		if (timeSoFar >= totalTime)//If we have reached the point
		{
			prevPos = newPos;	   //Set the previous position to the current one
			newPos = -prevPos;	   //The next position will be the current one but negated														

			glm::vec3 rotatingVec = glm::cross(-prevPos, setVec);//Make a vector to rotate around	

			glm::mat4 tempMat(1.f);//Temp mat4 for rotation math

			tempMat = glm::rotate(tempMat, glm::radians(RollDNFloat(-40, 40)), rotatingVec);//Rotate around the vector with an angle between -40 and 40
			newPos = glm::vec3(glm::vec4(newPos, 0.f) * tempMat);//Apply the matrix with rotation

			offset = newPos - prevPos;//New offset towards the new point
			timeSoFar = 0.f;//Reset time
			p = 0.f;//Reset percent
		}

		p = QuadraticEaseInOut(timeSoFar / totalTime);//Get a percentage with quadratic ease in out of how much we translate the node

		pos = prevPos + offset * p;//Change the position of the node 
		modelMatrix = glm::translate(modelMatrix, pos);//Apply the new position

		timeSoFar += seconds;//Add delta time to the current time
	}
	//update all of our children 
	for (auto n : children)
	{
		n->Update(seconds);
	}
}

float QuadraticEaseInOut(float p)
{
	if (p < 0.5)
	{
		return 2 * p * p;
	}
	else
	{
		return (-2 * p * p) + (4 * p) - 1;
	}
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   March 28th, 2018
//PURPOSE:Constructor for a camera node
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
CameraNode::CameraNode(SceneNode *parent)
{
	typeId = sceneNodeType::CAMERA;
	this->parent = parent;
	this->parent->children.push_back(this);

	//Initialisation of offsets for camera
	offset = glm::vec3(0.f, 20.f, 10.f);
	focusPoint = glm::vec4(0, -10, 0, 1);
	focusPosition = glm::vec3(0, 0, 0);
	cameraPostision = glm::vec3(0, 0, 0);
}

void CameraNode::Update(float seconds)
{
	snodeMutex.lock();
	parent->snodeMutex.lock();

	cameraPostision = glm::vec3(parent->modelMatrix * glm::vec4(offset, 1));//Set the camera position with an offset based on the parent's model matrix
	focusPosition = glm::vec3(parent->modelMatrix * focusPoint);//Set the focus point based on the parent's model matrix
	modelMatrix = parent->modelMatrix;//get modelMatrix from our parent	

	parent->snodeMutex.unlock();
	snodeMutex.unlock();
	
	viewMatrix = glm::lookAt(cameraPostision, focusPosition, glm::vec3(0, 1, 0));//Set where we are looking based on calculation form above
	
	//update all of our children 
	for (auto n : children)
	{
		n->Update(seconds);
	}
}
