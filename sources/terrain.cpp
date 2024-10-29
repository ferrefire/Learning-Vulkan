#include "terrain.hpp"

#include "manager.hpp"

#include <stdexcept>
#include <iostream>

void Terrain::Create()
{
    CreateMeshes();
	CreateGraphicsPipeline();
	CreateComputePipeline();
	CreateTextures();
	CreateObjects();
	CreateBuffers();
	CreateGraphicsDescriptor();
	CreateComputeDescriptor();
}

void Terrain::CreateTextures()
{
	SamplerConfiguration grassSamplerConfig;
	grassSamplerConfig.repeatMode = REPEAT;
	grassSamplerConfig.mipLodBias = 1.0f;
	grassTexture.CreateTexture("rocky_grass_diff.jpg", grassSamplerConfig);

	SamplerConfiguration heightMapSamplerConfig;

	ImageConfiguration heightMapConfig = Texture::ImageStorage(1024, 1024);
	heightMapTexture.CreateImage(heightMapConfig, heightMapSamplerConfig);
	heightMapTexture.TransitionImageLayout(heightMapConfig);

	ImageConfiguration heightMapLod1Config = Texture::ImageStorage(1024, 1024);
	heightMapLod1Texture.CreateImage(heightMapLod1Config, heightMapSamplerConfig);
	heightMapLod1Texture.TransitionImageLayout(heightMapLod1Config);

	ImageConfiguration heightMapLod0Config = Texture::ImageStorage(1024, 1024);
	heightMapLod0Texture.CreateImage(heightMapLod0Config, heightMapSamplerConfig);
	heightMapLod0Texture.TransitionImageLayout(heightMapLod0Config);
}

void Terrain::CreateMeshes()
{
    //mesh.coordinate = true;
    mesh.shape.SetShape(PLANE, 100);
    mesh.RecalculateVertices();

	mesh.Create();
}

void Terrain::CreateObjects()
{
	object.Create();

	object.Resize(glm::vec3(10000));
}

void Terrain::CreateGraphicsPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(5);
	descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE;
	descriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[2].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[3].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[3].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[4].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[4].stages = FRAGMENT_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.tesselation = true;

    VertexInfo vertexInfo = mesh.MeshVertexInfo();

    graphicsPipeline.CreateGraphicsPipeline("terrain", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Terrain::CreateComputePipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);
	descriptorLayoutConfig[0].type = IMAGE_STORAGE;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[1].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("heightMapCompute", descriptorLayoutConfig);
}

void Terrain::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(5);

	descriptorConfig[0].type = UNIFORM_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE;
	descriptorConfig[0].buffersInfo.resize(object.uniformBuffers.size());
	int i = 0;
	for (Buffer &buffer : object.uniformBuffers)
	{
		descriptorConfig[0].buffersInfo[i].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[i].range = sizeof(UniformBufferObject);
		descriptorConfig[0].buffersInfo[i].offset = 0;
		i++;
	}

	descriptorConfig[1].type = IMAGE_SAMPLER;
	descriptorConfig[1].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[1].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[1].imageInfo.imageView = heightMapTexture.imageView;
	descriptorConfig[1].imageInfo.sampler = heightMapTexture.sampler;

	descriptorConfig[2].type = IMAGE_SAMPLER;
	descriptorConfig[2].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[2].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[2].imageInfo.imageView = heightMapLod1Texture.imageView;
	descriptorConfig[2].imageInfo.sampler = heightMapLod1Texture.sampler;

	descriptorConfig[3].type = IMAGE_SAMPLER;
	descriptorConfig[3].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[3].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[3].imageInfo.imageView = heightMapLod0Texture.imageView;
	descriptorConfig[3].imageInfo.sampler = heightMapLod0Texture.sampler;

	descriptorConfig[4].type = IMAGE_SAMPLER;
	descriptorConfig[4].stages = FRAGMENT_STAGE;
	descriptorConfig[4].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[4].imageInfo.imageView = grassTexture.imageView;
	descriptorConfig[4].imageInfo.sampler = grassTexture.sampler;

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Terrain::CreateComputeDescriptor()
{
	computeDescriptor.perFrame = false;

	std::vector<DescriptorConfiguration> descriptorConfig(2);

	descriptorConfig[0].type = IMAGE_STORAGE;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[0].imageInfo.imageView = heightMapTexture.imageView;
	descriptorConfig[0].imageInfo.sampler = heightMapTexture.sampler;
	descriptorConfig[1].type = UNIFORM_BUFFER;
	descriptorConfig[1].stages = COMPUTE_STAGE;
	descriptorConfig[1].buffersInfo.resize(1);
	descriptorConfig[1].buffersInfo[0].buffer = heightMapVariablesBuffer.buffer;
	descriptorConfig[1].buffersInfo[0].range = sizeof(HeightMapVariables);
	descriptorConfig[1].buffersInfo[0].offset = 0;

	computeDescriptor.perFrame = false;
	computeDescriptor.Create(descriptorConfig, computePipeline.objectDescriptorSetLayout);
}

void Terrain::CreateBuffers()
{
	BufferConfiguration configuration;
	configuration.size = sizeof(HeightMapVariables);
	configuration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	configuration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	configuration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	configuration.mapped = true;

	heightMapVariablesBuffer.Create(configuration);
}

void Terrain::Destroy()
{
	DestroyPipelines();
    DestroyMeshes();
    DestroyTextures();
	DestroyObjects();
	DestroyDescriptors();
	DestroyBuffers();
}

void Terrain::DestroyTextures()
{
	grassTexture.Destroy();
	heightMapTexture.Destroy();
	heightMapLod1Texture.Destroy();
	heightMapLod0Texture.Destroy();
}

void Terrain::DestroyMeshes()
{
	mesh.Destroy();
}

void Terrain::DestroyObjects()
{
	object.Destroy();
}

void Terrain::DestroyPipelines()
{
	graphicsPipeline.Destroy();
	computePipeline.Destroy();
}

void Terrain::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	computeDescriptor.Destroy();
}

void Terrain::DestroyBuffers()
{
	heightMapVariablesBuffer.Destroy();
}

void Terrain::Start()
{
	ComputeHeightMap();
}

void Terrain::RecordCommands(VkCommandBuffer commandBuffer)
{
    graphicsPipeline.BindGraphics(commandBuffer, Manager::currentWindow);
	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	Manager::UpdateShaderVariables();
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	object.UpdateUniformBuffer(Manager::currentFrame);
    mesh.Bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
}

void Terrain::ComputeHeightMap()
{
	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();
	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	Manager::UpdateShaderVariables();
	//computeDescriptor.descriptorConfigs[0].imageInfo.imageView = heightMapLod1Texture.imageView;
	//computeDescriptor.descriptorConfigs[0].imageInfo.sampler = heightMapLod1Texture.sampler;
	//computeDescriptor.Update();
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	heightMapVariables.mapScale = 1.0;
	memcpy(heightMapVariablesBuffer.mappedBuffer, &heightMapVariables, sizeof(heightMapVariables));
	vkCmdDispatch(commandBuffer, 1024, 1024, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);

	commandBuffer = Manager::currentDevice.BeginComputeCommand();
	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	Manager::UpdateShaderVariables();
	computeDescriptor.descriptorConfigs[0].imageInfo.imageView = heightMapLod1Texture.imageView;
	computeDescriptor.descriptorConfigs[0].imageInfo.sampler = heightMapLod1Texture.sampler;
	computeDescriptor.Update();
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	heightMapVariables.mapScale = 0.5;
	memcpy(heightMapVariablesBuffer.mappedBuffer, &heightMapVariables, sizeof(heightMapVariables));
	vkCmdDispatch(commandBuffer, 1024, 1024, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);

	commandBuffer = Manager::currentDevice.BeginComputeCommand();
	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	Manager::UpdateShaderVariables();
	computeDescriptor.descriptorConfigs[0].imageInfo.imageView = heightMapLod0Texture.imageView;
	computeDescriptor.descriptorConfigs[0].imageInfo.sampler = heightMapLod0Texture.sampler;
	computeDescriptor.Update();
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	heightMapVariables.mapScale = 0.25;
	memcpy(heightMapVariablesBuffer.mappedBuffer, &heightMapVariables, sizeof(heightMapVariables));
	vkCmdDispatch(commandBuffer, 1024, 1024, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

Pipeline Terrain::graphicsPipeline{Manager::currentDevice, Manager::currentCamera};
Pipeline Terrain::computePipeline{Manager::currentDevice, Manager::currentCamera};
Texture Terrain::grassTexture{Manager::currentDevice};
Texture Terrain::heightMapTexture{Manager::currentDevice};
Texture Terrain::heightMapLod1Texture{Manager::currentDevice};
Texture Terrain::heightMapLod0Texture{Manager::currentDevice};
HeightMapVariables Terrain::heightMapVariables;
Buffer Terrain::heightMapVariablesBuffer;
Descriptor Terrain::graphicsDescriptor{Manager::currentDevice};
Descriptor Terrain::computeDescriptor{Manager::currentDevice};
Mesh Terrain::mesh{};
Object Terrain::object{&Terrain::mesh, &Terrain::graphicsPipeline};