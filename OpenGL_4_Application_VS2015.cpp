//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "SkyBox.hpp"
#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"



int glWindowWidth = 1366;
int glWindowHeight = 650;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;
gps::SkyBox mySkyBox;//*
gps::Shader skyboxShader;//*
glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
glm::mat4 view2;
GLuint viewLoc;

glm::mat4 projection;
glm::mat4 projection2;
GLuint projectionLoc;
glm::mat4 lightTransMat;
GLuint lightTransMatLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
double lastTime;
glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
glm::vec3 lightDir2;
GLuint lightDir2Loc;
glm::vec3 lightColor2;
GLuint lightColor2Loc;
GLfloat fog;
GLuint fogLoc;//location of the fog uniform
bool flag;
float deltaTime;
float verticalAngle= 0.0f;
float horizontalAngle= 3.14f;
float mouseSpeed = 25.0f;
float initialFoV = 90.0f;
float speed = 3.0f; // 
float count;

glm::vec3 position = glm::vec3(0, 0, 5);
gps::Camera myCamera(glm::vec3(200.0f, 50.0f, 200.5f), glm::vec3(0.0f, 5.0f, -10.0f));
gps::Camera myCameraDepth(glm::vec3(200.0f, 50.0f, 200.5f), glm::vec3(0.0f, 5.0f, -10.0f));
float cameraSpeed = 0.5f;

bool pressedKeys[1024];
float angle = 0.0f;

gps::Model3D myModel;
gps::Model3D ground;
gps::Model3D horse;
gps::Model3D hand;
gps::Model3D hand2;
gps::Model3D head;
gps::Model3D foot;
gps::Model3D handsOnly;

gps::Shader myCustomShader;

GLuint shadowMapFBO, depthMapTexture;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window)
{
	double xpos, ypos;
	glfwGetCursorPos(window,&xpos, &ypos);
	std::cout << xpos << " " << ypos << std::endl;
	double currentTime = glfwGetTime();
	deltaTime = float(currentTime - lastTime);
	lastTime = currentTime;
	horizontalAngle += mouseSpeed * deltaTime * float(xpos);
    verticalAngle += mouseSpeed * deltaTime * float(ypos);
	if (verticalAngle > 85.0f) verticalAngle = 85.0f;
	if (verticalAngle < -85.0f) verticalAngle = -85.0f;
	myCamera.rotate(-verticalAngle, -horizontalAngle);
	glfwSetCursorPos(window, 0, 0);
}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 0.1f;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 0.1f;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_J]) {
		flag = true;
	}
	if (pressedKeys[GLFW_KEY_H]) {
		flag = false;
	}
	if (pressedKeys[GLFW_KEY_SPACE]) {
		count++;
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_F]) {
		fog += 0.0001f;
		if (fog > 0.05f) fog = 0.05f;
	}
	if (pressedKeys[GLFW_KEY_G]) {
		fog -= 0.0001f;
		if (fog < 0.0005f) fog = 0.0005f;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	//glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);
	
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}

void initModels()
{
	//myModel = gps::Model3D("objects/nanosuit/nanosuit.obj", "objects/nanosuit/");
	ground = gps::Model3D("objects/cobblestone/untitled2.obj","objects/cobblestone/");
	hand = gps::Model3D("objects/hand1.obj","objects/");
	hand2 = gps::Model3D("objects/hand2/holding.obj","objects/hand2/");
	head = gps::Model3D("objects/head/head.obj","objects/head/");
	foot = gps::Model3D("objects/foot/foot.obj","objects/foot/");
	handsOnly = gps::Model3D("objects/handsOnly.obj","objects/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
}

void initUniforms()
{
	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(70.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f,1.0f,0.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	
	//set the light direction (direction towards the light)
	lightDir2 = glm::vec3(0.0f, 0.0f, 1.0f);
	lightDir2Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir2");
	glUniform3fv(lightDir2Loc, 1, glm::value_ptr(lightDir2));

	//set light color
	lightColor2 = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColor2Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor2");
	glUniform3fv(lightColor2Loc, 1, glm::value_ptr(lightColor2));

	//make fog uniform
	fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
	glUniform1f(fogLoc, fog);

	view2 = glm::lookAt(glm::vec3(200.0f, 50.0f, 200.5f),myCameraDepth.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));
	projection2 = glm::ortho(-2000.0f, 2000.0f, -2000.0f, 2000.0f);
	lightTransMatLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix");
	lightTransMat = projection2 * view2;
	glUniformMatrix4fv(lightTransMatLoc, 1, GL_FALSE, glm::value_ptr(lightTransMat));
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	processMovement();

	myCustomShader.useShaderProgram();

	//initialize the view matrix
	view = myCamera.getViewMatrix();
	//send view matrix data to shader	
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create model matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	glUniform1f(fogLoc, fog);

	skyboxShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));

	skyboxShader.useShaderProgram();
	mySkyBox.Draw(skyboxShader, view, projection);

	myCustomShader.useShaderProgram();
	ground.Draw(myCustomShader);
	hand.Draw(myCustomShader);
	model = glm::translate(model,glm::vec3(0.0f, count, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	handsOnly.Draw(myCustomShader);
	
	count -= 0.5f;
	if (count < 0) count = 0;
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initModels();
	initShaders();
	initUniforms();	

	std::vector<const GLchar*> faces;
	faces.push_back("textures/scenaFaina/criminal-impact_rt.tga");
	faces.push_back("textures/scenaFaina/criminal-impact_lf.tga");
	faces.push_back("textures/scenaFaina/criminal-impact_up.tga");
	faces.push_back("textures/scenaFaina/criminal-impact_dn.tga");
	faces.push_back("textures/scenaFaina/criminal-impact_bk.tga");
	faces.push_back("textures/scenaFaina/criminal-impact_ft.tga");
	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");


	glGenFramebuffers(1, &shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	4096, 4096, 0, GL_DEPTH_COMPONENT, GL_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	while (!glfwWindowShouldClose(glWindow)) {

		glViewport(0, 0, 4096, 4096);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		renderScene();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, retina_width, retina_height);
		renderScene();
		mouseCallback(glWindow);
		glfwPollEvents();
		glfwSwapBuffers(glWindow);

	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
