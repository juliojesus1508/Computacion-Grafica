#include <glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Rubikh.h"
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

#include <iostream>
// #include <Windows.h>

RubikCube *myRubik;

/*Global Camera Values*/
glm::vec3 cameraPos = glm::vec3(0.f, 0.f, 3.f);
glm::vec3 cameraFront = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);

/*Global print Values*/
int mode = 0;
bool unit = false;
bool giro = false;
short axis = 0;
short layer = 0;
bool isclock = 0;
bool iscounterclock = 0;

/*Global solver Values*/
std::string commands;
std::vector<std::string> movements;
int movesPos = 0;
bool solving = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void random_sequence(int number, RubikCube* cube){
	for(int i = 0; i < number; i++){
		int r = rand()%6;
		switch (r) {
      	case 0:
      		cube->F(1, false);
      		commands += "F ";
       		break;
       	case 1:
      		cube->R(1, false);
      		commands += "R ";
       		break;
       	case 2:
      		cube->U(1, false);
      		commands += "U ";
       		break;
       	case 3:
      		cube->B(1, false);
      		commands += "B ";
       		break;
       	case 4:
      		cube->L(1, false);
      		commands += "L ";
       		break;
       	case 5:
      		cube->D(1, false);
      		commands += "D ";
       		break;
      	
    	}

	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	const float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		mode = 0;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		mode = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		mode = 2;
	}

	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		mode = 3;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		unit = !unit;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		giro = !giro;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		axis = (axis+1)%3;
		switch (axis)
		{
		case 0:
			std::cout << "Eje X activo" << std::endl;
			break;
		case 1:
			std::cout << "Eje Y activo" << std::endl;
			break;
		case 2:
			std::cout << "Eje Z activo" << std::endl;
			break;
		default:
			break;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		layer = (layer == 2) ? 2 : layer + 1;
		std::cout << "Capa " << layer << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		layer = (layer == 0) ? 0 : layer - 1;
		std::cout << "Capa " << layer << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		isclock = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		iscounterclock = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		random_sequence(20, myRubik);
		// apply_sequence(myRubik, commands);
		// myRubik->F(1, false);
		// myRubik->R(1, false);
		// myRubik->L(1, false);
		// myRubik->D(1, false);
		// myRubik->B(1, false);
		// myRubik->U(1, false);
		// myRubik->D(1, false);
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		myRubik->colors();
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		movements = myRubik->solve(commands);
		commands = "";
		solving = true;
	}
}

void stepSolver(RubikCube * cube, int pos){
	switch (movements[pos][0]) {
      	case 'F':
      		if (movements[pos].size() == 2){
      			if(movements[pos][1] == '\'')
      				cube->F(1, true);
      			if(movements[pos][1] == '2')
      				cube->F(2, false);
      		}
      		else{
      				cube->F(1, false);
      		}
       		break;
      	case 'R':
        	if (movements[pos].size() == 2){
      			if(movements[pos][1] == '\'')
      				cube->R(1, true);
      			if(movements[pos][1] == '2')
      				cube->R(2, false);
      		}
      		else{
      			cube->R(1, false);
      		}
        	break;
      	case 'U':
        	if (movements[pos].size() == 2){
      			if(movements[pos][1] == '\'')
      				cube->U(1, true);
      			if(movements[pos][1] == '2')
      				cube->U(2, false);
      		}
      		else{
      			cube->U(1, false);
      		}
        	break;
      	case 'B':
        	if (movements[pos].size() == 2){
      			if(movements[pos][1] == '\'')
      				cube->B(1, true);
      			if(movements[pos][1] == '2')
      				cube->B(2, false);
      		}
      		else{
      			cube->B(1, false);
      		}
        	break;
      	case 'L':
        	if (movements[pos].size() == 2){
      			if(movements[pos][1] == '\'')
      				cube->L(1, true);
      			if(movements[pos][1] == '2')
      				cube->L(2, false);
      		}
      		else{
      			cube->L(1, false);
      		}
        	break;
      	case 'D':
        	if (movements[pos].size() == 2){
      			if(movements[pos][1] == '\'')
      				cube->D(1, true);
      			if(movements[pos][1] == '2')
      				cube->D(2, false);
      		}
      		else{
      			cube->D(1, false);
      		}
        	break;
    		}
}

void whatDraw(RubikCube* cube, int shaderID) {
	switch (mode) {
	case 0:
		cube->drawRubik(shaderID, unit);
		break;
	case 1:
		cube->drawCenters(shaderID);
		break;
	case 2:
		cube->drawCorners(shaderID);
		break;
	case 3:
		cube->drawEdges(shaderID);
		break;
	}
}

void rotate(RubikCube* cube) {
	switch (axis)
	{
	case 0:
		if (iscounterclock){
			cube->rotateXaxis(layer, COUNTERCLOCK);
			iscounterclock = 0;
		}
		else if (isclock) {
			cube->rotateXaxis(layer, CLOCK);
			isclock = 0;
		}
		
		break;
	case 1:
		if (iscounterclock) {
			cube->rotateYaxis(layer, COUNTERCLOCK);
			iscounterclock = 0;
		}
		else if (isclock) {
			cube->rotateYaxis(layer, CLOCK);
			isclock = 0;
		}
		break;
	case 2:
		if (iscounterclock) {
			cube->rotateZaxis(layer, COUNTERCLOCK);
			iscounterclock = 0;
		}
		else if (isclock) {
			cube->rotateZaxis(layer, CLOCK);
			isclock = 0;
		}
		break;
	default:
		break;
	}
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	

	GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/*Create Shaders*/
	Shader myShader("/home/mauro/c++/grafica/proyecto/GLFW_GLAD_GLUT_GLEW_cmake_project/src/rubik3/shaders/vertexShader.vs",
					"/home/mauro/c++/grafica/proyecto/GLFW_GLAD_GLUT_GLEW_cmake_project/src/rubik3/shaders/fragmentShader.fs");

	/*Configure mode to wireframe*/
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	
	/**/


	/*Render Loop*/
	


	/*Probar Rubik*/
	// RubikCube *myRubik;
	myRubik = new RubikCube;
	
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.f), 800.f / 600.f, 0.1f, 100.f);

	unsigned int modelLoc = glGetUniformLocation(myShader.ID, "model");
	

	unsigned int viewLoc = glGetUniformLocation(myShader.ID, "view");
	

	unsigned int projLoc = glGetUniformLocation(myShader.ID, "projection");


	/*Matriz view*/
	glm::mat4 view = glm::mat4(1.0f);;
	
	const float radius = 10.f;
	float camX, camZ;
	

	

	float rotacion = (float)glfwGetTime() * glm::radians(15.f);

	glEnable(GL_DEPTH_TEST);

	bool a = 1;

	myRubik->setShader(myShader.ID);
	myRubik->setWindow(window);

	while (!glfwWindowShouldClose(window)) {
		glfwSetKeyCallback(window, key_callback);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*Rotación y traslación de una matriz*/
		/*glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.f));
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.f));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));*/

		myRubik->setUnite(unit);

		myShader.use();		

		whatDraw(myRubik, myShader.ID);
		rotate(myRubik);

		if (solving){
			stepSolver(myRubik, movesPos);
			movesPos += 1;
			if (movesPos >= movements.size()){
				movesPos = 0;
				movements.clear();
				solving = 0;
			}
		}


		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		rotacion = giro ? (float)glfwGetTime() * glm::radians(15.f) : rotacion;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, rotacion, glm::vec3(1.f, 2.0f, 0.f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		
		glBindVertexArray(0);

		// Sleep(50);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	

	return 0;
}