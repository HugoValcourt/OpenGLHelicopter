#pragma once
#pragma once
#include "Blit3D.h"
#include "SceneNodes.h"

///////////////////////////////////////////
// Author: Hugo Valcourt
// Date:   March 16, 2018
// Github: https://github.com/HugoValcourt
///////////////////////////////////////////
class Helicopter
{
private:

	//Nodes for the helicopter
	FloaterNode *mainFloaterNode;

	TranslatorNode *mainTranslator;
	TranslatorNode *bladeTranslator;
	TranslatorNode *tailRotorTranslator;

	RotatorNode *mainPlacingRotator;
	RotatorNode *mainRotator;
	RotatorNode *bladeRotator;
	RotatorNode *tailRotorRotator;
	RotatorNode *pitch;
	RotatorNode *roll;
	RotatorNode *heading;

	MeshNode *bladeMeshNode;
	MeshNode *chopperMeshNode;
	MeshNode *tailRotatorMeshNode;

	Mesh *bladeMesh;
	Mesh *chopperMesh;
	Mesh *tailRotatorMesh;

	glm::vec4 gravity = glm::vec4(0.f, -1.f, 0.f, 0.f);//Where is gravity pulling?
	glm::vec4 liftVector = glm::vec4(0.f, 0.f, 1.f, 0.f);//Were is upward?
	glm::vec4 velocity = glm::vec4(0.f, 0.f, 0.f, 0.f);//We move the helicopter with this force

	float gravityForce = 0.90f;		//How heavy is the gravity?
	float power = 0.5f;				//Speed of the helicopter
	float liftForce = 1.f;			//The force of the upward force "from the blade"
	float angle = 0.f;				//Angle of the whole helicopter
	float pitchAngle = 0.f;			//Anlge of helicopter's pitch
	float rollAngle = 0.f;			//Anlge of helicopter's roll
	float maxPitch = 45.f;
	float maxRoll = 45.f;
	float prevPitchAngle = 0.f;
	float prevRollAngle = 0.f;

	bool moving = false;			//Are we moving?

	void RotatorAngleLimit(float &currentAngle, float maxAngle, float &angleStay, float &rotatorAngle);
public:
	CameraNode * cameraNode;

	glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);

	Helicopter(SceneNode *parent, glm::vec3 startingPos, GLSLProgram *shader, float maxPitch, float maxRoll, float moveSpeed);
	void Update(float seconds, float rightTrigger, float leftTrigger, float leftJoystickLeftRight, 
		 float rightJoystickUpDown, float rightJoystickLeftRight, float leftJoystickUpDown, float rightBumper, float leftBumper);
	void ResetPosition(glm::vec3 pos);//Reset the position of the helicopter when pressed 'R' when running the program
};