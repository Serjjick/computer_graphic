#define GLEW_DLL
#define GLFW_DLL
#include <iostream>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"



glm::vec3 camera_position = glm::vec3(0.0, 0.0, 5.0);
glm::vec3 camera_front = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 camera_up = glm::vec3(0.0, 1.0, 0.0);

void framebufer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* winodow, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;
float last_x = SCR_WIDTH / 2;
float last_y = SCR_HEIGHT / 2;

bool first_mouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

float delta_time = 0.0f;
float last_frame = 0.0f;

void process_input(GLFWwindow* window);

// Позиции подвижных деталей станка
float tableX = 0.0f;         // Сдвиг стола (Влево / Вправо)
float spindleHeadY = 0.0f;   // Сдвиг шпиндельной головы (Вверх / Вниз)
float saddleZ = 0.0f;        // Сдвиг нижнего кожуха (Вперед / Назад)

int main() {
	if (!glfwInit()) {
		fprintf(stderr, "Error: could not start GLFW3.\n");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1024, 1024, "MainWindow", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	GLenum ret = glewInit();
	if (GLEW_OK != ret) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
		return 1;
	}


	Shader myShader("ver_shader.glsl", "frag_shader.glsl");
	Model ourRTC("model.obj");

	glfwSetFramebufferSizeCallback(window, framebufer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);

	glm::vec3 Light_position = glm::vec3(0.0, 0.0, 1.0);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0, 1.0, 1.0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		process_input(window);

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
		glm::mat4 view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);
		glm::mat4 transform = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);

		float time_value = glfwGetTime();
		delta_time = time_value - last_frame;
		last_frame = time_value;

		myShader.use();

		myShader.setVec3("Viewpos", camera_position.x, camera_position.y, camera_position.z);

		// Настройка света
		myShader.setVec3("light.position", 2.0f, 4.0f, 3.5f);
		myShader.setVec3("light.ambient", 0.25f, 0.25f, 0.25f);
		myShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
		myShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// Настройка материала
		myShader.setVec3("mat.ambient", 0.12f, 0.22f, 0.15f);
		myShader.setVec3("mat.diffuse", 0.25f, 0.45f, 0.32f);
		myShader.setVec3("mat.specular", 0.35f, 0.35f, 0.35f);
		
		glUniform1f(glGetUniformLocation(myShader.ID, "mat.shininess"), 32.0f);

	
		myShader.setMat4("projection", projection);
		myShader.setMat4("view", view);
		myShader.setMat4("transform", transform);
		

		ourRTC.Draw(myShader, tableX, spindleHeadY, saddleZ);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}


void process_input(GLFWwindow* window) {
	const float camera_speed = 1.5f * delta_time;
	const float machine_speed = 2.0f * delta_time;


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  camera_position += camera_speed * camera_front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  camera_position -= camera_speed * camera_front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;


	// Движение стола (стрелка влево / стрелка вправо)
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		tableX += machine_speed;
		if (tableX > 0.26f) tableX = 0.26f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		tableX -= machine_speed;
		if (tableX < -0.26f) tableX = -0.26f;
	}

	// Движение шпиндельной головы (стрелка вверх / стрелка вниз)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		spindleHeadY += machine_speed;
		if (spindleHeadY > 0.0f) spindleHeadY = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		spindleHeadY -= machine_speed;
		if (spindleHeadY < -0.56f) spindleHeadY = -0.56f;
	}

	// Движение стола и верхнего кожуха (R / F)
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		saddleZ -= machine_speed;
		if (saddleZ < -0.25f) saddleZ = -0.25f;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		saddleZ += machine_speed;
		if (saddleZ > 0.25f) saddleZ = 0.25f;
	}
}

void mouse_callback(GLFWwindow* winodow, double xpos, double ypos) {
	float x_pos = static_cast<float>(xpos);
	float y_pos = static_cast<float>(ypos);

	if (first_mouse) {
		last_x = x_pos;
		last_y = y_pos;
		first_mouse = false;
	}

	float xoffset = x_pos - last_x;
	float yoffset = y_pos - last_y;
	last_x = x_pos;
	last_y = y_pos;

	float sensitivity = 0.01f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camera_front = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	fov -= (float)yoffset;
	if (fov < 1.0f) fov = 1.0f;
	if (fov > 45.0f) fov = 45.0f; 
}

void framebufer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}