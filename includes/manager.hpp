#pragma once

#ifndef SHADOW_CASCADE_COUNT
#define SHADOW_CASCADE_COUNT 5
#endif

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
	uint32_t extraFrames = 1;
	uint32_t maxFramesInFlight = 2;
	bool fullscreen = false;
	bool wireframe = false;
	bool discrete = true;
	bool msaa = false;
	VkSampleCountFlagBits maxSamples = VK_SAMPLE_COUNT_2_BIT;
	//VkSampleCountFlagBits maxSampleCount = VK_SAMPLE_COUNT_64_BIT;
	const std::vector<const char*> validationLayers = {	"VK_LAYER_KHRONOS_validation" };
	bool validationLayersActive = true;
	bool uncappedFPS = false;
	bool screenQuad = true;
	bool occlussionCulling = true;
	bool trees = true;
	bool shadows = true;
	bool performanceMode = false;
	bool startTimes = false;
	bool frameTimes = false;
};

struct ShaderVariables
{
	alignas(16) glm::mat4 view = glm::mat4(1);
	alignas(16) glm::mat4 projection = glm::mat4(1);
	alignas(16) glm::mat4 viewMatrix = glm::mat4(1);
	alignas(16) glm::mat4 viewLodMatrix = glm::mat4(1);
	alignas(16) glm::mat4 shadowCascadeMatrix[SHADOW_CASCADE_COUNT];
	alignas(16) glm::mat4 cullMatrix = glm::mat4(1);
	alignas(4) float viewHeight = 0;
	alignas(16) glm::vec3 viewPosition = glm::vec3(0);
	alignas(16) glm::vec3 viewDirection = glm::vec3(0);
	alignas(16) glm::vec3 viewRight = glm::vec3(0);
	alignas(16) glm::vec3 viewUp = glm::vec3(0);
	alignas(16) glm::vec4 resolution = glm::vec4(0);
	alignas(16) glm::vec4 ranges = glm::vec4(0);
	alignas(16) glm::vec3 lightDirection;
	alignas(16) glm::vec3 rotatedLightDirection;
	alignas(16) glm::vec3 terrainOffset;
	alignas(8) glm::vec2 terrainLod0Offset;
	alignas(8) glm::vec2 terrainLod1Offset;
	alignas(4) float terrainTotalSize;
	alignas(4) float terrainTotalSizeMult;
	alignas(4) int terrainChunksLength;
	alignas(4) float terrainChunksLengthMult;
	alignas(4) float terrainHeight;
	alignas(8) glm::vec2 waterHeight;
	alignas(4) float windDistanceMult;
	alignas(4) float windStrength;
	alignas(4) float time;
	alignas(4) uint32_t occlusionCulling;
	alignas(4) uint32_t shadows;
	alignas(4) uint32_t shadowBounding;
	alignas(4) uint32_t shadowCascades;
	alignas(16) glm::vec4 terrainShadowOffsets[3];
	alignas(16) glm::vec4 terrainShadowDistances[3];
	alignas(4) uint32_t capturing = 0;
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
		static bool quiting;
		static Settings settings;
		static ShaderVariables shaderVariables;

		static Window &currentWindow;
		static Device &currentDevice;
		static Graphics &currentGraphics;

		static Camera camera;

		static uint32_t currentFrame;
		static uint32_t previousFrame;
		//static VkCommandBuffer currentBuffer;

		static std::vector<Buffer> shaderVariableBuffers;
		static VkDescriptorSetLayout globalDescriptorSetLayout;
		static Descriptor globalDescriptor;

		static std::vector<Object *> objects;

		static Cinematic cinematic;

		static Object screenQuad;
		static Descriptor screenQuadDescriptor;

		static Texture occlusionTexture;

		static std::vector<glm::vec3> cameraIntersects;
		static std::vector<int> cameraIntersectIndexes;

		static glm::vec3 lightAngles; 

		static void Setup();
		static void Create();
		static void Clean();
		static void Quit(int exitCode);

		static void InitializeGLFW();
		static void InitializeVulkan();
		static void CreateShaderVariableBuffers();
		static void CreateDescriptorSetLayout();
		static void CreateDescriptor();
		static void CreateOcclusionTexture();

		static void DestroyPipelines();
		static void DestroyTextures();
		static void DestroyMeshes();
		static void DestroyObjects();
		static void DestroyShaderVariableBuffers();
		static void DestroyDescriptorSetLayout();
		static void DestroyDescriptor();
		static void DestroyOcclusionTexture();

		static void Start();
		static void PreFrame();
		static void Frame();
		static void PostFrame();
		static void UpdateShaderVariables();

		static Window &GetWindow();
		static Graphics &GetGraphics();

		static Pipeline *NewPipeline();
		static Texture *NewTexture();
		static Mesh *NewMesh();
		static Object *NewObject();
};