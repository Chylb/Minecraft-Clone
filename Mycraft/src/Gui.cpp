#include "Gui.h"

//#include "world/chunk/Chunk.h"

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

void Gui::RenderWindow(GLFWwindow* window, glm::vec3 camPos, int occupiedChunks, int freeChunks, int jobs, int polygons, const std::array<int, 4>& chunksLoadingStates)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (s_showDemoWindow)
		ImGui::ShowDemoWindow(&s_showDemoWindow);

	{
		int iwidth, iheight;
		float width, height;
		glfwGetWindowSize(window, &iwidth, &iheight);
		width = (float)iwidth;
		height = (float)iheight;

		ImGui::SetNextWindowPos({ 0,0 });
		ImGui::SetNextWindowSize({ (float)width,(float)height });
		ImGui::Begin("Crosshair", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs);
		ImColor yellow{ 255,255,100 };

		float crosshairLength = 5;
		ImGui::GetWindowDrawList()->AddLine({ width / 2,height / 2 - crosshairLength + 1 }, { width / 2,height / 2 + crosshairLength }, yellow);
		ImGui::GetWindowDrawList()->AddLine({ width / 2 - crosshairLength + 1,height / 2 }, { width / 2 + crosshairLength ,height / 2 }, yellow);

		ImGui::End();
	}

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
		if (ImGui::Button("CLS"))
			s_showChunksLoadingStates = !s_showChunksLoadingStates;

		ImGui::SameLine();
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

		ImGui::Text("XYZ: %.1f / %.1f / %.1f", camPos.x, camPos.y, camPos.z);
		ImGui::Text("Chunks used %d / free %d", occupiedChunks, freeChunks);
		ImGui::Text("Polygons %d Jobs %d ", polygons, jobs);

		if (s_showChunksLoadingStates) {
			ImGui::NewLine();
			ImGui::Text("Chunks loading states:");
			ImGui::Text("Loading blocks %d ", chunksLoadingStates[0]);
			ImGui::Text("Loaded blocks %d ", chunksLoadingStates[1]);
			ImGui::Text("Generating mesh %d ", chunksLoadingStates[2]);
			ImGui::Text("Completed %d ", chunksLoadingStates[3]);
		}

		//ImGui::Checkbox("ImGui demo window", &show_demo_window);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
