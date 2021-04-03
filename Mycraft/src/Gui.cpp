#include "Gui.h"

void Gui::Init(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 430";
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void Gui::Terminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Gui::RenderWindow(GLFWwindow* window, glm::vec3 camPos, int occupiedChunks, int freeChunks, int jobs, int polygons)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (s_showDemoWindow)
		ImGui::ShowDemoWindow(&s_showDemoWindow);
	{
		ImGui::SetNextWindowPos({ 0,0 });
		ImGui::SetNextWindowSize({ 210,-1 });
		ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

		if (ImGui::Button("Exit"))
			glfwSetWindowShouldClose(window, true);

		ImGui::SameLine();
		if (ImGui::Button("Focus")) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cursorCaptured = true;
		}

		ImGui::SameLine();
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

		ImGui::Text("XYZ: %.1f / %.1f / %.1f", camPos.x, camPos.y, camPos.z);
		ImGui::Text("Chunks used %d / free %d", occupiedChunks, freeChunks);
		ImGui::Text("Polygons %d Jobs %d ", polygons, jobs);

		//ImGui::Checkbox("ImGui demo window", &show_demo_window);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
