#pragma once

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

#define TEXT_COMPONENT 0
#define SLIDER_COMPONENT 1
#define BUTTON_COMPONENT 2

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

struct Component
{
	int type = 0;
	int index = 0;
};

struct Menu
{
	std::string title = "new menu";
	std::vector<Component> components;
	std::vector<TextComponent> textComponents;
	std::vector<SliderComponent> sliderComponents;
	std::vector<ButtonComponent> buttonComponents;

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
};

class UI
{
	private:
		
	public:
		static ImGuiIO *io;

		static std::vector<Menu> menus;

		static bool showFPS;
		static bool enabled;

		static void Create();
		static void CreateContext();

		static void Destroy();
		static void DestroyContext();

		static void Frame();
		static void RecordGraphicsCommands(VkCommandBuffer commandBuffer);
		static void MultiWindows();
		static void TriggerUI(bool mode);
		static void TriggerKeyboardInput(bool mode);
		static void TriggerMouseInput(bool mode);

		static void RenderFPS();
		static void RenderTextComponent(TextComponent &textComponent);
		static void RenderSliderComponent(SliderComponent &sliderComponent);

		static Menu &NewMenu(std::string title);
};