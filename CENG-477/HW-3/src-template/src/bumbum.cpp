#define _USE_MATH_DEFINES

#include "OpenGL.h"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <vector>
#include <array>
#include <cmath>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

//#define DEBUG_LIGHT

#pragma region GLOBALS

GLFWwindow* window = NULL;

GLFWmonitor* monitor;
const GLFWvidmode* mode;

constexpr float R = 175.f;
constexpr int sectorCount = 250;
constexpr int stackCount = 125;

int displayWidth;
int displayHeight;

int texWidth;
int texHeight;

Camera camera(glm::vec3(0, -1, 0), glm::vec3(0, 0, 1), 0, 0);

glm::vec3 lightPos = { 0, 1600, 0 };

GLfloat heightFactor = 0.0f;
GLint textureOffset = 0;

std::vector<float> positions;
std::vector<GLuint> indices;

#pragma endregion

#ifdef DEBUG_LIGHT
float verticesReal[] = {
		-5.f, -5.f, -5.f,
		 5.f, -5.f, -5.f,
		 5.f,  5.f, -5.f,
		 5.f,  5.f, -5.f,
		-5.f,  5.f, -5.f,
		-5.f, -5.f, -5.f,

		-5.f, -5.f,  5.f,
		 5.f, -5.f,  5.f,
		 5.f,  5.f,  5.f,
		 5.f,  5.f,  5.f,
		-5.f,  5.f,  5.f,
		-5.f, -5.f,  5.f,

		-5.f,  5.f,  5.f,
		-5.f,  5.f, -5.f,
		-5.f, -5.f, -5.f,
		-5.f, -5.f, -5.f,
		-5.f, -5.f,  5.f,
		-5.f,  5.f,  5.f,

		 5.f,  5.f,  5.f,
		 5.f,  5.f, -5.f,
		 5.f, -5.f, -5.f,
		 5.f, -5.f, -5.f,
		 5.f, -5.f,  5.f,
		 5.f,  5.f,  5.f,

		-5.f, -5.f, -5.f,
		 5.f, -5.f, -5.f,
		 5.f, -5.f,  5.f,
		 5.f, -5.f,  5.f,
		-5.f, -5.f,  5.f,
		-5.f, -5.f, -5.f,

		-5.f,  5.f, -5.f,
		 5.f,  5.f, -5.f,
		 5.f,  5.f,  5.f,
		 5.f,  5.f,  5.f,
		-5.f,  5.f,  5.f,
		-5.f,  5.f, -5.f
};
float vertices[] = {
		-5.f, -5.f, -5.f,
		 5.f, -5.f, -5.f,
		 5.f,  5.f, -5.f,
		 5.f,  5.f, -5.f,
		-5.f,  5.f, -5.f,
		-5.f, -5.f, -5.f,

		-5.f, -5.f,  5.f,
		 5.f, -5.f,  5.f,
		 5.f,  5.f,  5.f,
		 5.f,  5.f,  5.f,
		-5.f,  5.f,  5.f,
		-5.f, -5.f,  5.f,

		-5.f,  5.f,  5.f,
		-5.f,  5.f, -5.f,
		-5.f, -5.f, -5.f,
		-5.f, -5.f, -5.f,
		-5.f, -5.f,  5.f,
		-5.f,  5.f,  5.f,

		 5.f,  5.f,  5.f,
		 5.f,  5.f, -5.f,
		 5.f, -5.f, -5.f,
		 5.f, -5.f, -5.f,
		 5.f, -5.f,  5.f,
		 5.f,  5.f,  5.f,

		-5.f, -5.f, -5.f,
		 5.f, -5.f, -5.f,
		 5.f, -5.f,  5.f,
		 5.f, -5.f,  5.f,
		-5.f, -5.f,  5.f,
		-5.f, -5.f, -5.f,

		-5.f,  5.f, -5.f,
		 5.f,  5.f, -5.f,
		 5.f,  5.f,  5.f,
		 5.f,  5.f,  5.f,
		-5.f,  5.f,  5.f,
		-5.f,  5.f, -5.f
};

void CreateLightBox()
{
	for (int i = 0; i < 108; i += 3)
	{
		vertices[i] = verticesReal[i] + lightPos.x;
		vertices[i + 1] = verticesReal[i + 1] + lightPos.y;
		vertices[i + 2] = verticesReal[i + 2] + lightPos.z;
	}
}
#endif // DEBUG_LIGHT

void CreateVertices()
{
	positions.reserve((sectorCount + 1) * (stackCount + 1) * 5);
	float x, y, z;

	float sectorStep = 2 * M_PI / sectorCount;
	float stackStep = M_PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; i++)
	{
		stackAngle = M_PI / (float)2 - i * stackStep;
		z = R * sin(stackAngle);

		for (int j = 0; j <= sectorCount; j++)
		{
			sectorAngle = j * sectorStep;

			x = R * cos(stackAngle) * cos(sectorAngle);
			y = R * cos(stackAngle) * sin(sectorAngle);

			positions.push_back(x);
			positions.push_back(y);
			positions.push_back(z);

			positions.push_back((float)j);
			positions.push_back((float)i);
		}
	}
}

void CreateIndices()
{
	indices.reserve(6 * sectorCount * stackCount);
	GLuint k1, k2;

	for (int i = 0; i < stackCount; i++)
	{
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (int j = 0; j < sectorCount; j++, k1++, k2++)
		{
			//if (i != 0)
			//{
				indices.push_back(k1    );
				indices.push_back(k2    );
				indices.push_back(k1 + 1);
			//}

			//if(i != verSplit - 1)
			//{
				indices.push_back(k1 + 1);
				indices.push_back(k2    );
				indices.push_back(k2 + 1);
			//}
		}
	}
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Fullscreen.
	if (action == GLFW_REPEAT || action == GLFW_PRESS)
	{
		switch (key)
		{
			case GLFW_KEY_ESCAPE:	glfwSetWindowShouldClose(window, GLFW_TRUE);	break;
			case GLFW_KEY_R:		heightFactor += 0.5f;							break;
			case GLFW_KEY_F:		heightFactor -= 0.5f;							break;
			case GLFW_KEY_Q:		textureOffset -= 1;								break;
			case GLFW_KEY_E:		textureOffset += 1;								break;
			case GLFW_KEY_UP:		lightPos.z += 5;
#ifdef DEBUG_LIGHT
				CreateLightBox();
#endif // DEBUG_LIGHT
																					break;
			case GLFW_KEY_DOWN:		lightPos.z -= 5;
#ifdef DEBUG_LIGHT
				CreateLightBox();
#endif // DEBUG_LIGHT
																					break;
			case GLFW_KEY_RIGHT:	lightPos.x -= 5;
#ifdef DEBUG_LIGHT
				CreateLightBox();
#endif // DEBUG_LIGHT
																					break;
			case GLFW_KEY_LEFT:		lightPos.x += 5;
#ifdef DEBUG_LIGHT
				CreateLightBox();
#endif // DEBUG_LIGHT
																					break;
			case GLFW_KEY_T:		lightPos.y += 5;
#ifdef DEBUG_LIGHT
				CreateLightBox();
#endif // DEBUG_LIGHT
																					break;
			case GLFW_KEY_G:		lightPos.y -= 5;
#ifdef DEBUG_LIGHT
				CreateLightBox();
#endif // DEBUG_LIGHT
																					break;
			case GLFW_KEY_Y:		camera.speed += 0.25f;							break;
			case GLFW_KEY_H:		camera.speed -= 0.25f;							break;
			case GLFW_KEY_X:		camera.speed = 0;								break;
			case GLFW_KEY_W:		camera.pitch += 0.5; camera.Update();			break;
			case GLFW_KEY_S:		camera.pitch -= 0.5; camera.Update();			break;
			case GLFW_KEY_A:		camera.yaw -= 0.5;   camera.Update();			break;
			case GLFW_KEY_D:		camera.yaw += 0.5;   camera.Update();			break;
			case GLFW_KEY_I:		camera.Reset();									break;
			case GLFW_KEY_P:		glfwSetWindowMonitor(window, monitor, 0, 0,
										mode->width, mode->height, mode->refreshRate);
																					break;
			default:																break;
		}
	}
}

int main(int argc, char*args[])
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		return -1;

	
	window = glfwCreateWindow(1000, 1000, "477-3 SPHERE", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);

	if (glewInit() != GLEW_OK)
		std::cout << "glewInit failed!" << std::endl;

	/* Print the version */
	std::cout << glGetString(GL_VERSION) << std::endl;

	Texture textureNormal(args[2]);
	Texture textureHeight(args[1]);
	textureNormal.Bind(0);
	textureHeight.Bind(1);

	texWidth = textureNormal.GetWidth();
	texHeight = textureNormal.GetHeight();

	GLuint vertexCount = (sectorCount + 1) * (stackCount + 1);
	GLuint indexCount = 6 * (sectorCount) * stackCount;

	camera.SetPos(glm::vec3(0, 600, 0));

	CreateVertices();
	CreateIndices();

#ifdef DEBUG_LIGHT
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
#endif // DEBUG_LIGHT

	VertexBuffer vb(&positions[0], vertexCount * 5 * sizeof(GLfloat));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (const void *)(3*sizeof(GLfloat)));

	IndexBuffer ib(&indices[0], indexCount);

	Shader shader("res/sphere.shader");
	shader.Bind();

	shader.SetUniform1i("texWidth", texWidth);
	shader.SetUniform1i("texHeight", texHeight);
	shader.SetUniform1i("colorMap", 0);
	shader.SetUniform1i("heightMap", 1);

#ifdef DEBUG_LIGHT
	GLuint vaoLight;
	glGenVertexArrays(1, &vaoLight);
	glBindVertexArray(vaoLight);
	CreateLightBox();
	Shader shaderLight("res/onlyMVP.shader");
#endif // DEBUG_LIGHT

	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0, 0, 0, 1);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef DEBUG_LIGHT
		glBindVertexArray(vao);
		shader.Bind();
#endif // DEBUG_LIGHT

		/* UPDATE */
		glfwGetWindowSize(window, &displayWidth, &displayHeight);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		camera.pos += camera.speed * camera.gaze;
		glm::mat4 proj = glm::perspective(45.0f, (GLfloat)width / height, 0.1f, 1000.0f);
		glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.gaze, camera.up);

		shader.SetUniformMat4("MVP", proj * view);
		shader.SetUniformVec3("camPos", camera.pos);
		shader.SetUniformVec3("lightPos", lightPos);
		shader.SetUniform1f("heightFactor", heightFactor);
		shader.SetUniform1i("textureOffset", textureOffset);

		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

		/* UPDATE */

#ifdef DEBUG_LIGHT
		glBindVertexArray(vaoLight);
		VertexBuffer vbLight(vertices, 36 * 3 * sizeof(GLfloat));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		shaderLight.Bind();
		shaderLight.SetUniformMat4("MVP", proj * view);
		glDrawArrays(GL_TRIANGLES, 0, 36);
#endif // DEBUG_LIGHT

		glfwSwapBuffers(window);
		glfwPollEvents();
		glFlush();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}