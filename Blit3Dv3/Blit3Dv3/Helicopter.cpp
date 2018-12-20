#include "Helicopter.h"

extern Blit3D *blit3D;

///////////////////////////////////////////
// Author: Hugo Valcourt
// Date:   March 16, 2018
// Github: https://github.com/HugoValcourt
///////////////////////////////////////////

Helicopter::Helicopter(SceneNode *parent, glm::vec3 startingPos, GLSLProgram *shader, float maxPitch, float maxRoll, float moveSpeed)
{

	bladeMesh = new Mesh(blit3D->tManager, shader, "S3DModels\\Helicopter\\Blade.s3d");
	chopperMesh = new Mesh(blit3D->tManager, shader, "S3DModels\\Helicopter\\Chopper.s3d");
	tailRotatorMesh = new Mesh(blit3D->tManager, shader, "S3DModels\\Helicopter\\TailRotor.s3d");

	position = startingPos;
	
	glm::vec3 cameraOffSet = glm::vec3(startingPos.x, startingPos.y - 10.f, startingPos.z + 10.f);

	//Chopper
	mainTranslator = new TranslatorNode(startingPos, parent);
	mainPlacingRotator = new RotatorNode(0.f, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f), 0.f, mainTranslator);
	mainRotator = new RotatorNode(1.f, glm::radians(-90.f), glm::vec3(0.f, 0.f, 1.f), 0.f, mainPlacingRotator);
	pitch = new RotatorNode(1.f, 0.f, glm::vec3(1.f, 0.f, 0.f), 0.f, mainRotator);
	roll = new RotatorNode(1.f, 0.f, glm::vec3(0.f, 1.f, 0.f), 0.f, pitch);
	mainFloaterNode = new FloaterNode(0.5f, 1.f, roll);
	chopperMeshNode = new MeshNode(mainFloaterNode, chopperMesh);

	//Camera position attached on the helicopter
	cameraNode = new CameraNode(roll);

	//Blade
	bladeTranslator = new TranslatorNode(glm::vec3(0.f, 0.6f, 1.4f), chopperMeshNode);
	bladeRotator = new RotatorNode(500.f, 0.f, glm::vec3(0.f, 0.f, 1.f), 0.f, bladeTranslator);
	bladeMeshNode = new MeshNode(bladeRotator, bladeMesh);
	//TailRotor
	tailRotorTranslator = new TranslatorNode(glm::vec3(0.3f, 9.f, 0.f), chopperMeshNode);
	tailRotorRotator = new RotatorNode(500.f, 0.f, glm::vec3(1.f, 0.f, 0.f), 0.f, tailRotorTranslator);
	tailRotatorMeshNode = new MeshNode(tailRotorRotator, tailRotatorMesh);
}


void Helicopter::Update(float seconds, float rightTrigger, float leftTrigger, float leftJoystickLeftRight,
	float rightJoystickUpDown, float rightJoystickLeftRight, float leftJoystickUpDown, float rightBumper, float leftBumper)
{
	float friction = 0.995f;						//World friction
	if (rightTrigger >= 0)							//If the right trigger is halfway pushed or more
	{
		mainFloaterNode->active = false;			//Stop the helicopter from its floating animation
		power = rightTrigger * 0.75f;				//Add power
		bladeRotator->speed = 500.f;				//Make the blade spin
		tailRotorRotator->speed = 500.f;			//Make the blade spin
	}
	else if (leftTrigger >= 0)						//If the left trigger is halfway pushed or more
	{
		mainFloaterNode->active = false;			//Stop the helicopter from its floating animation
		power = -leftTrigger * 0.75f;				//Deduct power
		bladeRotator->speed = 500.f;				//Make the blade spin
		tailRotorRotator->speed = 500.f;			//Make the blade spin
	}
	else										    //Else (no trigger pushed)
	{
		power = 0.f;								//Reset the power to 0
		mainFloaterNode->active = true;				//Make the helicopter look like it's floating
	}

	position = glm::vec3(velocity);					//Make the position where the velocity is currently at

	if (mainTranslator->offset.y <= 0.f)			//If the helicopter has hit the ground
	{
		mainTranslator->offset.y = 0.f;				//It doesn't go through the ground, it stays on top of it
		bladeRotator->speed = 0.f;					//Blade stops moving
		tailRotorRotator->speed = 0.f;				//Blade stops moving
		mainFloaterNode->active = false;			//Stop the helicopter from its floating animation

		//Restablize the helicopter as it hits the ground
		if (pitchAngle > glm::radians(0.f))pitchAngle -= glm::radians(90.f);
		if (rollAngle > glm::radians(0.f))rollAngle -= glm::radians(90.f);
		if (pitchAngle < glm::radians(0.f))pitchAngle += glm::radians(90.f);
		if (rollAngle < glm::radians(0.f))rollAngle += glm::radians(90.f);
		friction = 0.8995f;							//Make it stop faster if it's sliding
	}
	else											//If the helicopter is in the air
	{
		if (rightBumper > 0) angle -= rightBumper; //If right bumpe pressed, turn the helicopter right
		if (leftBumper > 0) angle += leftBumper;   //If left bumpe pressed, turn the helicopter left
		pitchAngle += leftJoystickUpDown / 2;	   //Pitch the helicopter with UP and DOWN on left joystick
		rollAngle -= leftJoystickLeftRight / 2;	   //Roll the helicopter with LEFT and RIGHT on left joystick
		
		if (leftJoystickUpDown == 0)			   //If not pitching anymore,
			pitchAngle *= 0.979f;				   //Restablize the helicopter as it hits the ground
		if (leftJoystickLeftRight == 0)			   //If not rolling anymore, 
			rollAngle *= 0.979f;				   //Restablize the helicopter as it hits the ground

		velocity += (roll->modelMatrix * liftVector * glm::length(velocity)) * seconds; //Calculate pitch and roll on the velocity
	}

	velocity += ((roll->modelMatrix * liftVector * liftForce * power) + (gravity * gravityForce)) * seconds;//Calculate velocity based on pitch, roll, lift force and gravity
	if (glm::length(velocity) >= liftForce) velocity = glm::normalize(velocity) * liftForce;//Speed limit
	velocity *= friction;						   //Gradually decreasing
	
	RotatorAngleLimit(pitchAngle, maxPitch, prevPitchAngle, pitch->angle);//Rotation limit on rotator angles
	RotatorAngleLimit(rollAngle, maxRoll, prevRollAngle, roll->angle);	  //Rotation limit on rotator angles

	mainTranslator->offset += position;			   //Move the whole helicopter

	mainRotator->angle = glm::radians(angle);	   //Rotate the Helicopter to change heading
	pitch->angle = glm::radians(pitchAngle);	   //Rotate the Helicopter to change pitch
	roll->angle = glm::radians(rollAngle);		   //Rotate the Helicopter to change roll
	cameraNode->offset.y += -rightJoystickUpDown;  //Change camera view witch joystick
	cameraNode->offset.z += -rightJoystickLeftRight;//Change camera view witch joystick
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   March 19th 2018
//PURPOSE:Puts a limit of rotation on an angle
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void  Helicopter::RotatorAngleLimit(float &currentAngle, float maxAngle, float &angleStay, float &rotatorAngle)
{
	if (currentAngle <= maxAngle && currentAngle >= -maxAngle)	  //If the angle is still in his range
		angleStay = rotatorAngle = glm::radians(currentAngle);	  //Rotate it and keep track of the previous angle he was at

	else
	{
		rotatorAngle = angleStay;								  //If he is own of his range, make it stay where he is (at max range)
		if (currentAngle > 0)
			currentAngle = maxAngle;							  //Depending on which way it went, make it stay there
		else
			currentAngle = -maxAngle;							  //Depending on which way it went, make it stay there
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//AUTHOR: Hugo Valcourt
//DATE:   March 19th 2018
//PURPOSE:Resets the helicopter to a desired position
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
void Helicopter::ResetPosition(glm::vec3 pos)
{
	mainTranslator->offset = pos;								  //Translate the whole tank to this pos
}