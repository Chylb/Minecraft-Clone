#pragma once

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/ext/vector_float3.hpp>

#include <array>

#include "Resources.h"
#include "world/block/Blocks.h"
#include "world/block/Block.h"

class Gui
{
public:
	static void Init(GLFWwindow* window);
	static void Terminate();
	static void RenderWindow(GLFWwindow* window, glm::vec3 camPos, int occupiedChunks, int freeChunks, int jobs, int polygons, const std::array<int, 4>& chunksLoadingStates);
	static bool IsCursorCaptured();

private:
	inline static bool s_showDemoWindow = false;
	inline static bool s_showChunksLoadingStates = false;
	inline static bool s_showInventory = false;
	inline static float s_scale = 2.0f;

	inline static std::unordered_map<const Block*, GLuint> s_itemTextureMap;

	inline static constexpr float c_itemSize = 16 * 0.75f;

	static void DrawCrosshair();
	static void DrawInfo(glm::vec3 camPos, int occupiedChunks, int freeChunks, int jobs, int polygons, const std::array<int, 4>& chunksLoadingStates);
	static void DrawInventory();
	static void DrawHotbar();
};
