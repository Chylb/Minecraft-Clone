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

#include "world/chunk_worker/ChunkWorker.h"

#define NUM_OF_THREADS 6

Camera g_camera(glm::vec3(0, 2000, 40), 0, -90);
//Camera g_camera(glm::vec3(0, 100, 40));
int g_renderDistance = 20;
World* world;

//std::queue<Chunk*> g_chunkLoad_job_queue;
std::queue<ChunkWrapper*> g_chunks_requiring_buffer_update_queue;
std::mutex g_job_mutex, g_chunks_requiring_buffer_update_queue_mutex;
std::condition_variable g_cv;

void processInput(GLFWwindow* window, float deltaTime);

unsigned int g_polygons = 0;

int main()
{
	std::cout << "Starting!\n";
	
	if (Renderer::Init() != 0)
		return -1;

	Gui::Init(Renderer::window);
	Resources::LoadTextures();
	Blocks::Initialize();

	std::cout << "Initializing world... " << std::flush;
	world = new World();
	std::cout << "Done.\n";
	
	Shader basicShader("res/basic.vs", "res/basic.fs");
	basicShader.use();

	/*for (int i = 0; i < NUM_OF_THREADS; i++) {
		std::thread(loadChunksLoop).detach();
	}*/

	ChunkWorker cw1;
	cw1.name = "cw1";
	ChunkWorker cw2;
	cw2.name = "cw2";

	std::thread t1([&] {cw1.Apply4NeighbourhoodJob(ChunkWorker::GenerateBlocks); });
	std::thread t2([&] {cw2.Apply4NeighbourhoodJob(ChunkWorker::GenerateMesh); });
	std::cout << "t1 id: " << t1.get_id() << "\n";
	std::cout << "t1 id: " << t2.get_id() << "\n";
	/*t1.detach();
	t2.detach();
	*/
	static Timer timer("Emptying job queue");
	bool tempQueueWasNotEmpty = false;
	if (!timer.ticking)
		timer.begin();
	
	while (!glfwWindowShouldClose(Renderer::window)) {
		float dt = Renderer::BeginRendering();

		processInput(Renderer::window, dt);

		glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom), (float)Renderer::SCR_WIDTH / (float)Renderer::SCR_HEIGHT, 0.1f, 3000.0f);
		basicShader.setMat4("projection", projection);

		glm::mat4 view = g_camera.GetViewMatrix();
		basicShader.setMat4("view", view);

		//std::cout << "Updating... " << std::flush;
		world->Update();
		//std::cout << "Done\n";

		//std::cout << "Rendering... " << std::flush;
		world->Render();
		//std::cout << "Done\n";

		std::array<int, 5> chunksLoadingStates = world->DEV_ChunksLoadingStates();
		auto lock = std::unique_lock<std::mutex>(g_chunks_requiring_buffer_update_queue_mutex);
		Gui::RenderWindow(Renderer::window, g_camera.position, world->OccupiedChunkCount(), world->FreeChunkCount(), g_chunks_requiring_buffer_update_queue.size(), g_polygons, chunksLoadingStates);
		lock.unlock();

		Renderer::EndRendering();

		//if (g_chunks_requiring_buffer_update_queue.size() && !timer.ticking)
		tempQueueWasNotEmpty |= !g_chunks_requiring_buffer_update_queue.empty();
		if (!g_chunks_requiring_buffer_update_queue.empty() && tempQueueWasNotEmpty)
			timer.finish();

		//std::cout << "Notifying all... ";
		//std::cout.flush();
		g_cv.notify_all();
		//std::cout << "Done!\n";
	}

	Gui::Terminate();

	cw1.done.exchange(true);
	cw2.done.exchange(true);
	g_cv.notify_all();
	t1.join();
	t2.join();

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
