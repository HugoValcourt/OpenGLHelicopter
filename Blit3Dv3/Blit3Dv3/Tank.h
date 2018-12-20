#pragma once
#include "Blit3D.h"
#include "SceneNodes.h"
class Tank
{
private:

	TranslatorNode *mainTranslator;
	TranslatorNode *turretTranslator;
	TranslatorNode *gunTranslator;
	TranslatorNode *frontWheel1Translator;
	TranslatorNode *frontWheel2Translator;
	TranslatorNode *backWheel1Translator;
	TranslatorNode *backWheel2Translator;

	RotatorNode *placingRotator;
	RotatorNode *placingRotator2;
	RotatorNode *mainRotator;
	RotatorNode *turrentRotator;
	RotatorNode *gunPlacingRotator;
	RotatorNode *gunRotator;
	RotatorNode *frontWheel1PlacingRotator;
	RotatorNode *frontWheel1TurnRotator;
	RotatorNode *frontWheel1RollRotator;
	RotatorNode *frontWheel2PlacingRotator;
	RotatorNode *frontWheel2TurnRotator;
	RotatorNode *frontWheel2RollRotator;
	RotatorNode *backWheel2PlacingRotator;
	RotatorNode *backWheel2RollRotator;
	RotatorNode *backWheel1PlacingRotator;
	RotatorNode *backWheel1RollRotator;

	MeshNode *hullMeshNode;
	MeshNode *turretMeshNode;
	MeshNode *gunMeshNode;
	MeshNode *frontWheel1MeshNode;//Positive	1	2
	MeshNode *frontWheel2MeshNode;//Negative
	MeshNode *backWheel1MeshNode; //Positive	1	2
	MeshNode *backWheel2MeshNode; //Negative

	Mesh *hullMesh;
	Mesh *turretMesh;
	Mesh *gunMesh;
	Mesh *wheelMesh;

	float wheelTurnLimit = 2.5f;	//A limit for turning the wheels
	float gunLimit = 20.f;			//Range for the rotation of the gun
	float moveSpeed = 2.f;			//Move speed of the actual tank
	float turnSpeed = 700.f;		//Move speed of the actual tank
	float prevGunAngle = 0.f;		//Used for max point of gun rotating

	glm::vec3 facing;
	glm::vec3 velocity;
	int moveFoB = 0;				//Are we moving forward or backward? 1 or -1
	float wheelAngle = 0.f;			//Turning the wheels
	float gunAngle = 0.f;			//Rotating the gun
	float turretAngle = 0.f;		//Rotating the turret
	float wheelSpinAngle = 0.f;		//Spinning the wheel when moving
	float angle = 0.f;				//Angle of the whole tank
	bool moving = false;			//Are we moving?
public:


	Tank(SceneNode *parent, glm::vec3 startingPos, GLSLProgram *shader, float wheelTurnLimit, float gunLimit, float moveSpeed);
	void Update(float seconds, float rightBumper, float leftBumper, float leftJoystickLeftRight, float rightJoystickLeftRight, float rightJoystickUpDown);
	void ResetPosition(glm::vec3 pos);//Reset the position of the tank when pressed 'R' when running the program
	void AngleChecking(float &angle);
};