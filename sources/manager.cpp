#include "manager.hpp"

#include "input.hpp"
#include "terrain.hpp"
#include "grass.hpp"
#include "time.hpp"
#include "shadow.hpp"
#include "culling.hpp"
#include "trees.hpp"
#include "utilities.hpp"
#include "data.hpp"
#include "leaves.hpp"
#include "sky.hpp"

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
	int i = 0;
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(8);
	descriptorLayoutConfig[i].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[i++].stages = ALL_STAGE;
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i++].stages = ALL_STAGE;
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i++].stages = ALL_STAGE;
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i++].stages = ALL_STAGE;
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i++].stages = ALL_STAGE;
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[i++].count = Shadow::cascadeCount;
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i].stages = ALL_STAGE;
	descriptorLayoutConfig[i++].count = TERRAIN_SHADOW_CASCADES;
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i++].stages = ALL_STAGE;
	//descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	//descriptorLayoutConfig[i++].stages = FRAGMENT_STAGE;

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

	int j = 0;
	std::vector<DescriptorConfiguration> descriptorConfig(8);
	descriptorConfig[j].type = UNIFORM_BUFFER;
	descriptorConfig[j].stages = ALL_STAGE;
	descriptorConfig[j].buffersInfo.resize(shaderVariableBuffers.size());
	int i = 0;
	for (Buffer &buffer : shaderVariableBuffers)
	{
		descriptorConfig[j].buffersInfo[i].buffer = buffer.buffer;
		descriptorConfig[j].buffersInfo[i].range = sizeof(ShaderVariables);
		descriptorConfig[j].buffersInfo[i].offset = 0;
		i++;
	}
	j++;

	descriptorConfig[j].type = IMAGE_SAMPLER;
	descriptorConfig[j].stages = ALL_STAGE;
	descriptorConfig[j].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[j].imageInfo.imageView = Terrain::heightMapArrayTexture.imageView;
	descriptorConfig[j++].imageInfo.sampler = Terrain::heightMapArrayTexture.sampler;

	descriptorConfig[j].type = IMAGE_SAMPLER;
	descriptorConfig[j].stages = ALL_STAGE;
	descriptorConfig[j].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[j].imageInfo.imageView = Terrain::heightMapLod0Texture.imageView;
	descriptorConfig[j++].imageInfo.sampler = Terrain::heightMapLod0Texture.sampler;

	descriptorConfig[j].type = IMAGE_SAMPLER;
	descriptorConfig[j].stages = ALL_STAGE;
	descriptorConfig[j].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[j].imageInfo.imageView = Terrain::heightMapLod1Texture.imageView;
	descriptorConfig[j++].imageInfo.sampler = Terrain::heightMapLod1Texture.sampler;

	descriptorConfig[j].type = IMAGE_SAMPLER;
	descriptorConfig[j].stages = ALL_STAGE;
	descriptorConfig[j].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	//descriptorConfig[j].imageInfo.imageView = Culling::cullTexture.imageView;
	//descriptorConfig[j++].imageInfo.sampler = Culling::cullTexture.sampler;
	descriptorConfig[j].imageInfo.imageView = Culling::cullTextures[0].imageView;
	descriptorConfig[j++].imageInfo.sampler = Culling::cullTextures[0].sampler;

	/*descriptorConfig[j].type = IMAGE_SAMPLER;
	descriptorConfig[j].stages = FRAGMENT_STAGE;
	descriptorConfig[j].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[j].imageInfo.imageView = Shadow::shadowLod0Texture.imageView;
	descriptorConfig[j++].imageInfo.sampler = Shadow::shadowLod0Texture.sampler;

	descriptorConfig[j].type = IMAGE_SAMPLER;
	descriptorConfig[j].stages = FRAGMENT_STAGE;
	descriptorConfig[j].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[j].imageInfo.imageView = Shadow::shadowLod1Texture.imageView;
	descriptorConfig[j++].imageInfo.sampler = Shadow::shadowLod1Texture.sampler;*/

	descriptorConfig[j].type = IMAGE_SAMPLER;
	descriptorConfig[j].stages = FRAGMENT_STAGE;
	descriptorConfig[j].count = Shadow::cascadeCount;
	descriptorConfig[j].imageInfos.resize(Shadow::cascadeCount);
	for (int k = 0; k < Shadow::cascadeCount; k++)
	{
		descriptorConfig[j].imageInfos[k].imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[j].imageInfos[k].imageView = Shadow::shadowCascadeTextures[k].imageView;
		descriptorConfig[j].imageInfos[k].sampler = Shadow::shadowCascadeTextures[k].sampler;
	}
	j++;
	//descriptorConfig[j].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	//descriptorConfig[j].imageInfo.imageView = Shadow::shadowCascadeTextures[0].imageView;
	//descriptorConfig[j++].imageInfo.sampler = Shadow::shadowCascadeTextures[0].sampler;

	descriptorConfig[j].type = IMAGE_SAMPLER;
	descriptorConfig[j].stages = ALL_STAGE;
	descriptorConfig[j].count = TERRAIN_SHADOW_CASCADES;
	descriptorConfig[j].imageInfos.resize(TERRAIN_SHADOW_CASCADES);
	for (int k = 0; k < TERRAIN_SHADOW_CASCADES; k++)
	{
		descriptorConfig[j].imageInfos[k].imageLayout = LAYOUT_GENERAL;
		descriptorConfig[j].imageInfos[k].imageView = Terrain::terrainShadowTextures[k].imageView;
		descriptorConfig[j].imageInfos[k].sampler = Terrain::terrainShadowTextures[k].sampler;
	}
	j++;
	//escriptorConfig[j].imageInfo.imageLayout = LAYOUT_GENERAL;
	//escriptorConfig[j].imageInfo.imageView = Terrain::terrainShadowTextures[0].imageView;
	//escriptorConfig[j++].imageInfo.sampler = Terrain::terrainShadowTextures[0].sampler;

	descriptorConfig[j].type = IMAGE_SAMPLER;
	descriptorConfig[j].stages = ALL_STAGE;
	descriptorConfig[j].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[j].imageInfo.imageView = Sky::viewTexture.imageView;
	descriptorConfig[j++].imageInfo.sampler = Sky::viewTexture.sampler;

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
	cameraIntersects.resize(4);
	cameraIntersectIndexes.resize(4);

	Input::Start();
	Terrain::Start();
	if (Manager::settings.trees)
	{
		Trees::Start();
		Leaves::Start();
	}
	Grass::Start();
	Data::Start();

	cinematic.Start();
}

void Manager::PreFrame()
{
	Terrain::Frame();
	Trees::Frame();
	Leaves::Frame();
	Grass::Frame();
	Sky::Frame();
}

void Manager::Frame()
{
	if (!quiting && Input::GetKey(GLFW_KEY_ESCAPE).down)
	{
		quiting = true;
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

	if (Input::GetKey(GLFW_MOUSE_BUTTON_RIGHT, true).pressed) camera.PrintStatus();

	//Terrain::Frame();
	//if (Manager::settings.trees)
	//{
	//	Trees::Frame();
	//	Leaves::Frame();
	//}
	//Grass::Frame();

	bool lightUpdated = true;
	if (Input::GetKey(GLFW_KEY_RIGHT).down) lightAngles.y -= Time::deltaTime * 15.0f;
	else if (Input::GetKey(GLFW_KEY_LEFT).down) lightAngles.y += Time::deltaTime * 15.0f;
	else if (Input::GetKey(GLFW_KEY_DOWN).down) lightAngles.x -= Time::deltaTime * 15.0f;
	else if (Input::GetKey(GLFW_KEY_UP).down) lightAngles.x += Time::deltaTime * 15.0f;
	else lightUpdated = false;

	if (lightUpdated)
	{
		Terrain::updateTerrainShadows = true;
		Sky::shouldUpdateView = true;
		Sky::shouldUpdateAerial = true;
	}

	//if (Input::GetKey(GLFW_KEY_C).pressed)
	//	Terrain::ComputeShadows(0);

	//if (Time::newSecond)
	//{
	//	Utilities::PrintVec(lightAngles);
	//	Utilities::PrintVec(shaderVariables.lightDirection);
	//	std::cout << std::endl;
	//}
}

void Manager::PostFrame()
{
	//Terrain::CheckTerrainOffset();
}

void Manager::UpdateShaderVariables()
{
	//shaderVariables.lightDirection = glm::normalize(glm::vec3(0.25, 0.5, 0.25));
	glm::vec3 lightDirection = glm::vec3(0, 1, 0);
	lightDirection = Utilities::RotateVec(lightDirection, lightAngles.x, glm::vec3(1, 0, 0));
	lightDirection = Utilities::RotateVec(lightDirection, lightAngles.y, glm::vec3(0, 1, 0));
	lightDirection = Utilities::RotateVec(lightDirection, lightAngles.z, glm::vec3(0, 0, 1));
	shaderVariables.lightDirection = glm::normalize(lightDirection);

	glm::vec3 rotatedLightDirection = Utilities::RotateVec(shaderVariables.lightDirection, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	shaderVariables.rotatedLightDirection = glm::normalize(rotatedLightDirection);

	shaderVariables.view = camera.View();
	shaderVariables.projection = camera.Projection();
	shaderVariables.viewMatrix = shaderVariables.projection * shaderVariables.view;

	Shadow::SetCascadeProjections();
	Shadow::SetCascadeViews();
	Shadow::SetCascadeTransformations();
	for (int i = 0; i < Shadow::cascadeCount; i++)
	{
		shaderVariables.shadowCascadeMatrix[i] = Shadow::shadowCascadeTransformations[i] * Shadow::shadowCascadeProjections[i] * Shadow::shadowCascadeViews[i];
	}

	shaderVariables.cullMatrix = Culling::cullProjection * shaderVariables.view;

	shaderVariables.viewHeight = Data::GetGeneralData().viewHeight;
	shaderVariables.viewPosition = camera.Position();
	shaderVariables.viewDirection = camera.Front();
	shaderVariables.viewRight = camera.Side();
	shaderVariables.viewUp = camera.Up();

	shaderVariables.resolution = glm::vec4(window.width, window.height, 1.0 / window.width, 1.0 / window.height);
	shaderVariables.ranges = glm::vec4(camera.near, camera.far, 1.0 / camera.near, 1.0 / camera.far);

	shaderVariables.terrainOffset = Terrain::terrainOffset;
	shaderVariables.terrainLod0Offset = Terrain::terrainLod0Offset;
	shaderVariables.terrainLod1Offset = Terrain::terrainLod1Offset;
	//shaderVariables.terrainShadowOffset = Terrain::terrainShadowOffset;

	shaderVariables.time = Time::GetCurrentTime();

	shaderVariables.occlusionCulling = settings.occlussionCulling ? 1 : 0;
	shaderVariables.shadows = settings.shadows ? 1 : 0;
	//shaderVariables.shadowCascades = Shadow::trapezoidal ? 0 : 1;
	shaderVariables.shadowCascades = Shadow::cascadeCount;
	for (int i = 0; i < TERRAIN_SHADOW_CASCADES; i++)
	{
		shaderVariables.terrainShadowOffsets[i] = glm::vec4(Terrain::terrainShadowOffsets[i].x, Terrain::terrainShadowOffsets[i].y, 0, 0);
		shaderVariables.terrainShadowDistances[i] = glm::vec4(Terrain::shadowComputeVariables[i].distance);
	}
	//shaderVariables.terrainShadowDistance0 = Terrain::shadowComputeVariables[0].distance;
	//shaderVariables.terrainShadowDistance1 = Terrain::shadowComputeVariables[1].distance;

	//std::vector<glm::vec4> frustumCorners = camera.GetFrustumCorners(1.0f, 1000.0f);
	//shaderVariables.frustumCorner1 = frustumCorners[4];
	//shaderVariables.frustumCorner2 = frustumCorners[5];
	//shaderVariables.frustumCorner3 = frustumCorners[6];
	//shaderVariables.frustumCorner4 = frustumCorners[7];

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

bool Manager::quiting = false;

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
uint32_t Manager::previousFrame = 0;
//VkCommandBuffer Manager::currentBuffer = nullptr;

std::vector<Object *> Manager::objects;
Settings Manager::settings;

Cinematic Manager::cinematic;

Object Manager::screenQuad;
Descriptor Manager::screenQuadDescriptor;

Texture Manager::occlusionTexture;

std::vector<glm::vec3> Manager::cameraIntersects;
std::vector<int> Manager::cameraIntersectIndexes;

glm::vec3 Manager::lightAngles = glm::vec3(-35.0f, -135.0f, 0.0f);