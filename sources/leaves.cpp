#include "leaves.hpp"

#include "manager.hpp"
#include "shape.hpp"
#include "time.hpp"
#include "trees.hpp"

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
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.foliage = true;
	//pipelineConfiguration.pushConstantCount = 1;
	//pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	//pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = leafMeshLod1.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("leaf", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Leaves::CreateShadowPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.foliage = true;
	pipelineConfiguration.shadow = true;
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = leafMeshLod1.MeshVertexInfo();

	shadowPipeline.CreateGraphicsPipeline("leafShadow", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Leaves::CreateBuffers()
{
	dataBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration dataConfiguration;
	dataConfiguration.size = sizeof(LeafData) * 
			(Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal + 
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 + 
			Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2 + 
			Trees::treeLod3RenderCount * Trees::treeVariables.leafCount3 + 
			Trees::treeLod4RenderCount * Trees::treeVariables.leafCount4);
	dataConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	dataConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	dataConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	dataConfiguration.mapped = false;

	for (Buffer &buffer : dataBuffers)
	{
		buffer.Create(dataConfiguration);
	}
}

void Leaves::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());
	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(LeafData) * 
			(Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal + 
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 + 
			Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2 + 
			Trees::treeLod3RenderCount * Trees::treeVariables.leafCount3 + 
			Trees::treeLod4RenderCount * Trees::treeVariables.leafCount4);
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Leaves::CreateShadowDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());
	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(LeafData) * 
			(Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal + 
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 + 
			Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2 + 
			Trees::treeLod3RenderCount * Trees::treeVariables.leafCount3 + 
			Trees::treeLod4RenderCount * Trees::treeVariables.leafCount4);
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
	for (Buffer &buffer : dataBuffers) buffer.Destroy();
	dataBuffers.clear();
}

void Leaves::Start()
{

}

void Leaves::Frame()
{

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

void Leaves::RenderLeaves(VkCommandBuffer commandBuffer)
{
	leafMeshLod0.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod0.indices.size()), 
		Trees::treeRenderCounts[Manager::currentFrame].lod0Count * Trees::treeVariables.leafCountTotal, 0, 0, 0);

	leafMeshLod1.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		Trees::treeRenderCounts[Manager::currentFrame].lod1Count * Trees::treeVariables.leafCount1, 
		0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		Trees::treeRenderCounts[Manager::currentFrame].lod2Count * Trees::treeVariables.leafCount2, 
		0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
		Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		Trees::treeRenderCounts[Manager::currentFrame].lod3Count * Trees::treeVariables.leafCount3, 
		0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
		Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 +
		Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2);

	leafMeshLod2.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod2.indices.size()), 
		Trees::treeRenderCounts[Manager::currentFrame].lod4Count * Trees::treeVariables.leafCount4, 
		0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
		Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 +
		Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2 +
		Trees::treeLod3RenderCount * Trees::treeVariables.leafCount3);
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

		leafMeshLod0.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod0.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod0Count * Trees::treeVariables.leafCountTotal, 0, 0, 0);
	}
	else if (cascade == 1)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);

		leafMeshLod0.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod0.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod0Count * Trees::treeVariables.leafCountTotal, 0, 0, 0);

		leafMeshLod1.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod1Count * Trees::treeVariables.leafCount1, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod2Count * Trees::treeVariables.leafCount2, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1);
	}
	else if (cascade == 2)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod2), &lod2);

		leafMeshLod1.Bind(commandBuffer);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		//	Trees::treeRenderCounts[Manager::currentFrame].lod0Count * Trees::treeVariables.leafCountTotal, 0, 0, 0);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod1Count * Trees::treeVariables.leafCount1, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod2Count * Trees::treeVariables.leafCount2, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1);

		//leafMeshLod2.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod3Count * Trees::treeVariables.leafCount3, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 +
			Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2);
	}
	else if (cascade == 3)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod3), &lod3);

		leafMeshLod1.Bind(commandBuffer);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		//	Trees::treeRenderCounts[Manager::currentFrame].lod0Count * Trees::treeVariables.leafCountTotal, 0, 0, 0);

		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		//	Trees::treeRenderCounts[Manager::currentFrame].lod1Count * Trees::treeVariables.leafCount1, 
		//	0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod2Count * Trees::treeVariables.leafCount2, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod3Count * Trees::treeVariables.leafCount3, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 +
			Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2);

		leafMeshLod2.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod2.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod4Count * Trees::treeVariables.leafCount4, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 +
			Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2 +
			Trees::treeLod3RenderCount * Trees::treeVariables.leafCount3);
	}
	else if (cascade == 4)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod4), &lod4);

		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		//	Trees::treeRenderCounts[Manager::currentFrame].lod0Count * Trees::treeVariables.leafCountTotal, 0, 0, 0);

		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		//	Trees::treeRenderCounts[Manager::currentFrame].lod1Count * Trees::treeVariables.leafCount1, 
		//	0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal);

		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
		//	Trees::treeRenderCounts[Manager::currentFrame].lod2Count * Trees::treeVariables.leafCount2, 
		//	0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
		//	Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1);

		leafMeshLod1.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod1.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod3Count * Trees::treeVariables.leafCount3, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 +
			Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2);

		leafMeshLod2.Bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(leafMeshLod2.indices.size()), 
			Trees::treeRenderCounts[Manager::currentFrame].lod4Count * Trees::treeVariables.leafCount4, 
			0, 0, Trees::treeLod0RenderCount * Trees::treeVariables.leafCountTotal +
			Trees::treeLod1RenderCount * Trees::treeVariables.leafCount1 +
			Trees::treeLod2RenderCount * Trees::treeVariables.leafCount2 +
			Trees::treeLod3RenderCount * Trees::treeVariables.leafCount3);
	}
}
Mesh Leaves::leafMeshLod0;
Mesh Leaves::leafMeshLod1;
Mesh Leaves::leafMeshLod2;

Pipeline Leaves::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Leaves::shadowPipeline{Manager::currentDevice, Manager::camera};

Descriptor Leaves::graphicsDescriptor{Manager::currentDevice};
Descriptor Leaves::shadowDescriptor{Manager::currentDevice};

std::vector<Buffer> Leaves::dataBuffers;