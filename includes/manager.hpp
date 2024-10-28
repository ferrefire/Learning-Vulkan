#pragma once

#include "window.hpp"
#include "graphics.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "texture.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

struct Settings
{
	uint32_t maxFramesInFlight = 2;
	bool fullscreen = false;
	bool wireframe = false;
	bool discrete = true;
	VkSampleCountFlagBits maxSampleCount = VK_SAMPLE_COUNT_1_BIT;
	//VkSampleCountFlagBits maxSampleCount = VK_SAMPLE_COUNT_64_BIT;
	const std::vector<const char*> validationLayers = {	"VK_LAYER_KHRONOS_validation" };
	const bool validationLayersActive = true;
};

class Manager
{
	private:
        static bool glfwInitialized;
        static bool vulkanInitialized;

		static Window window;
		static Camera camera;
		static Device device;
		static Graphics graphics;
		static std::vector<Pipeline *> pipelines;
		static std::vector<Texture *> textures;
		static std::vector<Mesh *> meshes;

	public:
		static Settings settings;

		static Window &currentWindow;
		static Camera &currentCamera;
		static Device &currentDevice;
		static Graphics &currentGraphics;

		static uint32_t currentFrame;

		static std::vector<Object *> objects;

		static void Start();
		static void Clean();
		static void Quit(int exitCode);

		static void InitializeGLFW();
		static void InitializeVulkan();

		static void Frame();

		static void DestroyPipelines();
		static void DestroyTextures();
		static void DestroyMeshes();
		static void DestroyObjects();

		static Window &GetWindow();
		static Graphics &GetGraphics();

		static Pipeline *NewPipeline();
		static Texture *NewTexture();
		static Mesh *NewMesh();
		static Object *NewObject();
};