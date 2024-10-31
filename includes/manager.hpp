#pragma once

#include "window.hpp"
#include "graphics.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "texture.hpp"
#include "buffer.hpp"
#include "descriptor.hpp"
#include "cinematic.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct Settings
{
	uint32_t maxFramesInFlight = 2;
	bool fullscreen = false;
	bool wireframe = false;
	bool discrete = true;
	bool mssa = false;
	VkSampleCountFlagBits maxSamples = VK_SAMPLE_COUNT_4_BIT;
	//VkSampleCountFlagBits maxSampleCount = VK_SAMPLE_COUNT_64_BIT;
	const std::vector<const char*> validationLayers = {	"VK_LAYER_KHRONOS_validation" };
	bool validationLayersActive = true;
	bool uncappedFPS = false;
};

struct ShaderVariables
{
	alignas(16) glm::mat4 view = glm::mat4(1);
	alignas(16) glm::mat4 projection = glm::mat4(1);
	alignas(16) glm::vec3 viewPosition = glm::vec3(0);
	alignas(16) glm::vec3 viewDirection = glm::vec3(0);
	alignas(16) glm::vec3 viewRight = glm::vec3(0);
	alignas(16) glm::vec3 viewUp = glm::vec3(0);
	alignas(16) glm::vec4 resolution = glm::vec4(0);

	alignas(8) glm::vec2 terrainOffset;
	alignas(8) glm::vec2 terrainLod0Offset;
	alignas(8) glm::vec2 terrainLod1Offset;
	alignas(4) float terrainTotalSize;
	alignas(4) float terrainTotalSizeMult;
	alignas(4) int terrainChunksLength;
	alignas(4) float terrainChunksLengthMult;
};

class Manager
{
	private:
        static bool glfwInitialized;
        static bool vulkanInitialized;

		static Window window;
		static Device device;
		static Graphics graphics;
		static std::vector<Pipeline *> pipelines;
		static std::vector<Texture *> textures;
		static std::vector<Mesh *> meshes;

	public:
		static Settings settings;
		static ShaderVariables shaderVariables;

		static Window &currentWindow;
		static Device &currentDevice;
		static Graphics &currentGraphics;

		static Camera camera;

		static uint32_t currentFrame;
		//static VkCommandBuffer currentBuffer;

		static std::vector<Buffer> shaderVariableBuffers;
		static VkDescriptorSetLayout globalDescriptorSetLayout;
		static Descriptor globalDescriptor;

		static std::vector<Object *> objects;

		static Cinematic cinematic;

		static void Setup();
		static void Create();
		static void Clean();
		static void Quit(int exitCode);

		static void InitializeGLFW();
		static void InitializeVulkan();
		static void CreateShaderVariableBuffers();
		static void CreateDescriptor();

		static void Start();
		static void PreFrame();
		static void Frame();
		static void PostFrame();
		static void UpdateShaderVariables();

		static void DestroyPipelines();
		static void DestroyTextures();
		static void DestroyMeshes();
		static void DestroyObjects();
		static void DestroyShaderVariableBuffers();
		static void DestroyDescriptor();

		static Window &GetWindow();
		static Graphics &GetGraphics();

		static Pipeline *NewPipeline();
		static Texture *NewTexture();
		static Mesh *NewMesh();
		static Object *NewObject();
};