#include "manager.hpp"

#include "input.hpp"
#include "terrain.hpp"
#include "grass.hpp"
#include "time.hpp"
#include "shadow.hpp"
#include "culling.hpp"
#include "trees.hpp"
#include "utilities.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

void Manager::Setup()
{
	try
	{
		InitializeGLFW();
		InitializeVulkan();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		Quit(EXIT_FAILURE);
	}
}

void Manager::Create()
{
	//CreateOcclusionTexture();
	CreateShaderVariableBuffers();
	CreateDescriptorSetLayout();
	CreateDescriptor();
}

void Manager::Clean()
{
	DestroyPipelines();
	DestroyTextures();
	//DestroyOcclusionTexture();
	DestroyMeshes();
	DestroyObjects();
	DestroyShaderVariableBuffers();
	DestroyDescriptor();
	DestroyDescriptorSetLayout();
}

void Manager::Quit(int exitCode)
{
	//Clean();

	graphics.Destroy();

	window.Destroy();
	glfwTerminate();

	exit(exitCode);
}

void Manager::InitializeGLFW()
{
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize glfw library");
    }

    glfwInitialized = true;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window.Create();
}

void Manager::InitializeVulkan()
{
	graphics.Create();

	vulkanInitialized = true;

	std::cout << device.properties.deviceName << std::endl;
}

void Manager::CreateShaderVariableBuffers()
{
	if (shaderVariableBuffers.size() != 0) throw std::runtime_error("cannot create shader variable buffers because they already exist");

	shaderVariableBuffers.resize(settings.maxFramesInFlight);

	BufferConfiguration configuration;
	configuration.size = sizeof(ShaderVariables);
	configuration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	configuration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	configuration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	configuration.mapped = true;

	for (Buffer &buffer : shaderVariableBuffers)
	{
		buffer.Create(configuration);
	}
}

void Manager::CreateDescriptorSetLayout()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(7);
	descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[0].stages = ALL_STAGE;
	descriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[1].stages = ALL_STAGE;
	descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[2].stages = ALL_STAGE;
	descriptorLayoutConfig[3].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[3].stages = ALL_STAGE;
	descriptorLayoutConfig[4].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[4].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[5].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[5].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[6].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[6].stages = ALL_STAGE;

	Pipeline::CreateDescriptorSetLayout(descriptorLayoutConfig, &globalDescriptorSetLayout);
}

void Manager::CreateDescriptor()
{
	//std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(4);
	//descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	//descriptorLayoutConfig[0].stages = ALL_STAGE;
	//descriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	//descriptorLayoutConfig[1].stages = ALL_STAGE;
	//descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	//descriptorLayoutConfig[2].stages = ALL_STAGE;
	//descriptorLayoutConfig[3].type = IMAGE_SAMPLER;
	//descriptorLayoutConfig[3].stages = ALL_STAGE;
	//Pipeline::CreateDescriptorSetLayout(descriptorLayoutConfig, &globalDescriptorSetLayout);

	std::vector<DescriptorConfiguration> descriptorConfig(7);

	descriptorConfig[0].type = UNIFORM_BUFFER;
	descriptorConfig[0].stages = ALL_STAGE;
	descriptorConfig[0].buffersInfo.resize(shaderVariableBuffers.size());
	int i = 0;
	for (Buffer &buffer : shaderVariableBuffers)
	{
		descriptorConfig[0].buffersInfo[i].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[i].range = sizeof(ShaderVariables);
		descriptorConfig[0].buffersInfo[i].offset = 0;
		i++;
	}

	descriptorConfig[1].type = IMAGE_SAMPLER;
	descriptorConfig[1].stages = ALL_STAGE;
	descriptorConfig[1].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[1].imageInfo.imageView = Terrain::heightMapArrayTexture.imageView;
	descriptorConfig[1].imageInfo.sampler = Terrain::heightMapArrayTexture.sampler;

	descriptorConfig[2].type = IMAGE_SAMPLER;
	descriptorConfig[2].stages = ALL_STAGE;
	descriptorConfig[2].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[2].imageInfo.imageView = Terrain::heightMapLod0Texture.imageView;
	descriptorConfig[2].imageInfo.sampler = Terrain::heightMapLod0Texture.sampler;

	descriptorConfig[3].type = IMAGE_SAMPLER;
	descriptorConfig[3].stages = ALL_STAGE;
	descriptorConfig[3].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[3].imageInfo.imageView = Terrain::heightMapLod1Texture.imageView;
	descriptorConfig[3].imageInfo.sampler = Terrain::heightMapLod1Texture.sampler;

	descriptorConfig[4].type = IMAGE_SAMPLER;
	descriptorConfig[4].stages = FRAGMENT_STAGE;
	descriptorConfig[4].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[4].imageInfo.imageView = Shadow::shadowLod0Texture.imageView;
	descriptorConfig[4].imageInfo.sampler = Shadow::shadowLod0Texture.sampler;

	descriptorConfig[5].type = IMAGE_SAMPLER;
	descriptorConfig[5].stages = FRAGMENT_STAGE;
	descriptorConfig[5].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[5].imageInfo.imageView = Shadow::shadowLod1Texture.imageView;
	descriptorConfig[5].imageInfo.sampler = Shadow::shadowLod1Texture.sampler;

	descriptorConfig[6].type = IMAGE_SAMPLER;
	descriptorConfig[6].stages = ALL_STAGE;
	descriptorConfig[6].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[6].imageInfo.imageView = Culling::cullTexture.imageView;
	descriptorConfig[6].imageInfo.sampler = Culling::cullTexture.sampler;

	globalDescriptor.Create(descriptorConfig, globalDescriptorSetLayout);
}

void Manager::CreateOcclusionTexture()
{
	ImageConfiguration occlusionConfig;
	occlusionConfig.width = window.width;
	occlusionConfig.height = window.height;
	occlusionConfig.format = device.FindDepthFormat();
	occlusionConfig.transitionLayout = LAYOUT_GENERAL;
	occlusionConfig.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	occlusionConfig.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;

	SamplerConfiguration occlusionSamplerConfig;
	occlusionSamplerConfig.anisotrophic = VK_FALSE;

	occlusionTexture.CreateImage(occlusionConfig, occlusionSamplerConfig);
	occlusionTexture.TransitionImageLayout(occlusionConfig);
}

void Manager::Start()
{
	Input::Start();
	Terrain::Start();
	if (Manager::settings.trees) Trees::Start();
	Grass::Start();

	cinematic.Start();
}

void Manager::PreFrame()
{
	//Terrain::CheckTerrainOffset();
}

void Manager::Frame()
{
	if (Input::GetKey(GLFW_KEY_ESCAPE).pressed)
	{
		if (cinematic.creating) cinematic.Create(cinematic.name.c_str());
		window.Close();
	}

	if (cinematic.running && Terrain::HeightMapsGenerated())
	{
		cinematic.Play();
	}

	if (cinematic.creating && Input::GetKey(GLFW_MOUSE_BUTTON_LEFT, true).pressed)
	{
		cinematic.AddKey(camera.Position(), camera.Angles());
	}

	if (Input::GetKey(GLFW_MOUSE_BUTTON_RIGHT, true).pressed)
	{
		//glm::vec3 camDir = camera.Front();
		//glm::vec2 camDir2 = glm::vec2(camDir.x, camDir.z);
		//camDir2 = glm::normalize(camDir2);
		//std::cout << "x: " << camDir2.x << " z: " << camDir2.y << std::endl;

		//Utilities::PrintVec(camera.Position() + glm::vec3(Terrain::terrainOffset.x, 0, Terrain::terrainOffset.y));
		//Utilities::PrintVec("angles", camera.Angles());
		camera.PrintStatus();
	}

	//if (settings.fullscreen && Time::newSecond)
	//{
	//	std::cout << "FPS: " << Time::currentFPS << std::endl;
	//}

	Terrain::Frame();
	if (Manager::settings.trees) Trees::Frame();
	Grass::Frame();
}

void Manager::PostFrame()
{
	//Terrain::CheckTerrainOffset();
}

void Manager::UpdateShaderVariables()
{
	shaderVariables.lightDirection = glm::normalize(glm::vec3(0.25, 0.5, 0.25));
	//shaderVariables.lightDirection = glm::normalize(glm::vec3(1, 1, 1));

	shaderVariables.view = camera.View();
	shaderVariables.projection = camera.Projection();
	shaderVariables.viewMatrix = shaderVariables.projection * shaderVariables.view;
	shaderVariables.frustumMatrix = glm::inverse(camera.GetTempProjection(50.0f, 250.0f) * shaderVariables.view);
	//shaderVariables.shadowLod0View = Shadow::GetShadowView(0);
	//shaderVariables.shadowLod0Projection = Shadow::shadowLod0Projection;
	//shaderVariables.shadowLod0Matrix = shaderVariables.shadowLod0Projection * shaderVariables.shadowLod0View;
	shaderVariables.shadowLod0Matrix = Shadow::GetShadowProjection(0) * Shadow::GetShadowView(0);
	//shaderVariables.shadowLod1View = Shadow::GetShadowView(1);
	//shaderVariables.shadowLod1Projection = Shadow::GetShadowProjection(1);
	shaderVariables.shadowLod1Matrix = Shadow::GetShadowTransformation(1) * Shadow::shadowLod1Projection * Shadow::shadowLod1View;
	//shaderVariables.shadowLod1Projection = Shadow::shadowLod1Projection;
	//shaderVariables.shadowLod1Matrix = Shadow::shadowLod1Projection * Shadow::shadowLod1View;
	//shaderVariables.shadowLod1Matrix = Shadow::GetShadowProjection(1) * Shadow::GetShadowView(1);
	shaderVariables.cullMatrix = Culling::cullProjection * shaderVariables.view;

	//shaderVariables.shadowLod0View = Shadow::GetShadowView(0);
	//shaderVariables.shadowLod1View = Shadow::GetShadowView(1);
	//shaderVariables.shadowLod0Projection = Shadow::shadowLod0Projection;
	//shaderVariables.shadowLod1Projection = Shadow::shadowLod1Projection;
	//shaderVariables.cullProjection = Culling::cullProjection;

	shaderVariables.viewPosition = camera.Position();
	shaderVariables.viewDirection = camera.Front();
	shaderVariables.viewRight = camera.Side();
	shaderVariables.viewUp = camera.Up();

	shaderVariables.resolution = glm::vec4(window.width, window.height, 1.0 / window.width, 1.0 / window.height);

	shaderVariables.terrainOffset = Terrain::terrainOffset;
	shaderVariables.terrainLod0Offset = Terrain::terrainLod0Offset;
	shaderVariables.terrainLod1Offset = Terrain::terrainLod1Offset;

	shaderVariables.time = Time::GetCurrentTime();

	shaderVariables.occlusionCulling = settings.occlussionCulling ? 1 : 0;
	shaderVariables.shadows = settings.shadows ? 1 : 0;

	memcpy(shaderVariableBuffers[currentFrame].mappedBuffer, &shaderVariables, sizeof(shaderVariables));
}

void Manager::DestroyPipelines()
{
	for (Pipeline *pipeline : pipelines)
	{
        if (!pipeline) continue;

		pipeline->Destroy();
		delete(pipeline);
	}
    pipelines.clear();
}

void Manager::DestroyTextures()
{
	for (Texture *texture : textures)
	{
		if (!texture) continue;

		texture->Destroy();
		delete (texture);
	}
	textures.clear();
}

void Manager::DestroyMeshes()
{
	for (Mesh *mesh : meshes)
	{
        if (!mesh) continue;

        mesh->Destroy();
		delete(mesh);
	}
	meshes.clear();
}

void Manager::DestroyObjects()
{
	for (Object *object : objects)
	{
		if (!object) continue;

		object->DestroyUniformBuffers();
		delete(object);
	}
	objects.clear();
}

void Manager::DestroyShaderVariableBuffers()
{
	for (Buffer &buffer : shaderVariableBuffers)
	{
		buffer.Destroy();
	}

	shaderVariableBuffers.clear();
}

void Manager::DestroyDescriptor()
{
	globalDescriptor.Destroy();

	//if (globalDescriptorSetLayout)
	//{
	//	vkDestroyDescriptorSetLayout(device.logicalDevice, globalDescriptorSetLayout, nullptr);
	//	globalDescriptorSetLayout = nullptr;
	//}
}

void Manager::DestroyDescriptorSetLayout()
{
	if (globalDescriptorSetLayout)
	{
		vkDestroyDescriptorSetLayout(device.logicalDevice, globalDescriptorSetLayout, nullptr);
		globalDescriptorSetLayout = nullptr;
	}
}

void Manager::DestroyOcclusionTexture()
{
	occlusionTexture.Destroy();
}

Window &Manager::GetWindow()
{
	return (window);
}

Graphics &Manager::GetGraphics()
{
	return (graphics);
}

Pipeline *Manager::NewPipeline()
{
	Pipeline *pipeline = new Pipeline(device, camera);
	pipelines.push_back(pipeline);

	return (pipeline);
}

Texture *Manager::NewTexture()
{
	Texture *texture = new Texture();
	textures.push_back(texture);

	return (texture);
}

Mesh *Manager::NewMesh()
{
	Mesh *mesh = new Mesh();
	meshes.push_back(mesh);

	return (mesh);
}

Object *Manager::NewObject()
{
	Object *object = new Object();
	objects.push_back(object);
	
	return (object);
}

bool Manager::glfwInitialized = false;
bool Manager::vulkanInitialized = false;

Device Manager::device;
Window Manager::window{device};
Camera Manager::camera{window};
std::vector<Texture *> Manager::textures;
std::vector<Mesh *> Manager::meshes;
std::vector<Pipeline *> Manager::pipelines;
Graphics Manager::graphics{device, window};

Device &Manager::currentDevice = Manager::device;
Window &Manager::currentWindow = Manager::window;
//Camera &Manager::currentCamera = Manager::camera;
Graphics &Manager::currentGraphics = Manager::graphics;

ShaderVariables Manager::shaderVariables;
std::vector<Buffer> Manager::shaderVariableBuffers;
VkDescriptorSetLayout Manager::globalDescriptorSetLayout = nullptr;
Descriptor Manager::globalDescriptor;

uint32_t Manager::currentFrame = 0;
//VkCommandBuffer Manager::currentBuffer = nullptr;

std::vector<Object *> Manager::objects;
Settings Manager::settings;

Cinematic Manager::cinematic;

Object Manager::screenQuad;
Descriptor Manager::screenQuadDescriptor;

Texture Manager::occlusionTexture;