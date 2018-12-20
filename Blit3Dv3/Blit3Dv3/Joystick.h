#pragma once
#include<mutex>
#include "Config.h"

extern std::mutex joystickMutex;

struct JoystickState
{
	int axisCount; //how many axes there are for this joystick...yes, "axes" is the plural of "axis", I know it looks funny
	const float *axisStates; //array of axisCount floats, between -1 and 1 in value
	int buttonCount; //how many buttons there are for this joystick
	const unsigned char *buttonStates; //array of buttonCount unsigned chars, will either be GLFW_PRESS or GLFW_RELEASE in value
};

class Joystick
{
public:
	bool foundJoystick = false;
	JoystickState joystickState;
	int joystickNumber;
	float leftJoystickLeftRight = 0.f; //0
	float leftJoystickUpDown = 0.f;    //1
	float rightJoystickUpDown = 0.f;//2
	float rightJoystickLeftRight = 0.f;   //3
	float rightTrigger = 0.f;           //5
	float leftTrigger = 0.f;			   //4
	float rightBumper = 0.f;
	float leftBumper = 0.f;

	void ProcessJoystickAxis(float &joystickAxis, Configuration configuration);
	bool PollJoystick();
	void DoJoystick(void);
	bool Init();
};