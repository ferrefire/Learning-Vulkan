#include "ui.hpp"
#include "manager.hpp"
#include "terrain.hpp"
#include "graphics.hpp"

#include <iostream>

void UI::Create()
{
	UI::CreateContext();
}

/*static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}*/

void UI::CreateContext()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO &io = ImGui::GetIO();
	io = &ImGui::GetIO();
	//(void)io;
	//io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	//io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_NoMouse;
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	//io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	io->ConfigDockingWithShift = true;
	io->ConfigWindowsMoveFromTitleBarOnly = true;
	// io->ConfigViewportsNoAutoMerge = true;
	// io->ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

	ImGui_ImplGlfw_InitForVulkan(Manager::currentWindow.data, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.ApiVersion = VK_API_VERSION_1_3;
	init_info.Instance = Manager::currentGraphics.instance;
	init_info.PhysicalDevice = Manager::currentDevice.physicalDevice;
	init_info.Device = Manager::currentDevice.logicalDevice;
	init_info.QueueFamily = Manager::currentDevice.queueFamilies.graphicsFamily;
	init_info.Queue = Manager::currentDevice.graphicsQueue;
	init_info.PipelineCache = nullptr;
	init_info.DescriptorPool = Manager::currentDevice.descriptorPool;
	init_info.RenderPass = Manager::currentWindow.renderPass;
	init_info.Subpass = 1;
	init_info.MinImageCount = 2;
	init_info.ImageCount = 2;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	//init_info.CheckVkResultFn = check_vk_result;

	ImGui_ImplVulkan_Init(&init_info);
}

void UI::Destroy()
{
	UI::DestroyContext();
}

void UI::DestroyContext()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	io = nullptr;
}

void UI::Frame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (enabled || showFPS)
	{
		RenderFPS();
	}

	if (enabled)
	{
		for (Menu &menu : menus)
		{
			ImGui::Begin(menu.title.c_str());
		
			for (Component &component : menu.components)
			{
				if (component.type == TEXT_COMPONENT) RenderTextComponent(menu.textComponents[component.index]);
				else if (component.type == SLIDER_COMPONENT) RenderSliderComponent(menu.sliderComponents[component.index]);
			}
		
			ImGui::End();
		}
	}

	//static float f = 0.0f;
	//static int counter = 0;

	//ImGui::Begin("Hello, world!");
	//ImGui::Text("This is some useful text.");
	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	//ImGui::SliderFloat("water height", &Terrain::waterHeight, 0.0f, 1000.0f);
	//if (ImGui::Button("Button")) counter++;
	//ImGui::SameLine();
	//ImGui::Text("counter = %d", counter);
	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
	//ImGui::End();

	//ImGui::Begin("Another window!");
	//ImGui::Text("This is some useful text.");
	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	//if (ImGui::Button("Button")) counter++;
	//ImGui::SameLine();
	//ImGui::Text("counter = %d", counter);
	//ImGui::End();

	ImGui::Render();
}

void UI::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
	if (!io) return;

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

void UI::MultiWindows()
{
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void UI::TriggerUI(bool mode)
{
	if (!io) return;

	enabled = mode;
}

void UI::TriggerMouseInput(bool mode)
{
	if (!io) return;

	if (!mode) io->ConfigFlags |= ImGuiConfigFlags_NoMouse;
	else io->ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void UI::TriggerKeyboardInput(bool mode)
{
	if (!io) return;

	if (mode) io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	else io->ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
}

Menu &UI::NewMenu(std::string title)
{
	Menu newMenu;
	newMenu.title = title;

	menus.push_back(newMenu);
	return (menus[menus.size() - 1]);
}

void UI::RenderFPS()
{
	if (!io) return;

	ImGui::Begin("statistics");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
	ImGui::End();
}

void UI::RenderTextComponent(TextComponent &textComponent)
{
	ImGui::Text("%s", textComponent.content.c_str());
}

void UI::RenderSliderComponent(SliderComponent &sliderComponent)
{
	ImGui::SliderFloat(sliderComponent.name.c_str(), &sliderComponent.value, sliderComponent.min, sliderComponent.max);
}

ImGuiIO* UI::io = nullptr;

std::vector<Menu> UI::menus;

bool UI::showFPS = false;
bool UI::enabled = false;