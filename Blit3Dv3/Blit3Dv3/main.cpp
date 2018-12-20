/*
	Simple example of 3D with Blit3D
*/
//memory leak detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include "Blit3D.h"

#include "MeshTest.h"
#include "Mesh.h"
#include "SceneManager.h"
#include "SceneNodes.h"
#include "Tank.h"
#include "Joystick.h"
#include "Config.h"
#include "Helicopter.h"
#include "Cube.h"
#include "DieRoller.h"

Blit3D *blit3D = NULL;

//GLOBAL DATA
GLSLProgram *shader = NULL;

glm::mat4 modelMatrix;
std::atomic<float> angle = 0;

Cube *cube = NULL;
Configuration configuration;
Joystick joystick;
SceneManager *sceneManager = NULL;
Tank *tank = NULL;
Helicopter *heli = NULL;


std::vector<Cube*> cubeList;

bool stripped = false;

glm::vec3 initialTankPos(0.f, 0.f, -20.f);
glm::vec3 initialHelicopterPos(0.f, 0.f, 0.f);

glm::vec4 cameraPos = glm::vec4(0.f, -20.f, -50.f, 0.f);
glm::vec4 cameraOffset = glm::vec4(0.f, 10.f, -10.f, 0.f);
float moveX = 0.f;
float moveY = 0.f;
float moveZ = 0.f;
float moveSpeed = 16.f;
float cameraAngle = 0.f;
float cameraAngleMove = 0.f;
float cameraRotateSpeed = 5.f;

void MakeBlocks()
{
	//Ground cube
	cube = new Cube(blit3D->tManager, shader, "Media\\greenTex.png", 1500, 1, 1500);
	cube->pos = glm::vec3(0.f, -3.f, 0.f);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	//Roads
	cube = new Cube(blit3D->tManager, shader, "Media\\grayTex.png", 1100, 1, 150);
	cube->pos = glm::vec3(0.f, -2.7f, 0.f);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	cube = new Cube(blit3D->tManager, shader, "Media\\grayTex.png", 150, 1, 1100);
	cube->pos = glm::vec3(0.f, -2.7f, 0.f);
	cube->SetAxis(glm::vec3(0, 1, 0));
	cubeList.push_back(cube);

	//Buildings creation
	std::vector<std::string> texVec;
	texVec.push_back("Media\\greenTex.png");
	texVec.push_back("Media\\blueTex.png");
	texVec.push_back("Media\\orangeTex.png");
	texVec.push_back("Media\\redTex.png");

	int x, z, heigth;

	for (x = -5; x < 5; x++)
	{
		for (z = -5; z < 5; z++)
		{
			if (x != 0 && z != 0)
			{
				heigth = RollDNInt(80, 300);
				cube = new Cube(blit3D->tManager, shader, texVec[RollDNInt(0, 3)], 30, heigth, 30);
				cube->pos = glm::vec3(x * 100, heigth / 2.5f, z * 100);
				cube->SetAxis(glm::vec3(0, 1, 0));
				cubeList.push_back(cube);
			}
		}
	}
}

void Init()
{
	Seed();
	blit3D->SetMode(Blit3DRenderMode::BLIT3D);

	sceneManager = new SceneManager();

	shader = new GLSLProgram(); // instantiate the new shader
	shader->compileShaderFromFile("vertex.glsl", GLSLShader::GLSLShaderType::VERTEX);
	shader->compileShaderFromFile("fragment.glsl", GLSLShader::GLSLShaderType::FRAGMENT);
	//bind attributes for the shader layout
	shader->bindAttribLocation(0, "position");
	shader->bindAttribLocation(1, "normal");
	shader->bindAttribLocation(2, "texUV");
	shader->link();
	// link the new shader
	shader->use();
	// tell blit to use the new shader
	blit3D->SetMode(Blit3DRenderMode::BLIT3D, shader);
	// set blit mode to 3D using new shader
	//blit3D->viewMatrix = glm::mat4();

	//3d perspective projection
	blit3D->projectionMatrix = glm::perspective(45.0f, (GLfloat)(blit3D->screenWidth) / (GLfloat)(blit3D->screenHeight), 0.1f, 10000.0f);
	shader->setUniform("projectionMatrix", blit3D->projectionMatrix);
	shader->setUniform("viewMatrix", blit3D->viewMatrix);

	glm::vec3 lightPos = glm::vec3(-200.f, 1000.0f, 1000.0f);
	// light position
	shader->setUniform("lightPos", lightPos);
	// send lighting info to the shader

	glEnable(GL_BLEND);// enable blending	
	glEnable(GL_DEPTH_TEST);// enables proper 3D depth drawing	
	glEnable(GL_CULL_FACE);// backs of triangles are no longer drawn	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);// more blending

	//Helicopter object
	heli = new Helicopter(sceneManager->sceneGraph, initialHelicopterPos, shader, 10.f, 10.f, 5.f);
	
	//Build "so called city"
	MakeBlocks();

	//Load config file for joystick values
	if (!LoadConfig(configuration))
	{
		std::cout << "ERROR reading the config file!\n\n";
	}
	else
		DisplayConfig(configuration);

	//joystick stuff
	joystick.Init();
}

void DeInit(void)
{
	//Delete stuff from memory
	if (sceneManager != NULL) delete sceneManager;
	if (shader != NULL) delete shader;
	if (tank != NULL) delete tank;
	if (heli != NULL) delete heli;
	for (auto &C : cubeList) delete C;
}

void Update(double seconds)
{
	//cameraPos.z += moveZ * moveSpeed *seconds;
	if (seconds > 0.15) seconds = 0.15;


	//Update elements from the Cube vector
	for (int i = 0; i < cubeList.size(); i++)
		cubeList[i]->Update((float)seconds);

	joystick.DoJoystick(); //handle joystick polling
						   //process joystick values
	joystick.ProcessJoystickAxis(joystick.leftJoystickLeftRight, configuration);
	joystick.ProcessJoystickAxis(joystick.leftJoystickUpDown, configuration);
	joystick.ProcessJoystickAxis(joystick.rightJoystickLeftRight, configuration);
	joystick.ProcessJoystickAxis(joystick.rightJoystickUpDown, configuration);
	joystick.ProcessJoystickAxis(joystick.leftTrigger, configuration);
	joystick.ProcessJoystickAxis(joystick.rightTrigger, configuration);
	
	//Update the tank with joystick input
	//tank->Update((float)seconds, joystick.rightBumper, joystick.leftBumper, joystick.leftJoystickLeftRight, joystick.rightJoystickLeftRight, joystick.rightJoystickUpDown);
	
	//Update helicopter
	heli->Update((float)seconds, joystick.rightTrigger, joystick.leftTrigger, joystick.leftJoystickLeftRight,
		joystick.rightJoystickUpDown, joystick.rightJoystickLeftRight, joystick.leftJoystickUpDown, joystick.rightBumper, joystick.leftBumper);
	
	//Update every nodes in the sceneManger
	sceneManager->UpdateWorld((float)seconds);

	//Camera offset to follow helicopter
	blit3D->viewMatrix = heli->cameraNode->viewMatrix;

}

void Draw(void)
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);	//clear colour: r,g,b,a 	
											// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw elements from the Cube vector
	for (int i = 0; i < cubeList.size(); i++) 
		cubeList[i]->Draw();

	//move camera with key input
	//blit3D->viewMatrix = glm::translate(glm::mat4(), (glm::vec3)cameraPos);
	//blit3D->viewMatrix = glm::rotate(blit3D->viewMatrix, cameraAngle, glm::vec3(0.f, 1.f, 0.f));
	shader->setUniform("viewMatrix", blit3D->viewMatrix);

	sceneManager->DrawWorld();
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		stripped = false;
		//m->stripped = stripped;
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		stripped = true;
		//m->stripped = stripped;
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		moveZ = 1.f;
	}
	else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		moveZ = 0.f;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		moveZ = -1.f;
	}
	else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
	{
		moveZ = 0.f;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		moveX = -1.f;
	}
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		moveX = 0.f;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		moveX = 1.f;
	}
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		moveX = 0.f;
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		moveY = -1.f;
	}
	else if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
	{
		moveY = 0.f;
	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		moveY = 1.f;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
	{
		moveY = 0.f;
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		cameraAngleMove = 1.f;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
	{
		cameraAngleMove = 0.f;
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		cameraAngleMove = -1.f;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
	{
		cameraAngleMove = 0.f;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		heli->ResetPosition(initialHelicopterPos);//Reset the tank to its original position 
	}
}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//set X to the memory allocation number in order to force a break on the allocation:
	//useful for debugging memory leaks, as long as your memory allocations are deterministic.
	//_crtBreakAlloc = X;

	blit3D = new Blit3D(Blit3DWindowModel::DECORATEDWINDOW, 1024, 1024);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}