#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Gui.h"
#include "Camera.h"
#include "world/block/Blocks.h"
#include "world/World.h"
#include "Resources.h"

#include "renderer/Shader.h"
#include "renderer/Renderer.h"

void processInput(GLFWwindow* window, float deltaTime);

Camera g_camera(glm::vec3(0, 100, 40));

int main()
{
	if (Renderer::Init() != 0)
		return -1;

	Gui::Init(Renderer::window);
	Resources::LoadTextures();
	Blocks::Initialize();

	World* world = new World();

	Shader basicShader("res/basic.vs", "res/basic.fs");
	basicShader.use();

	while (!glfwWindowShouldClose(Renderer::window)) {
		float dt = Renderer::BeginRendering();

		processInput(Renderer::window, dt);

		glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom), (float)Renderer::SCR_WIDTH / (float)Renderer::SCR_HEIGHT, 0.1f, 2000.0f);
		basicShader.setMat4("projection", projection);

		glm::mat4 view = g_camera.GetViewMatrix();
		basicShader.setMat4("view", view);

		world->Render();

		Gui::RenderWindow(Renderer::window, g_camera.position);

		Renderer::EndRendering();
	}

	Gui::Terminate();

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		Gui::cursorCaptured = false;
	}
	g_camera.sprint = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		g_camera.ProcessMovement(CameraMovement::forward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		g_camera.ProcessMovement(CameraMovement::backward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		g_camera.ProcessMovement(CameraMovement::left, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		g_camera.ProcessMovement(CameraMovement::right, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		g_camera.ProcessMovement(CameraMovement::up, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		g_camera.ProcessMovement(CameraMovement::down, deltaTime);
}
