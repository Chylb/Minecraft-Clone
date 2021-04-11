#pragma once

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/ext/vector_float3.hpp>

#include <array>

class Gui
{
public:
	static void Init(GLFWwindow* window);
	static void Terminate();
	static void RenderWindow(GLFWwindow* window, glm::vec3 camPos, int occupiedChunks, int freeChunks, int jobs, int polygons, const std::array<int, 5>& chunksLoadingStates);

	inline static bool cursorCaptured = true;

private:
	inline static bool s_showDemoWindow = false;
	inline static bool s_showChunksLoadingStates = false;
};
