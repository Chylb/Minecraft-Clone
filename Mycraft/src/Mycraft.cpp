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

#include "Gui.h"
#include "Camera.h"
#include "world/block/Blocks.h"
#include "world/World.h"
#include "world/chunk/Chunk.h"
#include "Resources.h"

#include "renderer/Shader.h"
#include "renderer/Renderer.h"

#define NUM_OF_THREADS 3

Camera g_camera(glm::vec3(0, 100, 40));
int g_renderDistance = 8;
World* world;

std::queue<Chunk*> g_chunkLoad_job_queue;
std::queue<Chunk*> g_chunkLoad_completed_job_queue;
std::mutex g_job_mutex, g_completed_job_mutex;
std::condition_variable g_cv;

void processInput(GLFWwindow* window, float deltaTime);
void loadChunksLoop();

unsigned int g_polygons = 0;

int main()
{
	if (Renderer::Init() != 0)
		return -1;

	Gui::Init(Renderer::window);
	Resources::LoadTextures();
	Blocks::Initialize();

	world = new World();

	Shader basicShader("res/basic.vs", "res/basic.fs");
	basicShader.use();

	for (int i = 0; i < NUM_OF_THREADS; i++) {
		std::thread(loadChunksLoop).detach();
	}

	while (!glfwWindowShouldClose(Renderer::window)) {
		float dt = Renderer::BeginRendering();

		processInput(Renderer::window, dt);

		glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom), (float)Renderer::SCR_WIDTH / (float)Renderer::SCR_HEIGHT, 0.1f, 2000.0f);
		basicShader.setMat4("projection", projection);

		glm::mat4 view = g_camera.GetViewMatrix();
		basicShader.setMat4("view", view);

		world->Update();
		world->Render();

		Gui::RenderWindow(Renderer::window, g_camera.position, world->OccupiedChunkCount(), world->FreeChunkCount(), g_chunkLoad_job_queue.size(), g_polygons);

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

void loadChunksLoop()
{
	while (true) {
		std::unique_lock<std::mutex> g1(g_job_mutex);
		g_cv.wait(g1, [] {return !g_chunkLoad_job_queue.empty(); });

		Chunk* chunk = g_chunkLoad_job_queue.front();
		g_chunkLoad_job_queue.pop();
		g1.unlock();

		switch (chunk->loadingState) {
		case Chunk::LoadingState::loading_blocks:
		{
			world->PopulateChunk(*chunk);
		}
		break;

		case Chunk::LoadingState::generating_mesh:
		{
			chunk->GenerateMesh();
		}
		break;
		}

		std::lock_guard<std::mutex> g2(g_completed_job_mutex);
		g_chunkLoad_completed_job_queue.push(chunk);
	}
}
