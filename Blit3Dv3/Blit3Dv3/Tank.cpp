#include "Tank.h"

extern Blit3D *blit3D;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   February 26th 2018
//PURPOSE:Make an actual 3D tank by using nodes
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Tank::Tank(SceneNode *parent, glm::vec3 startingPos, GLSLProgram *shader, float wheelTurnLimit, float gunLimit, float moveSpeed)
{
	this->gunLimit = gunLimit;
	this->moveSpeed = moveSpeed;
	this->wheelTurnLimit = wheelTurnLimit;
	turnSpeed = std::pow(50, moveSpeed);									//Base the turning speed on how fast the tank is going

	hullMesh = new Mesh(blit3D->tManager, shader, "S3DModels\\Tank\\hull.s3d");
	turretMesh = new Mesh(blit3D->tManager, shader, "S3DModels\\Tank\\turret.s3d");
	gunMesh = new Mesh(blit3D->tManager, shader, "S3DModels\\Tank\\gun.s3d");
	wheelMesh = new Mesh(blit3D->tManager, shader, "S3DModels\\Tank\\wheel.s3d");

	//Main parts
	mainTranslator = new TranslatorNode(startingPos, parent);
	placingRotator = new RotatorNode(0.f, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f), 0.f, mainTranslator);
	placingRotator2 = new RotatorNode(0.f, glm::radians(180.f), glm::vec3(0.f, 0.f, 1.f), 0.f, placingRotator);
	mainRotator = new RotatorNode(1.f, 0.f, glm::vec3(0.f, 0.f, 1.f), 0.f, placingRotator2);
	hullMeshNode = new MeshNode(mainRotator, hullMesh);

	//Wheel 1 front: Parent = Hull
	frontWheel1Translator = new TranslatorNode(glm::vec3(-2.f, 1.75f, -1.6f), hullMeshNode);
	frontWheel1PlacingRotator = new RotatorNode(0.f, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f), 0.f, frontWheel1Translator);
	frontWheel1TurnRotator = new RotatorNode(1.f, glm::radians(0.f), glm::vec3(1.f, 0.f, 0.f), 0.f, frontWheel1PlacingRotator);
	frontWheel1RollRotator = new RotatorNode(1.f, glm::radians(0.f), glm::vec3(0.f, 0.f, 1.f), 0.f, frontWheel1TurnRotator);
	frontWheel1MeshNode = new MeshNode(frontWheel1RollRotator, wheelMesh);

	//Wheel 2 front: Parent = Hull
	frontWheel2Translator = new TranslatorNode(glm::vec3(2.f, 1.75f, -1.6f), hullMeshNode);
	frontWheel2PlacingRotator = new RotatorNode(0.f, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f), 0.f, frontWheel2Translator);
	frontWheel2TurnRotator = new RotatorNode(1.f, glm::radians(0.f), glm::vec3(1.f, 0.f, 0.f), 0.f, frontWheel2PlacingRotator);
	frontWheel2RollRotator = new RotatorNode(1.f, glm::radians(0.f), glm::vec3(0.f, 0.f, 1.f), 0.f, frontWheel2TurnRotator);
	frontWheel2MeshNode = new MeshNode(frontWheel2RollRotator, wheelMesh);
	
	//Wheel 1 back: Parent = Hull
	backWheel1Translator = new TranslatorNode(glm::vec3(-2.f, -2.25f, -1.6f), hullMeshNode);
	backWheel1PlacingRotator = new RotatorNode(0.f, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f), 0.f, backWheel1Translator);
	backWheel1RollRotator = new RotatorNode(1.f, glm::radians(0.f), glm::vec3(0.f, 0.f, 1.f), 0.f, backWheel1PlacingRotator);
	backWheel1MeshNode = new MeshNode(backWheel1RollRotator, wheelMesh);

	//Wheel 2 back: Parent = Hull
	backWheel2Translator = new TranslatorNode(glm::vec3(2.f, -2.25f, -1.6f), hullMeshNode);
	backWheel2PlacingRotator = new RotatorNode(0.f, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f), 0.f, backWheel2Translator);
	backWheel2RollRotator = new RotatorNode(1.f, glm::radians(0.f), glm::vec3(0.f, 0.f, 1.f), 0.f, backWheel2PlacingRotator);
	backWheel2MeshNode = new MeshNode(backWheel2RollRotator, wheelMesh);

	//Turret: Parrent = Hull 
	turretTranslator = new TranslatorNode(glm::vec3(0.f, 0.f, 2.f), hullMeshNode);
	turrentRotator = new RotatorNode(1.f, glm::radians(0.f), glm::vec3(0.f, 0.f, 1.f), 0.f, turretTranslator);
	turretMeshNode = new MeshNode(turrentRotator, turretMesh);

	//Gun: Parent = Turret
	gunTranslator = new TranslatorNode(glm::vec3(0.f, 1.7f, 0.f), turretMeshNode);
	gunPlacingRotator = new RotatorNode(0.f, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f), 0.f, gunTranslator);
	gunRotator = new RotatorNode(1.f, glm::radians(0.f), glm::vec3(0.f, 0.f, 1.f), 0.f, gunPlacingRotator);
	gunMeshNode = new MeshNode(gunRotator, gunMesh);

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   February 26th 2018
//PURPOSE:Move the tank forward and backward. Also
//		  rotate stuff on it like; wheels, turret and gun
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void Tank::Update(float seconds, float rightBumper, float leftBumper, float leftJoystickLeftRight, float rightJoystickLeftRight, float rightJoystickUpDown)
{

	//Go forward
	if (rightBumper > 0.f)
	{
		moving = true;
		velocity.z += rightBumper * -1;//Add the z velocity to go forward
		wheelSpinAngle += rightBumper; //Make the wheel spin
		angle += leftJoystickLeftRight * -1; //Turning the tank
		moveFoB = 1;
	}
	//Go backward
	else if (leftBumper > 0.f)
	{
		moving = true;
		velocity.z += leftBumper; //Add the z velocity to go backward
		wheelSpinAngle += leftBumper * -1; //Make the wheel spin
		angle += leftJoystickLeftRight; //Turning the tank
		moveFoB = -1;
	}
	else
		velocity.z = 0.f;

	//Rotate stuff
	wheelAngle = leftJoystickLeftRight; //Turn wheel
	gunAngle += rightJoystickUpDown; //Rotate gun
	turretAngle += rightJoystickLeftRight; //Rotate turret

	AngleChecking(angle);
	AngleChecking(wheelSpinAngle);
	AngleChecking(turretAngle);

	if (moving)														//If where moving
	{
		moving = false;												//Reset the next update to not moving if the case
		float radians = -(angle - 90.f) * (M_PI / 180.f);			//Get a radians with the angle of the tank with a 90 degrees offset
		facing.x = std::cos(radians);								//Calculate which way the tank is facing
		facing.z = std::sin(radians);
		velocity += facing * 700.f;									//Apply the facing vector to the velocity vector with a scale (Turning scale)
		velocity = glm::normalize(velocity) * moveSpeed;
		mainTranslator->offset += velocity * (moveSpeed * moveFoB) * seconds;
	}
	mainRotator->angle = glm::radians(angle);				      //Rotate the tank

	//Rotate the wheel for rolling and turning
	frontWheel1TurnRotator->angle = -wheelAngle / wheelTurnLimit; //Front wheel turning 
	frontWheel2TurnRotator->angle = wheelAngle / wheelTurnLimit;  //Front wheel turning 
	frontWheel1RollRotator->angle = glm::radians(wheelSpinAngle); //Wheel rolling
	frontWheel2RollRotator->angle = glm::radians(-wheelSpinAngle);//Wheel rolling
	backWheel1RollRotator->angle = glm::radians(wheelSpinAngle);  //Wheel rolling
	backWheel2RollRotator->angle = glm::radians(-wheelSpinAngle); //Wheel rolling


	if (gunAngle <= gunLimit + 10.f && gunAngle >= -gunLimit)	  //If the rotating gun is still in his range
		prevGunAngle = gunRotator->angle = glm::radians(-gunAngle);//Rotate it and keep track of the previous angle he was at
	else
	{
		gunRotator->angle = prevGunAngle;						  //If he is own of his range, make it stay where he is (at max range)
		if(gunAngle > 0)
			gunAngle = gunLimit + 10.f;							  //If the gun as reached the highest point of the range, stay there
		else
			gunAngle = -gunLimit;							      //If the gun as reached the lowesst point of the range, stay there
	}

	turrentRotator->angle = glm::radians(turretAngle);			  //Rotate the turret 

}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   February 28th 2018
//PURPOSE:Resets the tank to a desired position
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void Tank::ResetPosition(glm::vec3 pos)
{
	mainTranslator->offset = pos;//Translate the whole tank to this pos
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   February 28th 2018
//PURPOSE:Checks and manage angle to stay between 0 and 360
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void Tank::AngleChecking(float &angle)
{
	if (angle >= 360)
		angle = 0;
}
