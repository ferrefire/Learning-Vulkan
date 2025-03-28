#pragma once

#include "curve.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>

#define NODE_COMPONENT 0
#define TEXT_COMPONENT 1
#define SLIDER_COMPONENT 2
#define BUTTON_COMPONENT 3
#define GRAPH_COMPONENT 4

#define TEST_REPLACE_DEFINE "another replaceeeeeed"

struct GraphComponent
{
	std::string name = "graph";
	uint32_t resolution = 16;
	Curve &curve;
};

struct ButtonComponent
{
	std::string name = "button";
};

struct SliderComponent
{
	std::string name = "slider";
	float &value;
	float min = 0.0f;
	float max = 1.0f;
};

struct TextComponent
{
	std::string content = "text";
};

struct NodeComponent
{
	std::string name = "node";
	bool begin = true;
};

struct Component
{
	int type = 0;
	int index = 0;
};

struct Menu
{
	std::string title = "new menu";
	NodeComponent *currentNode = nullptr;

	std::vector<Component> components;
	std::vector<NodeComponent> nodeComponents;
	std::vector<TextComponent> textComponents;
	std::vector<SliderComponent> sliderComponents;
	std::vector<ButtonComponent> buttonComponents;
	std::vector<GraphComponent> graphComponents;

	void AddNode(std::string name, bool begin)
	{
		NodeComponent newNodeComponent;
		newNodeComponent.name = name;
		newNodeComponent.begin = begin;
		nodeComponents.push_back(newNodeComponent);

		Component newComponent;
		newComponent.type = NODE_COMPONENT;
		newComponent.index = nodeComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddText(std::string content)
	{
		TextComponent newTextComponent;
		newTextComponent.content = content;
		textComponents.push_back(newTextComponent);

		Component newComponent;
		newComponent.type = TEXT_COMPONENT;
		newComponent.index = textComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddSlider(std::string name, float &value, float min, float max)
	{
		SliderComponent newSliderComponent{name, value, min, max};
		sliderComponents.push_back(newSliderComponent);

		Component newComponent;
		newComponent.type = SLIDER_COMPONENT;
		newComponent.index = sliderComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddGraph(std::string name, uint32_t resolution, Curve &curve)
	{
		GraphComponent newGraphComponent{name, resolution, curve};
		graphComponents.push_back(newGraphComponent);

		Component newComponent;
		newComponent.type = GRAPH_COMPONENT;
		newComponent.index = graphComponents.size() - 1;
		components.push_back(newComponent);
	}
};

class UI
{
	private:
		static GraphComponent *currentGraph;

	public:
		static ImGuiIO *io;

		static std::vector<Menu> menus;

		static bool showFPS;
		static bool enabled;

		static void Create();
		static void CreateContext();

		static void Destroy();
		static void DestroyContext();

		static void Start();
		static void Frame();
		static void RecordGraphicsCommands(VkCommandBuffer commandBuffer);
		static void MultiWindows();
		static void TriggerUI(bool mode);
		static void TriggerKeyboardInput(bool mode);
		static void TriggerMouseInput(bool mode);

		static void RenderFPS();
		static void RenderComponents(Menu &menu, int start, int end);
		static void RenderComponents(Menu &menu);
		static void RenderTextComponent(TextComponent &textComponent);
		static void RenderSliderComponent(SliderComponent &sliderComponent);
		static void RenderGraphComponent(GraphComponent &graphComponent);

		static int FindNodeEnd(Menu &menu, std::string name, int start);
		static void DraggingGraphPoint(int index);

		static Menu &NewMenu(std::string title);
};