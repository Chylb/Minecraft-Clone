#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "../Gui.h"

class Renderer
{
public:
	static int Init();
	static void Terminate();

	static float BeginRendering();
	static void EndRendering();

	static inline GLFWwindow* window;
	static inline int window_width = 800, window_height = 600;

private:
	inline static double s_mouseLastX, s_mouseLastY;

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};
