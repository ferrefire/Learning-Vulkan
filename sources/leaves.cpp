#include "leaves.hpp"

#include "manager.hpp"
#include "shape.hpp"
#include "time.hpp"
#include "trees.hpp"
#include "capture.hpp"

#include <iostream>

void Leaves::Create()
{
	CreateMeshes();
	CreateGraphicsPipeline();
	if (Manager::settings.shadows) CreateShadowPipeline();
	//CreateBuffers();
	CreateGraphicsDescriptor();
	if (Manager::settings.shadows) CreateShadowDescriptor();
}

void Leaves::CreateMeshes()
{
	leafMeshLod0.shape.SetShape(LEAF, 2);
	leafMeshLod0.RecalculateVertices();
	leafMeshLod0.Create();

	leafMeshLod1.shape.SetShape(LEAF, 1);
	leafMeshLod1.RecalculateVertices();
	leafMeshLod1.Create();

	leafMeshLod2.shape.SetShape(LEAF, 0);
	leafMeshLod2.RecalculateVertices();
	leafMeshLod2.Create();
}

void Leaves::CreateGraphicsPipeline()
{
	int i = 0;

	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = VERTEX_STAGE;
	descriptorLayoutConfig[i].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[i++].stages = VERTEX_STAGE | FRAGMENT_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.foliage = true;
	//pipelineConfiguration.pushConstantCount = 1;
	//pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	//pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = leafMeshLod1.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("leaf", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);

	//pipelineConfiguration.capture = true;
	//capturePipeline.CreateGraphicsPipeline("leaf", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Leaves::CreateShadowPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.foliage = true;
	pipelineConfiguration.shadow = true;
	pipelineConfiguration.shadowFragment = true;
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = leafMeshLod1.MeshVertexInfo();

	shadowPipeline.CreateGraphicsPipeline("leafShadow", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Leaves::CreateBuffers()
{
	renderBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration renderConfiguration;
	renderConfiguration.size = sizeof(LeafData) * Trees::totalLeafCount;
	renderConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	renderConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	renderConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	renderConfiguration.mapped = false;

	for (Buffer &buffer : renderBuffers)
	{
		buffer.Create(renderConfiguration);
	}

	shadowBuffers.resize(Manager::settings.maxFramesInFlight);

	for (Buffer &buffer : shadowBuffers)
	{
		buffer.Create(renderConfiguration);
	}

	BufferConfiguration variablesConfiguration;
	variablesConfiguration.size = sizeof(LeafVariables);
	variablesConfiguration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	variablesConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	variablesConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	variablesConfiguration.mapped = true;

	variablesBuffer.Create(variablesConfiguration);
}

void Leaves::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(2);

	int i = 0;

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = VERTEX_STAGE;
	descriptorConfig[i].buffersInfo.resize(renderBuffers.size());
	int index = 0;
	for (Buffer &buffer : renderBuffers)
	{
		descriptorConfig[i].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[i].buffersInfo[index].range = sizeof(LeafData) * Trees::totalLeafCount;
		descriptorConfig[i].buffersInfo[index].offset = 0;
		index++;
	}
	i++;

	descriptorConfig[i].type = UNIFORM_BUFFER;
	descriptorConfig[i].stages = VERTEX_STAGE | FRAGMENT_STAGE;
	descriptorConfig[i].buffersInfo.resize(1);
	descriptorConfig[i].buffersInfo[0].buffer = variablesBuffer.buffer;
	descriptorConfig[i++].buffersInfo[0].range = sizeof(LeafVariables);

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Leaves::CreateShadowDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(shadowBuffers.size());
	int index = 0;
	for (Buffer &buffer : shadowBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(LeafData) * Trees::totalLeafCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	shadowDescriptor.Create(descriptorConfig, shadowPipeline.objectDescriptorSetLayout);
}

void Leaves::Destroy()
{
	DestroyPipelines();
	DestroyMeshes();
	DestroyDescriptors();
	DestroyBuffers();
}

void Leaves::DestroyPipelines()
{
	graphicsPipeline.Destroy();
	shadowPipeline.Destroy();
	//capturePipeline.Destroy();
}

void Leaves::DestroyMeshes()
{
	leafMeshLod0.Destroy();
	leafMeshLod1.Destroy();
	leafMeshLod2.Destroy();
}

void Leaves::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	shadowDescriptor.Destroy();
}

void Leaves::DestroyBuffers()
{
	for (Buffer &buffer : renderBuffers) buffer.Destroy();
	renderBuffers.clear();

	for (Buffer &buffer : shadowBuffers) buffer.Destroy();
	shadowBuffers.clear();

	variablesBuffer.Destroy();
}

void Leaves::Start()
{
	//memcpy(variablesBuffer.mappedBuffer, &leafVariables, sizeof(LeafVariables));
}

void Leaves::Frame()
{
	memcpy(variablesBuffer.mappedBuffer, &leafVariables, sizeof(LeafVariables));
}

void Leaves::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
	graphicsPipeline.BindGraphics(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	RenderLeaves(commandBuffer);
}

void Leaves::RecordShadowCommands(VkCommandBuffer commandBuffer, int cascade)
{
	if (cascade > 4) return;

	shadowPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	shadowDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderShadows(commandBuffer, cascade);
}

void Leaves::RecordCaptureCommands(VkCommandBuffer commandBuffer)
{
	capturePipeline.BindGraphics(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, capturePipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, capturePipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	RenderLeaves(commandBuffer);
}

void Leaves::RenderLeaves(VkCommandBuffer commandBuffer)
{
	leafMeshLod1.Bind(commandBuffer);
	

	//if (Capture::capturing)
	//{
	//	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
	//		1 * Trees::treeVariables.leafCounts[0].x, 0, 0, 0);
	//	return;
	//}

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		Trees::treeRenderCounts[Manager::currentFrame].lod0RenderCount * Trees::treeVariables.leafCounts[0].x, 0, 0, 0);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		Trees::treeRenderCounts[Manager::currentFrame].lod1RenderCount * Trees::treeVariables.leafCounts[1].x, 
		0, 0, Trees::treeVariables.leafCounts[1].y);

	//leafMeshLod1.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
		Trees::treeRenderCounts[Manager::currentFrame].lod2RenderCount * Trees::treeVariables.leafCounts[2].x,
		0, 0, Trees::treeVariables.leafCounts[2].y);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
		Trees::treeRenderCounts[Manager::currentFrame].lod3RenderCount * Trees::treeVariables.leafCounts[3].x,
		0, 0, Trees::treeVariables.leafCounts[3].y);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
		Trees::treeRenderCounts[Manager::currentFrame].lod4RenderCount * Trees::treeVariables.leafCounts[4].x,
		0, 0, Trees::treeVariables.leafCounts[4].y);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
		Trees::treeRenderCounts[Manager::currentFrame].lod5RenderCount * Trees::treeVariables.leafCounts[5].x,
		0, 0, Trees::treeVariables.leafCounts[5].y);
}

void Leaves::RenderShadows(VkCommandBuffer commandBuffer, int cascade)
{
	uint32_t lod0 = 0;
	uint32_t lod1 = 1;
	uint32_t lod2 = 2;
	uint32_t lod3 = 3;
	uint32_t lod4 = 4;

	if (cascade == 0)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);

		leafMeshLod1.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod0ShadowCount * Trees::treeVariables.leafCounts[0].x, 0, 0, 0);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod1ShadowCount * Trees::treeVariables.leafCounts[1].x, 
			0, 0, Trees::treeVariables.leafCounts[1].y);

		//leafMeshLod1.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
			Trees::treeRenderCounts[Manager::currentFrame].lod2ShadowCount * Trees::treeVariables.leafCounts[2].x,
			0, 0, Trees::treeVariables.leafCounts[2].y);
	}
	else if (cascade == 1)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);

		leafMeshLod1.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod0ShadowCount * Trees::treeVariables.leafCounts[0].x, 0, 0, 0);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod1ShadowCount * Trees::treeVariables.leafCounts[1].x, 
			0, 0, Trees::treeVariables.leafCounts[1].y);

		//leafMeshLod1.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
			Trees::treeRenderCounts[Manager::currentFrame].lod2ShadowCount * Trees::treeVariables.leafCounts[2].x,
			0, 0, Trees::treeVariables.leafCounts[2].y);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
			Trees::treeRenderCounts[Manager::currentFrame].lod3ShadowCount * Trees::treeVariables.leafCounts[3].x,
			0, 0, Trees::treeVariables.leafCounts[3].y);
	}
	else if (cascade == 2)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod2), &lod2);

		leafMeshLod1.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod1ShadowCount * Trees::treeVariables.leafCounts[1].x, 
			0, 0, Trees::treeVariables.leafCounts[1].y);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
			Trees::treeRenderCounts[Manager::currentFrame].lod2ShadowCount * Trees::treeVariables.leafCounts[2].x,
			0, 0, Trees::treeVariables.leafCounts[2].y);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
			Trees::treeRenderCounts[Manager::currentFrame].lod3ShadowCount * Trees::treeVariables.leafCounts[3].x,
			0, 0, Trees::treeVariables.leafCounts[3].y);
	}
	else if (cascade == 3)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod3), &lod3);

		leafMeshLod1.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod2ShadowCount * Trees::treeVariables.leafCounts[2].x, 
			0, 0, Trees::treeVariables.leafCounts[2].y);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
			Trees::treeRenderCounts[Manager::currentFrame].lod3ShadowCount * Trees::treeVariables.leafCounts[3].x,
			0, 0, Trees::treeVariables.leafCounts[3].y);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
			Trees::treeRenderCounts[Manager::currentFrame].lod4ShadowCount * Trees::treeVariables.leafCounts[4].x,
			0, 0, Trees::treeVariables.leafCounts[4].y);
	}
	else if (cascade == 4)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod4), &lod4);

		leafMeshLod1.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod3ShadowCount * Trees::treeVariables.leafCounts[3].x, 
			0, 0, Trees::treeVariables.leafCounts[3].y);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()),
			Trees::treeRenderCounts[Manager::currentFrame].lod4ShadowCount * Trees::treeVariables.leafCounts[4].x,
			0, 0, Trees::treeVariables.leafCounts[4].y);
	}
}
Mesh Leaves::leafMeshLod0;
Mesh Leaves::leafMeshLod1;
Mesh Leaves::leafMeshLod2;

Pipeline Leaves::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Leaves::shadowPipeline{Manager::currentDevice, Manager::camera};
Pipeline Leaves::capturePipeline{Manager::currentDevice, Manager::camera};

Descriptor Leaves::graphicsDescriptor{Manager::currentDevice};
Descriptor Leaves::shadowDescriptor{Manager::currentDevice};

std::vector<Buffer> Leaves::renderBuffers;
std::vector<Buffer> Leaves::shadowBuffers;
Buffer Leaves::variablesBuffer;

LeafVariables Leaves::leafVariables;