#include "ui.hpp"
#include "manager.hpp"
#include "terrain.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "time.hpp"

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

void UI::Start()
{
	std::cout << TEST_REPLACE_DEFINE << std::endl;
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
		ImGui::Begin("inspector");

		for (Menu &menu : menus)
		{
			if (ImGui::CollapsingHeader(menu.title.c_str()))
			{
				RenderComponents(menu);

				//for (Component &component : menu.components)
				//{
				//	if (component.type == TEXT_COMPONENT)
				//		RenderTextComponent(menu.textComponents[component.index]);
				//	else if (component.type == SLIDER_COMPONENT)
				//		RenderSliderComponent(menu.sliderComponents[component.index]);
				//}
			}
		}

		ImGui::End();
	}

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

void UI::RenderComponents(Menu &menu, int start, int end)
{
	for (int i = start; i < end; i++)
	{
		Component &component = menu.components[i];

		if (component.type == TEXT_COMPONENT)
			RenderTextComponent(menu.textComponents[component.index]);
		else if (component.type == FLOAT_SLIDER_COMPONENT)
			RenderSliderComponent(menu.floatSliderComponents[component.index]);
		else if (component.type == INT_SLIDER_COMPONENT)
			RenderSliderComponent(menu.intSliderComponents[component.index]);
		else if (component.type == FLOAT_RANGE_COMPONENT)
			RenderRangeComponent(menu.floatRangeComponents[component.index]);
		else if (component.type == INT_RANGE_COMPONENT)
			RenderRangeComponent(menu.intRangeComponents[component.index]);
		else if (component.type == BUTTON_COMPONENT)
			RenderButtonComponent(menu.buttonComponents[component.index]);
		else if (component.type == INT_INPUT_COMPONENT)
			RenderInputComponent(menu.intInputComponents[component.index]);
		else if (component.type == GRAPH_COMPONENT)
			RenderGraphComponent(menu.graphComponents[component.index]);
		else if (component.type == CHECK_COMPONENT)
			RenderCheckComponent(menu.checkComponents[component.index]);
		else if (component.type == FLOAT3_DRAG_COMPONENT)
			RenderDragComponent(menu.float3DragComponents[component.index]);

		if (component.type == NODE_COMPONENT)
		{
			int nodeStart = i + 1;
			int nodeEnd = FindNodeEnd(menu, menu.nodeComponents[component.index].name, nodeStart);

			if (ImGui::TreeNode(menu.nodeComponents[component.index].name.c_str()))
			{
				RenderComponents(menu, nodeStart, nodeEnd);
				ImGui::TreePop();
			}

			i = nodeEnd;
		}
	}
}

void UI::RenderComponents(Menu &menu)
{
	RenderComponents(menu, 0, menu.components.size());
}

void UI::RenderTextComponent(TextComponent &textComponent)
{
	ImGui::Text("%s", textComponent.content.c_str());
}

void UI::RenderSliderComponent(FloatSliderComponent &sliderComponent)
{
	ImGui::PushItemWidth(250.0f);
	ImGui::SliderFloat(sliderComponent.name.c_str(), &sliderComponent.value, sliderComponent.min, sliderComponent.max);
	ImGui::PopItemWidth();
}

void UI::RenderSliderComponent(IntSliderComponent &sliderComponent)
{
	ImGui::PushItemWidth(250.0f);
	ImGui::SliderInt(sliderComponent.name.c_str(), &sliderComponent.value, sliderComponent.min, sliderComponent.max);
	ImGui::PopItemWidth();
}

void UI::RenderRangeComponent(FloatRangeComponent &rangeComponent)
{
	ImGui::PushItemWidth(500.0f);
	ImGui::DragFloatRange2(rangeComponent.name.c_str(), &rangeComponent.value1, &rangeComponent.value2, 1, rangeComponent.min, 
		rangeComponent.max);
	ImGui::PopItemWidth();
}

void UI::RenderRangeComponent(IntRangeComponent &rangeComponent)
{
	ImGui::PushItemWidth(500.0f);
	ImGui::DragIntRange2(rangeComponent.name.c_str(), &rangeComponent.value1, &rangeComponent.value2, 1, rangeComponent.min,
		rangeComponent.max);
	ImGui::PopItemWidth();
}

void UI::RenderButtonComponent(ButtonComponent &buttonComponent)
{
	if (ImGui::Button(buttonComponent.name.c_str()))
	{
		buttonComponent.func();
	}
}

void UI::RenderInputComponent(IntInputComponent &inputComponent)
{
	ImGui::PushItemWidth(125.0f);
	ImGui::InputInt(inputComponent.name.c_str(), &inputComponent.value);
	ImGui::PopItemWidth();
}

void UI::RenderGraphComponent(GraphComponent &graphComponent)
{
	currentGraph = &graphComponent;

	std::vector<float> values(graphComponent.resolution);
	for (int i = 0; i < graphComponent.resolution; i++)
		values[i] = (graphComponent.curve.Evaluate(float(i) / float(graphComponent.resolution - 1)));

	ImGui::PlotLines(graphComponent.name.c_str(), values.data(), graphComponent.resolution, 0, NULL, 0.0f, 1.0f, ImVec2(0, 200.0f));

	currentGraph = nullptr;
}

void UI::RenderCheckComponent(CheckComponent &checkComponent)
{
	ImGui::Checkbox(checkComponent.name.c_str(), &checkComponent.value);
}

void UI::RenderDragComponent(Float3DragComponent &dragComponent)
{
	ImGui::DragFloat3(dragComponent.name.c_str(), &dragComponent.value[0], 0.01f);
}

int UI::FindNodeEnd(Menu &menu, std::string name, int start)
{
	for (int i = start; i < menu.components.size(); i++)
	{
		Component &component = menu.components[i];

		if (component.type == NODE_COMPONENT && menu.nodeComponents[component.index].name.compare(name) == 0)
			return (i);
	}

	return (menu.components.size());
}

void UI::DraggingGraphPoint(int index)
{
	//std::cout << "dragging " << index << std::endl;

	if (currentGraph == nullptr) return;

	float modification = 0.0f;
	if (Input::GetKey(GLFW_MOUSE_BUTTON_LEFT, true).down)
		modification = 0.5f * Time::deltaTime;
	else if (Input::GetKey(GLFW_MOUSE_BUTTON_RIGHT, true).down)
		modification = -0.5f * Time::deltaTime;
	currentGraph->curve.SetPoint(index, currentGraph->curve.GetPoint(index) + modification);
	//currentGraph->curve.SetPoint(index, currentGraph->curve.GetPoint(index) + io->MouseDelta.y * -0.01f);
	//currentGraph->curve.SetPoint(index + 1, currentGraph->curve.GetPoint(index + 1) + io->MouseDelta.y * -0.01f);
}

ImGuiIO* UI::io = nullptr;

std::vector<Menu> UI::menus;
GraphComponent *UI::currentGraph = nullptr;

bool UI::showFPS = true;
bool UI::enabled = false;