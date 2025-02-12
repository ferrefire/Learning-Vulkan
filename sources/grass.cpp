#include "grass.hpp"

#include "manager.hpp"
#include "shape.hpp"
#include "time.hpp"
#include "data.hpp"

#include <iostream>

void Grass::Create()
{
	grassCount = grassBase * grassBase;
	grassLodBase = grassBase + grassLodBase;
	grassLodCount = grassLodBase * grassLodBase - grassCount;
	grassTotalBase = grassLodBase;
	grassTotalCount = grassCount + grassLodCount;

	CreateMeshes();
	CreateGraphicsPipeline();
	if (Manager::settings.shadows) CreateShadowPipeline();
	CreateComputePipelines();
	CreateTextures();
	CreateBuffers();
	CreateGraphicsDescriptor();
	if (Manager::settings.shadows) CreateShadowDescriptor();
	CreateComputeDescriptors();
}

void Grass::CreateTextures()
{
	ImageConfiguration clumpingConfig = Texture::ImageStorage(1024, 1024);
	clumpingConfig.format = VK_FORMAT_R16G16B16A16_SFLOAT;

	SamplerConfiguration clumpingSamplerConfig;
	clumpingSamplerConfig.repeatMode = MIRRORED_REPEAT;
	//clumpingSamplerConfig.minFilter = VK_FILTER_NEAREST;
	//clumpingSamplerConfig.magFilter = VK_FILTER_NEAREST;

	clumpingTexture.CreateImage(clumpingConfig, clumpingSamplerConfig);
	clumpingTexture.TransitionImageLayout(clumpingConfig);
}

void Grass::CreateMeshes()
{
	grassMesh.coordinate = true;
	grassMesh.shape.coordinate = true;
	grassMesh.shape.SetShape(BLADE, 3);
	grassMesh.RecalculateVertices();
	grassMesh.Create();

	grassLodMesh.coordinate = true;
	grassLodMesh.shape.coordinate = true;
	grassLodMesh.shape.SetShape(BLADE, 1);
	grassLodMesh.RecalculateVertices();
	grassLodMesh.Create();
}

void Grass::CreateGraphicsPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;
	descriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.foliage = true;
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = grassMesh.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("grass", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Grass::CreateShadowPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;
	//descriptorLayoutConfig[1].type = STORAGE_BUFFER;
	//descriptorLayoutConfig[1].stages = VERTEX_STAGE;
	descriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.foliage = true;
	//pipelineConfiguration.cullFront = true;
	pipelineConfiguration.shadow = true;
	pipelineConfiguration.pushConstantCount = 2;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = grassMesh.MeshVertexInfo();

	shadowPipeline.CreateGraphicsPipeline("grassShadow", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Grass::CreateComputePipelines()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(4);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	//descriptorLayoutConfig[1].type = STORAGE_BUFFER;
	//descriptorLayoutConfig[1].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[1].type = STORAGE_BUFFER;
	descriptorLayoutConfig[1].stages = COMPUTE_STAGE;
	//descriptorLayoutConfig[2].type = STORAGE_BUFFER;
	//descriptorLayoutConfig[2].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[2].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[2].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[3].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[3].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("grassCompute", descriptorLayoutConfig);

	std::vector<DescriptorLayoutConfiguration> clumpingDescriptorLayoutConfig(1);
	clumpingDescriptorLayoutConfig[0].type = IMAGE_STORAGE;
	clumpingDescriptorLayoutConfig[0].stages = COMPUTE_STAGE;

	clumpingComputePipeline.CreateComputePipeline("clumpingCompute", clumpingDescriptorLayoutConfig);
}

void Grass::CreateBuffers()
{
	dataBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration dataConfiguration;
	dataConfiguration.size = sizeof(GrassData) * grassTotalCount;
	dataConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	dataConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	dataConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	dataConfiguration.mapped = false;

	for (Buffer &buffer : dataBuffers)
	{
		buffer.Create(dataConfiguration);
	}

	countBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration countConfiguration;
	countConfiguration.size = sizeof(CountData);
	countConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	countConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	countConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	countConfiguration.mapped = true;

	for (Buffer &buffer : countBuffers)
	{
		buffer.Create(countConfiguration);
	}

	variableBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration variableConfiguration;
	variableConfiguration.size = sizeof(GrassVariables);
	variableConfiguration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	variableConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	variableConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	variableConfiguration.mapped = true;

	for (Buffer &buffer : variableBuffers)
	{
		buffer.Create(variableConfiguration);
	}
}

void Grass::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(2);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());
	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(GrassData) * grassTotalCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[1].type = UNIFORM_BUFFER;
	descriptorConfig[1].stages = VERTEX_STAGE;
	descriptorConfig[1].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(GrassVariables);
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Grass::CreateShadowDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(2);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());
	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		//descriptorConfig[0].buffersInfo[index].range = sizeof(GrassData) * grassCount;
		descriptorConfig[0].buffersInfo[index].range = sizeof(GrassData) * grassTotalCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[1].type = UNIFORM_BUFFER;
	descriptorConfig[1].stages = VERTEX_STAGE;
	descriptorConfig[1].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(GrassVariables);
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	shadowDescriptor.Create(descriptorConfig, shadowPipeline.objectDescriptorSetLayout);
}

void Grass::CreateComputeDescriptors()
{
	std::vector<DescriptorConfiguration> descriptorConfig(4);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());
	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		//descriptorConfig[0].buffersInfo[index].range = sizeof(GrassData) * grassCount;
		descriptorConfig[0].buffersInfo[index].range = sizeof(GrassData) * grassTotalCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	//descriptorConfig[1].type = STORAGE_BUFFER;
	//descriptorConfig[1].stages = COMPUTE_STAGE;
	//descriptorConfig[1].buffersInfo.resize(lodDataBuffers.size());
	//index = 0;
	//for (Buffer &buffer : lodDataBuffers)
	//{
	//	descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
	//	descriptorConfig[1].buffersInfo[index].range = sizeof(GrassData) * grassLodCount;
	//	descriptorConfig[1].buffersInfo[index].offset = 0;
	//	index++;
	//}

	descriptorConfig[1].type = STORAGE_BUFFER;
	descriptorConfig[1].stages = COMPUTE_STAGE;
	descriptorConfig[1].buffersInfo.resize(countBuffers.size());
	index = 0;
	for (Buffer &buffer : countBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(CountData);
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	//descriptorConfig[2].type = STORAGE_BUFFER;
	//descriptorConfig[2].stages = COMPUTE_STAGE;
	//descriptorConfig[2].buffersInfo.resize(lodCountBuffers.size());
	//index = 0;
	//for (Buffer &buffer : lodCountBuffers)
	//{
	//	descriptorConfig[2].buffersInfo[index].buffer = buffer.buffer;
	//	descriptorConfig[2].buffersInfo[index].range = sizeof(uint32_t);
	//	descriptorConfig[2].buffersInfo[index].offset = 0;
	//	index++;
	//}

	descriptorConfig[2].type = UNIFORM_BUFFER;
	descriptorConfig[2].stages = COMPUTE_STAGE;
	descriptorConfig[2].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[2].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[2].buffersInfo[index].range = sizeof(GrassVariables);
		descriptorConfig[2].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[3].type = IMAGE_SAMPLER;
	descriptorConfig[3].stages = COMPUTE_STAGE;
	descriptorConfig[3].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[3].imageInfo.imageView = clumpingTexture.imageView;
	descriptorConfig[3].imageInfo.sampler = clumpingTexture.sampler;

	computeDescriptor.Create(descriptorConfig, computePipeline.objectDescriptorSetLayout);

	std::vector<DescriptorConfiguration> clumpingDescriptorConfig(1);

	clumpingDescriptorConfig[0].type = IMAGE_STORAGE;
	clumpingDescriptorConfig[0].stages = COMPUTE_STAGE;
	clumpingDescriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	clumpingDescriptorConfig[0].imageInfo.imageView = clumpingTexture.imageView;

	clumpingComputeDescriptor.perFrame = false;
	clumpingComputeDescriptor.Create(clumpingDescriptorConfig, clumpingComputePipeline.objectDescriptorSetLayout);
}

void Grass::Destroy()
{
	DestroyPipelines();
	DestroyMeshes();
	DestroyTextures();
	DestroyDescriptors();
	DestroyBuffers();
}

void Grass::DestroyMeshes()
{
	grassMesh.Destroy();
	grassLodMesh.Destroy();
}

void Grass::DestroyPipelines()
{
	graphicsPipeline.Destroy();
	shadowPipeline.Destroy();
	computePipeline.Destroy();
	clumpingComputePipeline.Destroy();
}

void Grass::DestroyTextures()
{
	clumpingTexture.Destroy();
}

void Grass::DestroyBuffers()
{
	for (Buffer &buffer : dataBuffers) buffer.Destroy();
	dataBuffers.clear();

	for (Buffer &buffer : countBuffers) buffer.Destroy();
	countBuffers.clear();

	for (Buffer &buffer : variableBuffers) buffer.Destroy();
	variableBuffers.clear();
}

void Grass::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	shadowDescriptor.Destroy();
	computeDescriptor.Destroy();
	clumpingComputeDescriptor.Destroy();
}

void Grass::Start()
{
	grassVariables.grassBase = grassBase;
	grassVariables.grassBaseMult = 1.0 / grassBase;
	grassVariables.grassCount = grassCount;

	grassVariables.grassLodBase = grassLodBase;
	grassVariables.grassLodBaseMult = 1.0 / grassLodBase;
	grassVariables.grassLodCount = grassLodCount;

	grassVariables.grassTotalBase = grassTotalBase;
	grassVariables.grassTotalBaseMult = 1.0 / grassTotalBase;
	grassVariables.grassTotalCount = grassTotalCount;

	grassRenderCounts.resize(Manager::settings.maxFramesInFlight);
	//grassLodRenderCounts.resize(Manager::settings.maxFramesInFlight);

	for (Buffer &buffer : variableBuffers)
	{
		memcpy(buffer.mappedBuffer, &grassVariables, sizeof(grassVariables));
	}

	ComputeClumping();
}

void Grass::Frame()
{
	//ComputeGrass();
}

void Grass::PostFrame()
{
	ComputeGrass(nullptr);

	//if (Time::newFrameTick) ComputeGrass();
}

void Grass::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
	graphicsPipeline.BindGraphics(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	RenderGrass(commandBuffer);
}

void Grass::RecordComputeCommands(VkCommandBuffer commandBuffer)
{
	ComputeGrass(commandBuffer);
}

void Grass::RecordShadowCommands(VkCommandBuffer commandBuffer, int cascade)
{
	if (cascade > 1) return;

	shadowPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	shadowDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderShadows(commandBuffer, cascade);
}

void Grass::RenderGrass(VkCommandBuffer commandBuffer)
{
	uint32_t lod0 = 0;
	uint32_t lod1 = 1;

	grassMesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassMesh.indices.size()), grassRenderCounts[Manager::currentFrame].renderCount, 0, 0, 0);

	grassLodMesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassLodMesh.indices.size()), grassRenderCounts[Manager::currentFrame].lodRenderCount, 0, 0, 0);
}

void Grass::RenderShadows(VkCommandBuffer commandBuffer, int cascade)
{
	uint32_t lod0 = 0;
	uint32_t lod1 = 1;
	uint32_t lod2 = 2;

	if (cascade == 0)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, sizeof(uint32_t), sizeof(lod0), &lod0);

		grassMesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassMesh.indices.size()), grassRenderCounts[Manager::currentFrame].renderCount, 0, 0, 0);

		//grassLodMesh.Bind(commandBuffer);
		//vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassLodMesh.indices.size()), grassRenderCounts[Manager::currentFrame].lodRenderCount, 0, 0, 0);
	}
	else if (cascade == 1)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, sizeof(uint32_t), sizeof(lod1), &lod1);

		//grassMesh.Bind(commandBuffer);
		//vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassMesh.indices.size()), grassRenderCounts[Manager::currentFrame].renderCount, 0, 0, 0);

		grassLodMesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassLodMesh.indices.size()), grassRenderCounts[Manager::currentFrame].lodRenderCount, 0, 0, 0);
	}
	else if (cascade == 2)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, sizeof(uint32_t), sizeof(lod2), &lod2);

		//grassMesh.Bind(commandBuffer);
		//vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassMesh.indices.size()), grassRenderCounts[Manager::currentFrame].renderCount, 0, 0, 0);

		grassLodMesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassLodMesh.indices.size()), grassRenderCounts[Manager::currentFrame].lodRenderCount, 0, 0, 0);
	}
}

void Grass::ComputeGrass(VkCommandBuffer commandBuffer)
{
	//vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	uint32_t computeCount = ceil((float)grassTotalBase / 8.0);

	//VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();
	bool oneTimeBuffer = commandBuffer == nullptr;
	if (oneTimeBuffer) commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	memcpy(variableBuffers[Manager::currentFrame].mappedBuffer, &grassVariables, sizeof(grassVariables));
	
	vkCmdDispatch(commandBuffer, computeCount, computeCount, 1);

	if (oneTimeBuffer)
	{
		Manager::currentDevice.EndComputeCommand(commandBuffer);
		grassRenderCounts[Manager::currentFrame] = *(CountData *)countBuffers[Manager::currentFrame].mappedBuffer;
	}
}

void Grass::SetData()
{
	grassRenderCounts[Manager::currentFrame] = *(CountData *)countBuffers[Manager::currentFrame].mappedBuffer;
}

void Grass::ComputeClumping()
{
	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	clumpingComputePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, clumpingComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	clumpingComputeDescriptor.Bind(commandBuffer, clumpingComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, 128, 128, 1);

	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

uint32_t Grass::grassBase = 256 + 128;
uint32_t Grass::grassCount = Grass::grassBase * Grass::grassBase;

uint32_t Grass::grassLodBase = 2048;
uint32_t Grass::grassLodCount = Grass::grassLodBase * Grass::grassLodBase - Grass::grassCount;

uint32_t Grass::grassTotalBase = Grass::grassLodBase;
uint32_t Grass::grassTotalCount = Grass::grassTotalBase * Grass::grassTotalBase;

std::vector<CountData> Grass::grassRenderCounts;

Mesh Grass::grassMesh;
Mesh Grass::grassLodMesh;

Pipeline Grass::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Grass::shadowPipeline{Manager::currentDevice, Manager::camera};
Pipeline Grass::computePipeline{Manager::currentDevice, Manager::camera};
Pipeline Grass::clumpingComputePipeline{Manager::currentDevice, Manager::camera};

Descriptor Grass::graphicsDescriptor{Manager::currentDevice};
Descriptor Grass::shadowDescriptor{Manager::currentDevice};
Descriptor Grass::computeDescriptor{Manager::currentDevice};
Descriptor Grass::clumpingComputeDescriptor{Manager::currentDevice};

std::vector<Buffer> Grass::dataBuffers;
std::vector<Buffer> Grass::countBuffers;
std::vector<Buffer> Grass::variableBuffers;

Texture Grass::clumpingTexture;

GrassVariables Grass::grassVariables;