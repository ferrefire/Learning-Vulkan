#include "sky.hpp"

#include "manager.hpp"

void Sky::Create()
{
	CreateMesh();
	CreatePipeline();
}

void Sky::CreateMesh()
{
	skyMesh.shape.SetShape(QUAD);
	skyMesh.RecalculateVertices();
	skyMesh.Create();
}

void Sky::CreatePipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(0);
	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();

	pipelineConfiguration.depthStencil.depthWriteEnable = VK_FALSE;
	pipelineConfiguration.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	pipelineConfiguration.rasterization.cullMode = VK_CULL_MODE_NONE;

	VertexInfo vertexInfo = skyMesh.MeshVertexInfo();

	skyPipeline.CreateGraphicsPipeline("sky", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Sky::Destroy()
{
	DestroyPipeline();
	DestroyMesh();
}

void Sky::DestroyPipeline()
{
	skyPipeline.Destroy();
}

void Sky::DestroyMesh()
{
	skyMesh.Destroy();
}

void Sky::Start()
{
	
}

void Sky::Frame()
{
	
}

void Sky::RecordCommands(VkCommandBuffer commandBuffer)
{
	skyPipeline.BindGraphics(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, skyPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	RenderSky(commandBuffer);
}

void Sky::RenderSky(VkCommandBuffer commandBuffer)
{
	skyMesh.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(skyMesh.indices.size()), 1, 0, 0, 0);
}

Mesh Sky::skyMesh;
Pipeline Sky::skyPipeline{Manager::currentDevice, Manager::camera};