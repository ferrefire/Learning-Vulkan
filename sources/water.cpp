#include "water.hpp"
#include "manager.hpp"
#include "sky.hpp"

void Water::Create()
{
    CreateMeshes();
	CreateTextures();
    CreatePipelines();
	CreateDescriptors();
}

void Water::CreateMeshes()
{
    waterMesh.shape.SetShape(PLANE, 100);
    waterMesh.RecalculateVertices();
    waterMesh.Create();
}

void Water::CreateTextures()
{
    SamplerConfiguration normalSamplerConfig;
	normalSamplerConfig.repeatMode = REPEAT;
	normalSamplerConfig.mipLodBias = 0.0f;
	normalSamplerConfig.anisotrophic = VK_TRUE;
	normalSamplerConfig.anisotrophicSampleCount = 8.0;
	normalSamplerConfig.mipLodBias = 0.0f;

    normalTextures.resize(1);
	normalTextures[0].CreateTexture("wave_norm.jpg", normalSamplerConfig);
}

void Water::CreatePipelines()
{
    std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(5);
	descriptorLayoutConfig[0].type = INPUT_ATTACHMENT;
	descriptorLayoutConfig[0].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[1].type = INPUT_ATTACHMENT;
	descriptorLayoutConfig[1].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[2].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[3].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[3].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[4].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[4].stages = FRAGMENT_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.subpass = 1;
	pipelineConfiguration.depthStencil.depthWriteEnable = VK_FALSE;

	VertexInfo vertexInfo = waterMesh.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("water", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Water::CreateDescriptors()
{
    std::vector<DescriptorConfiguration> descriptorConfig(5);

	descriptorConfig[0].type = INPUT_ATTACHMENT;
	descriptorConfig[0].stages = FRAGMENT_STAGE;
	descriptorConfig[0].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[0].imageInfo.imageView = Manager::currentWindow.colorTexture.imageView;

	descriptorConfig[1].type = INPUT_ATTACHMENT;
	descriptorConfig[1].stages = FRAGMENT_STAGE;
	descriptorConfig[1].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[1].imageInfo.imageView = Manager::currentWindow.depthTexture.imageView;

	descriptorConfig[2].type = IMAGE_SAMPLER;
	descriptorConfig[2].stages = FRAGMENT_STAGE;
	descriptorConfig[2].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[2].imageInfo.imageView = Sky::viewTexture.imageView;
	descriptorConfig[2].imageInfo.sampler = Sky::viewTexture.sampler;

	descriptorConfig[3].type = IMAGE_SAMPLER;
	descriptorConfig[3].stages = FRAGMENT_STAGE;
	descriptorConfig[3].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[3].imageInfo.imageView = Sky::aerialTexture.imageView;
	descriptorConfig[3].imageInfo.sampler = Sky::aerialTexture.sampler;

	descriptorConfig[4].type = IMAGE_SAMPLER;
	descriptorConfig[4].stages = FRAGMENT_STAGE;
	descriptorConfig[4].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[4].imageInfo.imageView = normalTextures[0].imageView;
	descriptorConfig[4].imageInfo.sampler = normalTextures[0].sampler;

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Water::Destroy()
{
	DestroyMeshes();
	DestroyTextures();
	DestroyPipelines();
	DestroyDescriptors();
}

void Water::DestroyMeshes()
{
	waterMesh.Destroy();
}

void Water::DestroyTextures()
{
	for (Texture &texture : normalTextures)
	{
		texture.Destroy();
	}
	normalTextures.clear();
}

void Water::DestroyPipelines()
{
	graphicsPipeline.Destroy();
}

void Water::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
}

void Water::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
	graphicsPipeline.BindGraphics(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	RenderWater(commandBuffer);
}

void Water::RenderWater(VkCommandBuffer commandBuffer)
{
	waterMesh.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(waterMesh.indices.size()), 1, 0, 0, 0);
}

Mesh Water::waterMesh{};

std::vector<Texture> Water::normalTextures;

Pipeline Water::graphicsPipeline{Manager::currentDevice, Manager::camera};

Descriptor Water::graphicsDescriptor{Manager::currentDevice};