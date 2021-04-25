#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <array>

#include "Gui.h"
#include "Camera.h"
#include "world/block/Blocks.h"
#include "world/World.h"
#include "world/chunk/Chunk.h"
#include "Resources.h"

#include "renderer/Shader.h"
#include "renderer/Renderer.h"

#include "utils/Timer.h"

#include <time.h>
//Camera g_camera(glm::vec3(0, 2000, 40), 0, -90);
Camera g_camera(glm::vec3(0, 100, 40));
int g_renderDistance = 8;
World* world;

void processInput(GLFWwindow* window, float deltaTime);

unsigned int g_polygons = 0;

int main()
{
	srand(time(NULL));
	if (Renderer::Init() != 0)
		return -1;

	Gui::Init(Renderer::window);
	Resources::LoadTextures();
	Blocks::Initialize();

	world = new World();

	Shader basicShader("res/basic.vs", "res/basic.fs");
	basicShader.use();

	static Timer timer("Update", 100);

	while (!glfwWindowShouldClose(Renderer::window)) {
		float dt = Renderer::BeginRendering();

		processInput(Renderer::window, dt);

		glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom), (float)Renderer::SCR_WIDTH / (float)Renderer::SCR_HEIGHT, 0.1f, 3000.0f);
		basicShader.setMat4("projection", projection);

		glm::mat4 view = g_camera.GetViewMatrix();
		basicShader.setMat4("view", view);

		timer.begin();
		world->Update();
		world->UpdateMeshes();
		timer.finish();

		world->Render();

		std::array<int, 4> chunksLoadingStates = world->DEV_ChunksLoadingStates();
		Gui::RenderWindow(Renderer::window, g_camera.position, world->OccupiedChunkCount(), world->FreeChunkCount(), 0, g_polygons, chunksLoadingStates);

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

	static bool prevC;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !prevC)
		world->DEV_UnloadWorld();
	prevC = glfwGetKey(window, GLFW_KEY_C);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		const float reach = 10.0f;
		auto end = g_camera.position + g_camera.facingDirection * reach;

		auto [hit, hitPos, hitFace] = world->DoBlockRayTrace(g_camera.position, end);

		if (hit)
			world->SetBlock(hitPos, 0);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		const float reach = 10.0f;
		auto end = g_camera.position + g_camera.facingDirection * reach;

		auto [hit, hitPos, hitFace] = world->DoBlockRayTrace(g_camera.position, end);

		if (hit) {
			auto blockPos = hitPos.Adjacent(hitFace);
			if (!world->GetBlock(blockPos)->IsOpaque())
				world->SetBlock(hitPos.Adjacent(hitFace), Blocks::dirt->GetId());
		}
	}

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


