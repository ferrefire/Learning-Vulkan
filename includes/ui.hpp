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
#define FLOAT_SLIDER_COMPONENT 2
#define INT_SLIDER_COMPONENT 3
#define FLOAT_RANGE_COMPONENT 4
#define INT_RANGE_COMPONENT 5
#define BUTTON_COMPONENT 6
#define INT_INPUT_COMPONENT 7
#define GRAPH_COMPONENT 8
#define CHECK_COMPONENT 9
#define FLOAT3_DRAG_COMPONENT 10
#define COLOR_COMPONENT 11
#define DROPDOWN_COMPONENT 12

#define TEST_REPLACE_DEFINE "another replaceeeeeed"

struct DropdownComponent
{
	std::string name = "dropdown";
	std::vector<std::string> options = {"option 1"};
	int &value;
	void(*func)(void) = nullptr;
};

struct ColorComponent
{
	std::string name = "color";
	glm::vec4 &value;
};

struct Float3DragComponent
{
	std::string name = "drag";
	glm::vec3 &value;
	float speed = 0.1f;
};

struct CheckComponent
{
	std::string name = "check";
	bool &value;
};

struct GraphComponent
{
	std::string name = "graph";
	uint32_t resolution = 16;
	Curve &curve;
};

struct IntInputComponent
{
	std::string name = "input";
	int &value;
};

struct ButtonComponent
{
	std::string name = "button";
	void(*func)(void);
};

struct FloatSliderComponent
{
	std::string name = "slider";
	float &value;
	float min = 0.0f;
	float max = 1.0f;
	void(*func)(void);
};

struct IntSliderComponent
{
	std::string name = "slider";
	int &value;
	int min = 0;
	int max = 1;
	void(*func)(void);
};

struct FloatRangeComponent
{
	std::string name = "range";
	float &value1;
	float &value2;
	float min = 0.0f;
	float max = 1.0f;
};

struct IntRangeComponent
{
	std::string name = "range";
	int &value1;
	int &value2;
	int min = 0;
	int max = 1;
};

struct TextComponent
{
	std::string content = "text";
};

struct NodeComponent
{
	std::string name = "node";
	bool begin = true;
	void(*func)(void) = nullptr;
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
	std::vector<FloatSliderComponent> floatSliderComponents;
	std::vector<IntSliderComponent> intSliderComponents;
	std::vector<FloatRangeComponent> floatRangeComponents;
	std::vector<IntRangeComponent> intRangeComponents;
	std::vector<ButtonComponent> buttonComponents;
	std::vector<IntInputComponent> intInputComponents;
	std::vector<GraphComponent> graphComponents;
	std::vector<CheckComponent> checkComponents;
	std::vector<Float3DragComponent> float3DragComponents;
	std::vector<ColorComponent> colorComponents;
	std::vector<DropdownComponent> dropdownComponents;

	void AddNode(std::string name, bool begin, void(*func)(void) = nullptr)
	{
		NodeComponent newNodeComponent;
		newNodeComponent.name = name;
		newNodeComponent.begin = begin;
		newNodeComponent.func = func;
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

	void AddSlider(std::string name, float &value, float min, float max, void(*func)(void) = nullptr)
	{
		FloatSliderComponent newFloatSliderComponent{name, value, min, max, func};
		floatSliderComponents.push_back(newFloatSliderComponent);

		Component newComponent;
		newComponent.type = FLOAT_SLIDER_COMPONENT;
		newComponent.index = floatSliderComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddSlider(std::string name, int &value, int min, int max, void(*func)(void) = nullptr)
	{
		IntSliderComponent newIntSliderComponent{name, value, min, max, func};
		intSliderComponents.push_back(newIntSliderComponent);

		Component newComponent;
		newComponent.type = INT_SLIDER_COMPONENT;
		newComponent.index = intSliderComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddRange(std::string name, float &value1, float &value2, float min, float max)
	{
		FloatRangeComponent newFloatRangeComponent{name, value1, value2, min, max};
		floatRangeComponents.push_back(newFloatRangeComponent);

		Component newComponent;
		newComponent.type = FLOAT_RANGE_COMPONENT;
		newComponent.index = floatRangeComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddRange(std::string name, int &value1, int &value2, int min, int max)
	{
		IntRangeComponent newIntRangeComponent{name, value1, value2, min, max};
		intRangeComponents.push_back(newIntRangeComponent);

		Component newComponent;
		newComponent.type = INT_RANGE_COMPONENT;
		newComponent.index = intRangeComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddButton(std::string name, void(*func)(void))
	{
		ButtonComponent newButtonComponent{name, func};
		buttonComponents.push_back(newButtonComponent);

		Component newComponent;
		newComponent.type = BUTTON_COMPONENT;
		newComponent.index = buttonComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddInput(std::string name, int &value)
	{
		IntInputComponent newIntInputComponent{name, value};
		intInputComponents.push_back(newIntInputComponent);

		Component newComponent;
		newComponent.type = INT_INPUT_COMPONENT;
		newComponent.index = intInputComponents.size() - 1;
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

	void AddCheck(std::string name, bool &value)
	{
		CheckComponent newCheckComponent{name, value};
		checkComponents.push_back(newCheckComponent);

		Component newComponent;
		newComponent.type = CHECK_COMPONENT;
		newComponent.index = checkComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddDrag(std::string name, glm::vec3 &value, float speed)
	{
		Float3DragComponent newFloat3DragComponent{name, value, speed};
		float3DragComponents.push_back(newFloat3DragComponent);

		Component newComponent;
		newComponent.type = FLOAT3_DRAG_COMPONENT;
		newComponent.index = float3DragComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddColor(std::string name, glm::vec4 &value)
	{
		ColorComponent newColorComponent{name, value};
		colorComponents.push_back(newColorComponent);

		Component newComponent;
		newComponent.type = COLOR_COMPONENT;
		newComponent.index = colorComponents.size() - 1;
		components.push_back(newComponent);
	}

	void AddDropdown(std::string name, std::vector<std::string> options, int &value, void(*func)(void) = nullptr)
	{
		DropdownComponent newDropdownComponent{name, options, value, func};
		dropdownComponents.push_back(newDropdownComponent);

		Component newComponent;
		newComponent.type = DROPDOWN_COMPONENT;
		newComponent.index = dropdownComponents.size() - 1;
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
		static void RenderSliderComponent(FloatSliderComponent &sliderComponent);
		static void RenderSliderComponent(IntSliderComponent &sliderComponent);
		static void RenderRangeComponent(FloatRangeComponent &rangeComponent);
		static void RenderRangeComponent(IntRangeComponent &rangeComponent);
		static void RenderButtonComponent(ButtonComponent &buttonComponent);
		static void RenderInputComponent(IntInputComponent &inputComponent);
		static void RenderGraphComponent(GraphComponent &graphComponent);
		static void RenderCheckComponent(CheckComponent &checkComponent);
		static void RenderDragComponent(Float3DragComponent &dragComponent);
		static void RenderColorComponent(ColorComponent &colorComponent);
		static void RenderDropdownComponent(DropdownComponent &dropdownComponent);

		static int FindNodeEnd(Menu &menu, std::string name, int start);
		static void DraggingGraphPoint(int index);

		static Menu &NewMenu(std::string title);
};