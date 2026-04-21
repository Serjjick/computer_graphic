#define GLEW_DLL
#define GLFW_DLL
#include <iostream>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


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


	const int segments = 100;
	const int vertexCount = segments + 2;
	float ellipseVertices[vertexCount * 3];
	unsigned int indices[vertexCount];

	float radiusX = 0.5f;
	float radiusY = 0.3f;
	ellipseVertices[0] = 0.0f;
	ellipseVertices[1] = 0.0f;
	ellipseVertices[2] = 0.0f;
	indices[0] = 0;

	for (int i = 0; i <= segments; i++) {
		float theta = 2.0f * 3.1415926f * (float)i / (float)segments;
		int idx = (i + 1);
		ellipseVertices[idx * 3 + 0] = radiusX * cosf(theta);
		ellipseVertices[idx * 3 + 1] = radiusY * sinf(theta);
		ellipseVertices[idx * 3 + 2] = 0.0f;
		indices[idx] = idx;
	}


	GLuint VAO, VBO;

	glCreateBuffers(1, &VBO);
	glCreateVertexArrays(1, &VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(ellipseVertices), ellipseVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader myShader("ver_shader.glsl", "frag_shader.glsl");

	glfwSetFramebufferSizeCallback(window, framebufer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0, 1.0, 1.0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		process_input(window);

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
		glm::mat4 view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);
		glm::mat4 transform = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);

		float time_value = glfwGetTime();
		float period_red_value = sin(time_value * 0.7f + 0.0f) * 0.5f + 0.5f;
		float period_green_value = sin(time_value * 0.8f + 2.0f) * 0.5f + 0.5f;
		float period_blue_value = sin(time_value * 0.9f + 4.0f) * 0.5f + 0.5f;

		delta_time = time_value - last_frame;
		last_frame = time_value;

		myShader.use();
	
		myShader.setMat4("projection", projection);
		myShader.setMat4("view", view);
		myShader.setMat4("transform", transform);
		myShader.setMat4("model", model);
	
		myShader.setVec4("uni_color", period_red_value, period_green_value, period_blue_value, 1.0f);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_FAN, vertexCount, GL_UNSIGNED_INT, 0);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}


void process_input(GLFWwindow* window) {
	const float camera_speed = 1.5f * delta_time;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera_position += camera_speed * camera_front;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera_position -= camera_speed * camera_front;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
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