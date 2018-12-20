#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Joystick.h"
#include "Blit3DBaseFiles\Blit3D\Blit3D.h"

std::mutex joystickMutex;

extern Blit3D *blit3D;

bool Joystick::Init()
{
	//check for joysticks, stop at first joystick found plugged in
	for (joystickNumber = GLFW_JOYSTICK_1; joystickNumber <= GLFW_JOYSTICK_LAST; ++joystickNumber)
	{
		if (glfwJoystickPresent(joystickNumber))
		{
			foundJoystick = true;
			break;
		}
	}

	if (foundJoystick)
	{
		//get the state arrays for this joystick
		if (!PollJoystick()) foundJoystick = false;
	}

	return foundJoystick;
}

void Joystick::ProcessJoystickAxis(float &joystickAxis, const Configuration configuration)
{
	//deadzone

	if (joystickAxis >= -configuration.deadzone &&
		joystickAxis <= configuration.deadzone) joystickAxis = 0.f;
	else
	{
		if (joystickAxis < 0)
			joystickAxis -= configuration.deadzone;
		else
			joystickAxis += configuration.deadzone;

		//scale back into range 0 ... +/-1
		joystickAxis /= (1.f - configuration.deadzone);

		//scaling

		//squared response curve
		/*if (joystickAxis < 0)
		joystickAxis = -joystickAxis * joystickAxis;
		else
		joystickAxis = joystickAxis * joystickAxis;*/

		//cubed response curve
		joystickAxis = joystickAxis * joystickAxis * joystickAxis;
	}
}

bool Joystick::PollJoystick()
{
	if (!glfwJoystickPresent(joystickNumber)) return false;

	//first fetch axis states
	joystickState.axisStates = glfwGetJoystickAxes(joystickNumber, &joystickState.axisCount);
	//now fetch button states
	joystickState.buttonStates = glfwGetJoystickButtons(joystickNumber, &joystickState.buttonCount);

	return true;
}

void Joystick::DoJoystick(void)
{
	if (foundJoystick)
	{

		if (PollJoystick())
		{
			//joystick at joystickNumber is still plugged in, so we can work with it's states
			joystickMutex.lock();
			//axis states are between -1.f and 1.f
			if (joystickState.axisCount > 0) leftJoystickLeftRight = joystickState.axisStates[0];
			if (joystickState.axisCount > 1) leftJoystickUpDown = joystickState.axisStates[1];
			if (joystickState.axisCount > 2) rightJoystickLeftRight = -joystickState.axisStates[2];
			if (joystickState.axisCount > 3) rightJoystickUpDown = joystickState.axisStates[3];
			if (joystickState.axisCount > 4) leftTrigger = joystickState.axisStates[4];
			if (joystickState.axisCount > 5) rightTrigger = joystickState.axisStates[5];

			if (joystickState.buttonCount > 6)
			{
				//If back button pressed, exit the program
				if (joystickState.buttonStates[6] == GLFW_PRESS)
					blit3D->Quit();

				//Input from bumpers
				if (joystickState.buttonStates[4] == GLFW_PRESS)
					leftBumper = 1.f; //There is input on left bumper
				else if (joystickState.buttonStates[4] == GLFW_RELEASE)
					leftBumper = 0.f;

				if (joystickState.buttonStates[5] == GLFW_PRESS)
					rightBumper = 1.f; //There is input on right bumper
				else if (joystickState.buttonStates[5] == GLFW_RELEASE)
					rightBumper = 0.f;
			}
			joystickMutex.unlock();
		}
		else foundJoystick = false;
	}
	else
	{
		//search for a joystick again
		joystickMutex.lock();
		//check for joysticks, stop at first joystick found plugged in
		for (joystickNumber = GLFW_JOYSTICK_1; joystickNumber <= GLFW_JOYSTICK_LAST; ++joystickNumber)
		{
			if (glfwJoystickPresent(joystickNumber))
			{
				foundJoystick = true;
				break;
			}
		}

		if (foundJoystick)
		{
			//get the state arrays for this joystick
			if (!PollJoystick()) foundJoystick = false;
		}

		joystickMutex.unlock();
	}
}