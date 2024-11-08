#include "trees.hpp"

#include "manager.hpp"

#include <iostream>

void Trees::Create()
{
	return;

	CreateMeshes();
	//std::cout << "meshes" << std::endl;
	CreateGraphicsPipeline();
	//CreateShadowPipeline();
	//CreateCullPipeline();
	CreateComputeSetupPipeline();
	CreateComputeRenderPipeline();
	//std::cout << "pipelines" << std::endl;
	CreateBuffers();
	//std::cout << "buffers" << std::endl;
	CreateGraphicsDescriptor();
	//CreateShadowDescriptor();
	//CreateCullDescriptor();
	CreateComputeSetupDescriptor();
	CreateComputeRenderDescriptor();
	//std::cout << "descriptors" << std::endl;
}

void Trees::CreateMeshes()
{
	treeMesh.coordinate = false;
	treeMesh.shape.positionsOnly = true;
	treeMesh.shape.SetShape(CUBE, 0);
	treeMesh.RecalculateVertices();
	treeMesh.Create();
}

void Trees::CreateGraphicsPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.foliage = true;

	VertexInfo vertexInfo = treeMesh.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("tree", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Trees::CreateShadowPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.shadow = true;

	VertexInfo vertexInfo = treeMesh.MeshVertexInfo();

	shadowPipeline.CreateGraphicsPipeline("treeShadow", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Trees::CreateCullPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.cull = true;

	VertexInfo vertexInfo = treeMesh.MeshVertexInfo();

	shadowPipeline.CreateGraphicsPipeline("treeCull", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Trees::CreateComputeSetupPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;

	computeSetupPipeline.CreateComputePipeline("treeSetupCompute", descriptorLayoutConfig);
}

void Trees::CreateComputeRenderPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(3);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[1].type = STORAGE_BUFFER;
	descriptorLayoutConfig[1].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[2].type = STORAGE_BUFFER;
	descriptorLayoutConfig[2].stages = COMPUTE_STAGE;

	computeRenderPipeline.CreateComputePipeline("treeRenderCompute", descriptorLayoutConfig);
}

void Trees::CreateBuffers()
{
	BufferConfiguration dataConfiguration;
	dataConfiguration.size = sizeof(TreeData) * treeCount;
	dataConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	dataConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	dataConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	dataConfiguration.mapped = false;

	dataBuffer.Create(dataConfiguration);

	renderBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration renderConfiguration;
	renderConfiguration.size = sizeof(TreeRenderData) * treeRenderCount;
	renderConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	renderConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	renderConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	renderConfiguration.mapped = false;

	for (Buffer &buffer : renderBuffers)
	{
		buffer.Create(renderConfiguration);
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
}

void Trees::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(renderBuffers.size());
	int index = 0;
	for (Buffer &buffer : renderBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(TreeRenderData) * treeRenderCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Trees::CreateShadowDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(renderBuffers.size());
	int index = 0;
	for (Buffer &buffer : renderBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(TreeRenderData) * treeRenderCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	shadowDescriptor.Create(descriptorConfig, shadowPipeline.objectDescriptorSetLayout);
}

void Trees::CreateCullDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(renderBuffers.size());
	int index = 0;
	for (Buffer &buffer : renderBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(TreeRenderData) * treeRenderCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	cullDescriptor.Create(descriptorConfig, cullPipeline.objectDescriptorSetLayout);
}

void Trees::CreateComputeSetupDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].buffersInfo.resize(1);
	descriptorConfig[0].buffersInfo[0].buffer = dataBuffer.buffer;
	descriptorConfig[0].buffersInfo[0].range = sizeof(TreeData) * treeCount;
	descriptorConfig[0].buffersInfo[0].offset = 0;

	computeSetupDescriptor.perFrame = false;
	computeSetupDescriptor.Create(descriptorConfig, computeSetupPipeline.objectDescriptorSetLayout);
}

void Trees::CreateComputeRenderDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(3);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].buffersInfo.resize(1);
	descriptorConfig[0].buffersInfo[0].buffer = dataBuffer.buffer;
	descriptorConfig[0].buffersInfo[0].range = sizeof(TreeData) * treeCount;
	descriptorConfig[0].buffersInfo[0].offset = 0;

	descriptorConfig[1].type = STORAGE_BUFFER;
	descriptorConfig[1].stages = COMPUTE_STAGE;
	descriptorConfig[1].buffersInfo.resize(renderBuffers.size());
	int index = 0;
	for (Buffer &buffer : renderBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(TreeRenderData) * treeRenderCount;
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

	computeRenderDescriptor.Create(descriptorConfig, computeRenderPipeline.objectDescriptorSetLayout);
}

void Trees::Destroy()
{
	return;

	DestroyPipelines();
	DestroyMeshes();
	DestroyDescriptors();
	DestroyBuffers();
}

void Trees::DestroyMeshes()
{
	treeMesh.Destroy();
}

void Trees::DestroyPipelines()
{
	graphicsPipeline.Destroy();
	//shadowPipeline.Destroy();
	//cullPipeline.Destroy();
	computeSetupPipeline.Destroy();
	computeRenderPipeline.Destroy();
}

void Trees::DestroyBuffers()
{
	dataBuffer.Destroy();

	for (Buffer &buffer : renderBuffers)
	{
		buffer.Destroy();
	}
	renderBuffers.clear();

	for (Buffer &buffer : countBuffers)
	{
		buffer.Destroy();
	}
	countBuffers.clear();
}

void Trees::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	//shadowDescriptor.Destroy();
	//cullDescriptor.Destroy();
	computeSetupDescriptor.Destroy();
	computeRenderDescriptor.Destroy();
}

void Trees::Start()
{
	return;

	treeRenderCounts.resize(Manager::settings.maxFramesInFlight);

	ComputeTreeSetup();
}

void Trees::Frame()
{

}

void Trees::PostFrame()
{
	return;

	ComputeTreeRender();
}

void Trees::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
	return;

	graphicsPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderTrees(commandBuffer);
}

void Trees::RecordShadowCommands(VkCommandBuffer commandBuffer)
{
	shadowPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	shadowDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderShadows(commandBuffer);
}

void Trees::RecordCullCommands(VkCommandBuffer commandBuffer)
{
	cullPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, cullPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	cullDescriptor.Bind(commandBuffer, cullPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderCulling(commandBuffer);
}

void Trees::RenderTrees(VkCommandBuffer commandBuffer)
{
	treeMesh.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeMesh.indices.size()), treeRenderCounts[Manager::currentFrame], 0, 0, 0);
}

void Trees::RenderShadows(VkCommandBuffer commandBuffer)
{
	treeMesh.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeMesh.indices.size()), treeRenderCounts[Manager::currentFrame], 0, 0, 0);
}

void Trees::RenderCulling(VkCommandBuffer commandBuffer)
{
	treeMesh.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeMesh.indices.size()), treeRenderCounts[Manager::currentFrame], 0, 0, 0);
}

void Trees::ComputeTreeSetup()
{
	uint32_t computeCount = ceil(float(treeBase) / 8.0f);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computeSetupPipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computeSetupPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeSetupDescriptor.Bind(commandBuffer, computeSetupPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, computeCount, computeCount, 1);

	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

void Trees::ComputeTreeRender()
{
	//std::cout << "start" << std::endl;

	uint32_t computeCount = ceil(float(treeRenderBase) / 8.0f);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	//std::cout << "begin" << std::endl;

	computeRenderPipeline.BindCompute(commandBuffer);
	//std::cout << "pipe" << std::endl;
	Manager::globalDescriptor.Bind(commandBuffer, computeRenderPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeRenderDescriptor.Bind(commandBuffer, computeRenderPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	//std::cout << "des" << std::endl;

	vkCmdDispatch(commandBuffer, computeCount, computeCount, 1);
	//std::cout << "dis" << std::endl;

	Manager::currentDevice.EndComputeCommand(commandBuffer);
	//std::cout << "end" << std::endl;

	treeRenderCounts[Manager::currentFrame] = *(uint32_t *)countBuffers[Manager::currentFrame].mappedBuffer;
	//std::cout << "count" << std::endl;
}

uint32_t Trees::treeBase = 512;
uint32_t Trees::treeCount = Trees::treeBase * Trees::treeBase;

uint32_t Trees::treeRenderBase = 128;
uint32_t Trees::treeRenderCount = Trees::treeRenderBase * Trees::treeRenderBase;
std::vector<uint32_t> Trees::treeRenderCounts;

Mesh Trees::treeMesh;

Pipeline Trees::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::shadowPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::cullPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::computeSetupPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::computeRenderPipeline{Manager::currentDevice, Manager::camera};

Descriptor Trees::graphicsDescriptor{Manager::currentDevice};
Descriptor Trees::shadowDescriptor{Manager::currentDevice};
Descriptor Trees::cullDescriptor{Manager::currentDevice};
Descriptor Trees::computeSetupDescriptor{Manager::currentDevice};
Descriptor Trees::computeRenderDescriptor{Manager::currentDevice};

Buffer Trees::dataBuffer;
std::vector<Buffer> Trees::renderBuffers;
std::vector<Buffer> Trees::countBuffers;