#include "Renderer.h"

#include "../Camera.h"
extern Camera g_camera;

int Renderer::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_width, window_height, "Mycraft", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW!\n";
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 0;
}

void Renderer::Terminate()
{
	glfwTerminate();
}

float Renderer::BeginRendering()
{
	glfwGetWindowSize(window, &window_width, &window_height);

	static float dt;
	static float lastFrame;

	glClearColor(0.2f, 0.6f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float currentFrame = glfwGetTime();
	dt = currentFrame - lastFrame;
	lastFrame = currentFrame;

	return dt;
}

void Renderer::EndRendering()
{
	glfwSwapBuffers(Renderer::window);
	glfwPollEvents();
}

void Renderer::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float Xoffset = xpos - s_mouseLastX;
	float Yoffset = s_mouseLastY - ypos;

	s_mouseLastX = xpos;
	s_mouseLastY = ypos;

	if (!Gui::IsCursorCaptured())
		g_camera.ProcessMouseMovement(Xoffset, Yoffset);
}

void Renderer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	g_camera.ProcessMouseScroll(yoffset);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
