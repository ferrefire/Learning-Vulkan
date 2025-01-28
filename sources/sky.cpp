#include "sky.hpp"

#include "manager.hpp"

void Sky::Create()
{
	CreateMesh();
	CreatePipeline();
	CreateDescriptor();
}

void Sky::CreateMesh()
{
	skyMesh.coordinate = true;
	skyMesh.shape.coordinate = true;
	skyMesh.shape.SetShape(QUAD);
	skyMesh.shape.positions[0] = glm::vec3(-0.73638, -0.414214, 1.0);
	skyMesh.shape.positions[1] = glm::vec3(0.73638, 0.414214, 1.0);
	skyMesh.shape.positions[2] = glm::vec3(-0.73638, 0.414214, 1.0);
	skyMesh.shape.positions[3] = glm::vec3(0.73638, -0.414214, 1.0);
	skyMesh.RecalculateVertices();
	skyMesh.Create();
}

void Sky::CreatePipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);

	descriptorLayoutConfig[0].type = INPUT_ATTACHMENT;
	descriptorLayoutConfig[0].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[1].type = INPUT_ATTACHMENT;
	descriptorLayoutConfig[1].stages = FRAGMENT_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();

	pipelineConfiguration.depthStencil.depthWriteEnable = VK_FALSE;
	//pipelineConfiguration.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	pipelineConfiguration.depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
	pipelineConfiguration.rasterization.cullMode = VK_CULL_MODE_NONE;
	pipelineConfiguration.subpass = 1;

	VertexInfo vertexInfo = skyMesh.MeshVertexInfo();

	skyPipeline.CreateGraphicsPipeline("sky", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Sky::CreateDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(2);

	descriptorConfig[0].type = INPUT_ATTACHMENT;
	descriptorConfig[0].stages = FRAGMENT_STAGE;
	descriptorConfig[0].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[0].imageInfo.imageView = Manager::currentWindow.colorTexture.imageView;
	//descriptorConfig[0].imageInfo.sampler = grassDiffuseTexture.sampler;

	descriptorConfig[1].type = INPUT_ATTACHMENT;
	descriptorConfig[1].stages = FRAGMENT_STAGE;
	descriptorConfig[1].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[1].imageInfo.imageView = Manager::currentWindow.depthTexture.imageView;
	// descriptorConfig[0].imageInfo.sampler = grassDiffuseTexture.sampler;

	skyDescriptor.Create(descriptorConfig, skyPipeline.objectDescriptorSetLayout);
}

void Sky::Destroy()
{
	DestroyPipeline();
	DestroyMesh();
	DestroyDescriptor();
}

void Sky::DestroyPipeline()
{
	skyPipeline.Destroy();
}

void Sky::DestroyMesh()
{
	skyMesh.Destroy();
}

void Sky::DestroyDescriptor()
{
	skyDescriptor.Destroy();
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
	skyDescriptor.Bind(commandBuffer, skyPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	RenderSky(commandBuffer);
}

void Sky::RenderSky(VkCommandBuffer commandBuffer)
{
	skyMesh.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(skyMesh.indices.size()), 1, 0, 0, 0);
}

Mesh Sky::skyMesh;
Pipeline Sky::skyPipeline{Manager::currentDevice, Manager::camera};
Descriptor Sky::skyDescriptor{Manager::currentDevice};