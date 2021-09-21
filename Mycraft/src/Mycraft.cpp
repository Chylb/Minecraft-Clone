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
#include <numbers>

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
#include "utils/MathUtils.h"

//Camera g_camera(glm::vec3(0, 2000, 40), 0, -90);
Camera g_camera(glm::vec3(0, 100, 40));
//int g_renderDistance = 8;
int g_renderDistance = 24;
World* world;
thread_local std::vector<float> g_mesh;
int max_mesh_size = 0;

void processInput(GLFWwindow* window, float deltaTime);
void renderWorld(World* world, const glm::mat4& frustum);

unsigned int g_polygons = 0;
unsigned int g_renderedChunks = 0;

const Block* g_items[9];
size_t g_selectedItemIx = 0;

int main()
{
	srand(time(NULL));
	if (Renderer::Init() != 0)
		return -1;

	g_mesh.reserve(193860);

	Resources::Initialize();
	Blocks::Initialize();
	BlockModelRegistry::Resize(BlockRegistry::GetBlockStateCount());
	BlockModels::Initialize();
	BlockColors::Initialize();

	Gui::Init(Renderer::window);

	g_items[0] = Blocks::cobblestone;
	g_items[1] = Blocks::dirt;
	g_items[2] = Blocks::grass;
	g_items[3] = Blocks::wood;
	g_items[4] = Blocks::slab;
	g_items[5] = Blocks::door;
	g_items[6] = Blocks::powerSensor;
	g_items[7] = Blocks::redstoneWallTorch;
	g_items[8] = Blocks::redstoneWire;

	world = new World();

	Shader basicShader("res/basic.vs", "res/basic.fs");
	basicShader.use();

	static Timer timer("Update", 100);

	while (!glfwWindowShouldClose(Renderer::window)) {
		float dt = Renderer::BeginRendering();

		processInput(Renderer::window, dt);

		int width, height;
		glfwGetWindowSize(Renderer::window, &width, &height);
		glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom), (float)width / (float)height, 0.1f, 3000.0f);
		basicShader.setMat4("projection", projection);

		glm::mat4 view = g_camera.GetViewMatrix();
		basicShader.setMat4("view", view);

		timer.begin();
		world->Update();
		world->UpdateMeshes();
		timer.finish();

		glm::mat4 projectionView = projection * view;
		renderWorld(world, projectionView);

		/*if (g_mesh.size() > max_mesh_size)
		{
			max_mesh_size = g_mesh.size();
			printf("size %d \n", max_mesh_size);
		}*/

		std::array<int, 4> chunksLoadingStates = world->DEV_ChunksLoadingStates();
		Gui::RenderWindow(Renderer::window, g_camera.position, world->OccupiedChunkCount(), world->FreeChunkCount(), 0, g_polygons, g_renderedChunks, chunksLoadingStates);

		Renderer::EndRendering();
	}

	Gui::Terminate();

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window, float deltaTime)
{
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
					auto useContext = BlockItemUseContext(world, rayTraceResult, *g_items[g_selectedItemIx], g_camera.facingDirection);
					if (useContext.Place(*g_items[g_selectedItemIx]))
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

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) g_selectedItemIx = 0;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) g_selectedItemIx = 1;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) g_selectedItemIx = 2;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) g_selectedItemIx = 3;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) g_selectedItemIx = 4;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) g_selectedItemIx = 5;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) g_selectedItemIx = 6;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) g_selectedItemIx = 7;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) g_selectedItemIx = 8;
}

void renderWorld(World* world, const glm::mat4& frustum)
{
	auto planes = MathUtils::extractFrustumPlanes(frustum);
	constexpr float chunkRadius = std::numbers::sqrt3_v<float> / 2.0f * Chunk::CHUNK_WIDTH;
	constexpr float minChunkY = chunkRadius;
	constexpr float maxChunkY = Chunk::CHUNK_HEIGHT - chunkRadius;

	g_renderedChunks = 0;
	for (const Chunk& chunk : world->GetChunks()) {
		if (chunk.loadingState < Chunk::LoadingState::completed)
			continue;

		const float chunkX = chunk.GetPos().x * Chunk::CHUNK_WIDTH + Chunk::CHUNK_WIDTH / 2;
		const float chunkZ = chunk.GetPos().z * Chunk::CHUNK_WIDTH + Chunk::CHUNK_WIDTH / 2;

		for (int i = 0; i < 4; i++)
		{
			glm::vec4& plane = planes[i];
			float chunkY = -(plane.x * chunkX + plane.z * chunkZ + plane.w) / plane.y; //finds such y, that lies on a frustum plane
			chunkY = MathUtils::clamp(chunkY, minChunkY, maxChunkY);

			bool shouldDraw = true;
			for (auto& checkedPlane : planes) {
				float dis = checkedPlane.x * chunkX + checkedPlane.y * chunkY + checkedPlane.z * chunkZ + checkedPlane.w;
				if (dis < -chunkRadius) {
					shouldDraw = false;
					break;
				}
			}

			if (shouldDraw)
			{
				g_renderedChunks++;
				chunk.Render();
				break;
			}
		}
	}
}

