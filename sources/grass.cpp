#include "grass.hpp"

#include "manager.hpp"
#include "shape.hpp"

void Grass::Create()
{
	CreateMeshes();
	CreateGraphicsPipeline();
	CreateComputePipeline();
	CreateBuffers();
	CreateGraphicsDescriptor();
	CreateComputeDescriptor();
}

void Grass::CreateMeshes()
{
	grassMesh.shape.SetShape(BLADE, 2);
	grassMesh.RecalculateVertices();
	grassMesh.Create();

	grassLodMesh.shape.SetShape(BLADE, 0);
	grassLodMesh.RecalculateVertices();
	grassLodMesh.Create();
}

void Grass::CreateGraphicsPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(3);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;
	descriptorLayoutConfig[1].type = STORAGE_BUFFER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE;
	descriptorLayoutConfig[2].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[2].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.foliage = true;
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t); //change to uint16

	VertexInfo vertexInfo = grassMesh.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("grass", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Grass::CreateComputePipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(5);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[1].type = STORAGE_BUFFER;
	descriptorLayoutConfig[1].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[2].type = STORAGE_BUFFER;
	descriptorLayoutConfig[2].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[3].type = STORAGE_BUFFER;
	descriptorLayoutConfig[3].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[4].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[4].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("grassCompute", descriptorLayoutConfig);
}

void Grass::CreateBuffers()
{
	dataBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration dataConfiguration;
	dataConfiguration.size = sizeof(GrassData) * grassCount;
	dataConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	dataConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	dataConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	dataConfiguration.mapped = false;

	for (Buffer &buffer : dataBuffers)
	{
		buffer.Create(dataConfiguration);
	}

	lodDataBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration lodDataConfiguration;
	lodDataConfiguration.size = sizeof(GrassData) * grassLodCount;
	lodDataConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	lodDataConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	lodDataConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	lodDataConfiguration.mapped = false;

	for (Buffer &buffer : lodDataBuffers)
	{
		buffer.Create(lodDataConfiguration);
	}

	countBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration countConfiguration;
	countConfiguration.size = sizeof(uint32_t);
	countConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	countConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	countConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	countConfiguration.mapped = true;

	for (Buffer &buffer : countBuffers)
	{
		buffer.Create(countConfiguration);
	}

	lodCountBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration lodCountConfiguration;
	lodCountConfiguration.size = sizeof(uint32_t);
	lodCountConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	lodCountConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	lodCountConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	lodCountConfiguration.mapped = true;

	for (Buffer &buffer : lodCountBuffers)
	{
		buffer.Create(lodCountConfiguration);
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
	std::vector<DescriptorConfiguration> descriptorConfig(3);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());
	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(GrassData) * grassCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[1].type = STORAGE_BUFFER;
	descriptorConfig[1].stages = VERTEX_STAGE;
	descriptorConfig[1].buffersInfo.resize(lodDataBuffers.size());
	index = 0;
	for (Buffer &buffer : lodDataBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(GrassData) * grassLodCount;
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[2].type = UNIFORM_BUFFER;
	descriptorConfig[2].stages = VERTEX_STAGE;
	descriptorConfig[2].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[2].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[2].buffersInfo[index].range = sizeof(GrassVariables);
		descriptorConfig[2].buffersInfo[index].offset = 0;
		index++;
	}

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Grass::CreateComputeDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(5);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());
	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(GrassData) * grassCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[1].type = STORAGE_BUFFER;
	descriptorConfig[1].stages = COMPUTE_STAGE;
	descriptorConfig[1].buffersInfo.resize(lodDataBuffers.size());
	index = 0;
	for (Buffer &buffer : lodDataBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(GrassData) * grassLodCount;
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[2].type = STORAGE_BUFFER;
	descriptorConfig[2].stages = COMPUTE_STAGE;
	descriptorConfig[2].buffersInfo.resize(countBuffers.size());
	index = 0;
	for (Buffer &buffer : countBuffers)
	{
		descriptorConfig[2].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[2].buffersInfo[index].range = sizeof(uint32_t);
		descriptorConfig[2].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[3].type = STORAGE_BUFFER;
	descriptorConfig[3].stages = COMPUTE_STAGE;
	descriptorConfig[3].buffersInfo.resize(lodCountBuffers.size());
	index = 0;
	for (Buffer &buffer : lodCountBuffers)
	{
		descriptorConfig[3].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[3].buffersInfo[index].range = sizeof(uint32_t);
		descriptorConfig[3].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[4].type = UNIFORM_BUFFER;
	descriptorConfig[4].stages = COMPUTE_STAGE;
	descriptorConfig[4].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[4].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[4].buffersInfo[index].range = sizeof(GrassVariables);
		descriptorConfig[4].buffersInfo[index].offset = 0;
		index++;
	}

	computeDescriptor.Create(descriptorConfig, computePipeline.objectDescriptorSetLayout);
}

void Grass::Destroy()
{
	DestroyPipelines();
	DestroyMeshes();
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
	computePipeline.Destroy();
}

void Grass::DestroyBuffers()
{
	for (Buffer &buffer : dataBuffers) buffer.Destroy();
	dataBuffers.clear();

	for (Buffer &buffer : lodDataBuffers) buffer.Destroy();
	lodDataBuffers.clear();

	for (Buffer &buffer : countBuffers) buffer.Destroy();
	countBuffers.clear();

	for (Buffer &buffer : lodCountBuffers) buffer.Destroy();
	lodCountBuffers.clear();

	for (Buffer &buffer : variableBuffers) buffer.Destroy();
	variableBuffers.clear();
}

void Grass::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	computeDescriptor.Destroy();
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

	memcpy(variableBuffers[Manager::currentFrame].mappedBuffer, &grassVariables, sizeof(grassVariables));
}

void Grass::Frame()
{
	//ComputeGrass();
}

void Grass::PostFrame()
{
	ComputeGrass();
}

void Grass::RecordCommands(VkCommandBuffer commandBuffer)
{
	graphicsPipeline.BindGraphics(commandBuffer, Manager::currentWindow);

	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	//Manager::UpdateShaderVariables();
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	memcpy(variableBuffers[Manager::currentFrame].mappedBuffer, &grassVariables, sizeof(grassVariables));

	RenderGrass(commandBuffer);
}

void Grass::RenderGrass(VkCommandBuffer commandBuffer)
{
	uint32_t lod0 = 0;
	uint32_t lod1 = 1;
	int renderCount = *(int *)countBuffers[Manager::currentFrame].mappedBuffer;
	int lodRenderCount = *(int *)lodCountBuffers[Manager::currentFrame].mappedBuffer;

	grassMesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassMesh.indices.size()), renderCount, 0, 0, 0);

	grassLodMesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassLodMesh.indices.size()), lodRenderCount, 0, 0, 0);
}

void Grass::ComputeGrass()
{
	//vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	uint32_t computeCount = ceil((float)grassTotalBase / 8.0);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	memcpy(variableBuffers[Manager::currentFrame].mappedBuffer, &grassVariables, sizeof(grassVariables));
	
	vkCmdDispatch(commandBuffer, computeCount, computeCount, 1);

	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

uint32_t Grass::grassBase = 384;
uint32_t Grass::grassCount = Grass::grassBase * Grass::grassBase;

uint32_t Grass::grassLodBase = 4096;
uint32_t Grass::grassLodCount = Grass::grassLodBase * Grass::grassLodBase;

uint32_t Grass::grassTotalBase = Grass::grassBase + Grass::grassLodBase;
uint32_t Grass::grassTotalCount = Grass::grassTotalBase * Grass::grassTotalBase;

Mesh Grass::grassMesh;
Mesh Grass::grassLodMesh;

Pipeline Grass::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Grass::computePipeline{Manager::currentDevice, Manager::camera};

Descriptor Grass::graphicsDescriptor{Manager::currentDevice};
Descriptor Grass::computeDescriptor{Manager::currentDevice};

std::vector<Buffer> Grass::dataBuffers;
std::vector<Buffer> Grass::lodDataBuffers;
std::vector<Buffer> Grass::countBuffers;
std::vector<Buffer> Grass::lodCountBuffers;
std::vector<Buffer> Grass::variableBuffers;

GrassVariables Grass::grassVariables;