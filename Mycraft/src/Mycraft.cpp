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
#include "state/properties/BlockStateProperties.h"
#include "state/StateContainer.h"
#include "state/StateHolder.h"
#include "model/BlockModels.h"
#include "model/BlockModelRegistry.h"
#include "item/BlockItemUseContext.h"

#include "renderer/Shader.h"
#include "renderer/Renderer.h"
#include "renderer/color/BlockColors.h"

#include "utils/Timer.h"

#include <time.h>

//Camera g_camera(glm::vec3(0, 2000, 40), 0, -90);
Camera g_camera(glm::vec3(0, 100, 40));
Block* placedBlock;
int g_renderDistance = 8;
World* world;
thread_local std::vector<float> g_mesh;
int max_mesh_size = 0;

void processInput(GLFWwindow* window, float deltaTime);

unsigned int g_polygons = 0;

int main()
{
	srand(time(NULL));
	if (Renderer::Init() != 0)
		return -1;

	g_mesh.reserve(90000);

	Gui::Init(Renderer::window);
	Resources::LoadTextures();
	Blocks::Initialize();
	BlockModelRegistry::Resize(BlockRegistry::GetBlockStateCount());
	BlockModels::Initialize();
	BlockColors::Initialize();

	placedBlock = Blocks::cobblestone;

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
		/*if (g_mesh.size() > max_mesh_size)
		{
			max_mesh_size = g_mesh.size();
			printf("size %d \n", max_mesh_size);
		}*/

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

	static bool justEditedWorld;

	if (!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS))
		justEditedWorld = false;

	if (!justEditedWorld) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			const float reach = 10.0f;
			auto end = g_camera.position + g_camera.facingDirection * reach;

			auto rayTraceResult = world->Clip(g_camera.position, end);

			if (rayTraceResult.hit) {
				world->SetBlock(rayTraceResult.blockPos, *Blocks::air->DefaultBlockState(), 3);
				justEditedWorld = true;
			}
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			const float reach = 10.0f;
			auto end = g_camera.position + g_camera.facingDirection * reach;

			auto rayTraceResult = world->Clip(g_camera.position, end);

			if (rayTraceResult.hit) {
				auto state = world->GetBlockState(rayTraceResult.blockPos);
				if (state->Use(*world, rayTraceResult.blockPos, rayTraceResult))
				{
					justEditedWorld = true;
				}
				else
				{
					auto useContext = BlockItemUseContext(world, rayTraceResult, *placedBlock, g_camera.facingDirection);
					if (useContext.Place(*placedBlock))
						justEditedWorld = true;
				}
			}
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

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) placedBlock = Blocks::cobblestone;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) placedBlock = Blocks::stone;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) placedBlock = Blocks::dirt;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) placedBlock = Blocks::grass;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) placedBlock = Blocks::wood;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) placedBlock = Blocks::slab;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) placedBlock = Blocks::door;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) placedBlock = Blocks::powerSensor;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) placedBlock = Blocks::redstoneWallTorch;
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) placedBlock = Blocks::redstoneWire;
}


