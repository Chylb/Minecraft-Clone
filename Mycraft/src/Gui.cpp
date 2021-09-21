#include "Resources.h"

#include "Gui.h"

#include "imgui/imgui_internal.h"
#include "renderer/Renderer.h"

extern const Block* g_items[9];
extern size_t g_selectedItemIx;

void Gui::Init(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 430";
	ImGui_ImplOpenGL3_Init(glsl_version);

	s_itemTextureMap[Blocks::cobblestone] = Resources::GetTexture("cobblestone");
	s_itemTextureMap[Blocks::stone] = Resources::GetTexture("stone");
	s_itemTextureMap[Blocks::dirt] = Resources::GetTexture("dirt");
	s_itemTextureMap[Blocks::grass] = Resources::GetTexture("grass_block_side");
	s_itemTextureMap[Blocks::leaves] = Resources::GetTexture("leaves");
	s_itemTextureMap[Blocks::grass] = Resources::GetTexture("grass_block_side");
	s_itemTextureMap[Blocks::wood] = Resources::GetTexture("oak_log");
	s_itemTextureMap[Blocks::debugBlock] = Resources::GetTexture("debug");
	s_itemTextureMap[Blocks::trapDoor] = Resources::GetTexture("oak_trapdoor");
	s_itemTextureMap[Blocks::wallTorch] = Resources::GetTexture("torch");
	s_itemTextureMap[Blocks::flower] = Resources::GetTexture("poppy");
	s_itemTextureMap[Blocks::plank] = Resources::GetTexture("oak_planks");
	s_itemTextureMap[Blocks::slab] = Resources::GetTexture("oak_planks");
	s_itemTextureMap[Blocks::door] = Resources::GetTexture("oak_door_top");
	s_itemTextureMap[Blocks::powerSensor] = Resources::GetTexture("sensor_off");
	s_itemTextureMap[Blocks::redstoneWire] = Resources::GetTexture("redstone_dust_line0");
	s_itemTextureMap[Blocks::redstoneWallTorch] = Resources::GetTexture("redstone_torch_off");
}

void Gui::Terminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Gui::RenderWindow(GLFWwindow* window, glm::vec3 camPos, int occupiedChunks, int freeChunks, int jobs, int polygons, int renderedChunks, const std::array<int, 4>& chunksLoadingStates)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (ImGui::IsKeyPressed(GLFW_KEY_ESCAPE))
	{
		if (s_showInventory)
		{
			s_showInventory = false;
			glfwSetInputMode(Renderer::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			s_showInventory = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	DrawCrosshair();
	DrawInfo(camPos, occupiedChunks, freeChunks, jobs, polygons, renderedChunks, chunksLoadingStates);

	if (s_showInventory)
		DrawInventory();
	else
		DrawHotbar();

	//ImGui::ShowMetricsWindow();
	if (s_showDemoWindow)
		ImGui::ShowDemoWindow(&s_showDemoWindow);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool Gui::IsCursorCaptured()
{
	return s_showInventory;
}

void Gui::DrawCrosshair()
{
	int iwidth, iheight;
	float width, height;
	glfwGetWindowSize(Renderer::window, &iwidth, &iheight);
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


void Gui::DrawInfo(glm::vec3 camPos, int occupiedChunks, int freeChunks, int jobs, int polygons, int renderedChunks, const std::array<int, 4>& chunksLoadingStates)
{
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({ 210,-1 });
	ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

	if (ImGui::Button("Exit"))
		glfwSetWindowShouldClose(Renderer::window, true);

	ImGui::SameLine();
	if (ImGui::Button("CLS"))
		s_showChunksLoadingStates = !s_showChunksLoadingStates;
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Chunk loading states");

	ImGui::SameLine();
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

	ImGui::Text("XYZ: %.1f / %.1f / %.1f", camPos.x, camPos.y, camPos.z);
	ImGui::Text("Chunks used %d / free %d", occupiedChunks, freeChunks);
	ImGui::Text("Polygons %d Jobs %d ", polygons, jobs);
	ImGui::Text("Rendered chunks %d", renderedChunks);

	ImGui::SliderFloat("Gui size", &s_scale, 1, 4);

	if (s_showChunksLoadingStates) {
		ImGui::NewLine();
		ImGui::Text("Chunks loading states:");
		ImGui::Text("Loading blocks %d ", chunksLoadingStates[0]);
		ImGui::Text("Loaded blocks %d ", chunksLoadingStates[1]);
		ImGui::Text("Generating mesh %d ", chunksLoadingStates[2]);
		ImGui::Text("Completed %d ", chunksLoadingStates[3]);
	}

	//ImGui::Checkbox("ImGui demo window", &s_showDemoWindow);

	ImGui::End();
}

void Gui::DrawInventory()
{
	const float inventoryTexWidth = 256;
	const float inventoryTexHeight = 256;
	const float inventoryWidth = 194 * s_scale;
	const float inventoryHeight = 135 * s_scale;

	ImGui::SetNextWindowSize({ -1,-1 });
	ImGui::SetNextWindowPos({ (float)(Renderer::window_width / 2.0 - inventoryWidth / 2.0),(float)(Renderer::window_height / 2.0 - inventoryHeight / 2.0) });
	ImGui::Begin("Inventory", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(ImColor(255, 255, 255, 100)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

	ImGui::Image((void*)Resources::GetTexture("tab_items"), ImVec2(inventoryTexWidth * s_scale, inventoryTexHeight * s_scale));

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2 * s_scale, 2 * s_scale));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2 * s_scale, 2 * s_scale));

	float padding = 8;
	int row = 0, col = 0;
	for (Block* block : BlockRegistry::GetBlocks())
	{
		if (block == Blocks::air || block == Blocks::torch || block == Blocks::redstoneTorch)
			continue;

		if (col++ % 9 != 0) {
			ImGui::SameLine();
		}
		else {
			row++;
			ImGui::SetCursorPos({ padding + s_scale * 9,padding + s_scale * 18 * row });
		}

		ImGui::PushID(block);
		ImGui::ImageButton((void*)s_itemTextureMap[block], { c_itemSize * s_scale,c_itemSize * s_scale });

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("item", &block, sizeof(void*));
			ImGui::Image((void*)s_itemTextureMap[block], { c_itemSize * s_scale,c_itemSize * s_scale });
			ImGui::EndDragDropSource();
		}
		ImGui::PopID();
	}

	int hotbarIx = 0;
	ImGui::SetCursorPos({ padding + s_scale * 9,padding + s_scale * 112 });
	for (const Block* block : g_items)
	{
		ImGui::PushID(block);

		ImGui::ImageButton((void*)s_itemTextureMap[block], { c_itemSize * s_scale,c_itemSize * s_scale });
		ImGui::SameLine();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("item"))
			{
				IM_ASSERT(payload->DataSize == sizeof(void*));
				const Block* blk_src = *(Block**)payload->Data;
				g_items[hotbarIx] = blk_src;
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
		hotbarIx++;
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	ImGui::End();
}

void Gui::DrawHotbar()
{
	const int padding = 8;

	const float hotbarTexSize = 256;
	const int hotbarWidth = 182;
	const int hotbarHeight = 22;

	const ImVec2 uv0 = { 0,0 };
	const ImVec2 uv1 = { hotbarWidth / hotbarTexSize, hotbarHeight / hotbarTexSize };

	const ImVec2 pos = { (float)(Renderer::window_width / 2.0 - hotbarWidth * s_scale / 2.0),(float)(Renderer::window_height - hotbarHeight * s_scale - padding) };
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize({ -1,-1 });
	ImGui::Begin("Hotbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs);
	ImGui::Image((void*)Resources::GetTexture("widgets"), ImVec2(hotbarWidth * s_scale, hotbarHeight * s_scale), uv0, uv1);

	const int slotStep = 20 * s_scale;
	for (int i = 0; i < 9; i++)
	{
		const ImVec2 itemPos = { padding + (float)i * slotStep + 5 * s_scale, padding + 5 * s_scale };
		ImGui::SetCursorPos(itemPos);
		ImGui::Image((void*)s_itemTextureMap[g_items[i]], { c_itemSize * s_scale, c_itemSize * s_scale });
	}

	const int selectedItemFrameSize = 24;
	const int selectedItemFrameRealSize = s_scale * 24;
	const ImVec2 selectedItemUv0 = { 0, hotbarHeight / hotbarTexSize };
	const ImVec2 selectedItemUv1 = { selectedItemFrameSize / hotbarTexSize, (hotbarHeight + selectedItemFrameSize) / hotbarTexSize };

	ImGui::SetCursorPos({ padding - s_scale + (float)g_selectedItemIx * slotStep,padding - s_scale });
	ImGui::Image((void*)Resources::GetTexture("widgets"), ImVec2(selectedItemFrameRealSize, selectedItemFrameRealSize), selectedItemUv0, selectedItemUv1);

	ImGui::End();
}
