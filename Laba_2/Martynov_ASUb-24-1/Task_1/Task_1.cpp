#define GLEW_DLL
#define GLFW_DLL
#include <iostream>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"


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

	Shader myShader("shader.vs", "shader.fs");


	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0, 1.0, 1.0, 0);
		glClear(GL_COLOR_BUFFER_BIT);


		float time_value = glfwGetTime();
		float period_red_value = sin(time_value * 0.7f + 0.0f) * 0.5f + 0.5f;
		float period_green_value = sin(time_value * 0.8f + 2.0f) * 0.5f + 0.5f;
		float period_blue_value = sin(time_value * 0.9f + 4.0f) * 0.5f + 0.5f;

		myShader.use();
		myShader.setVec4("uni_color", period_red_value, period_green_value, period_blue_value, 1.0f);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_FAN, vertexCount, GL_UNSIGNED_INT, 0);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}