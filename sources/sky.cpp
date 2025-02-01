#include "sky.hpp"

#include "manager.hpp"
#include "terrain.hpp"

void Sky::Create()
{
	CreateMesh();
	CreatePipelines();
	CreateTextures();
	CreateDescriptors();
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

void Sky::CreatePipelines()
{
	std::vector<DescriptorLayoutConfiguration> skyDescriptorLayoutConfig(3);

	skyDescriptorLayoutConfig[0].type = INPUT_ATTACHMENT;
	skyDescriptorLayoutConfig[0].stages = FRAGMENT_STAGE;
	skyDescriptorLayoutConfig[1].type = INPUT_ATTACHMENT;
	skyDescriptorLayoutConfig[1].stages = FRAGMENT_STAGE;
	skyDescriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	skyDescriptorLayoutConfig[2].stages = FRAGMENT_STAGE;

	PipelineConfiguration skyPipelineConfiguration = Pipeline::DefaultConfiguration();

	skyPipelineConfiguration.depthStencil.depthWriteEnable = VK_FALSE;
	//skyPipelineConfiguration.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	skyPipelineConfiguration.depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
	skyPipelineConfiguration.rasterization.cullMode = VK_CULL_MODE_NONE;
	skyPipelineConfiguration.subpass = 1;

	VertexInfo vertexInfo = skyMesh.MeshVertexInfo();

	skyPipeline.CreateGraphicsPipeline("sky", skyDescriptorLayoutConfig, skyPipelineConfiguration, vertexInfo);

	std::vector<DescriptorLayoutConfiguration> scatterDescriptorLayoutConfig(1);

	scatterDescriptorLayoutConfig[0].type = IMAGE_STORAGE;
	scatterDescriptorLayoutConfig[0].stages = COMPUTE_STAGE;

	scatterComputePipeline.CreateComputePipeline("scatterCompute", scatterDescriptorLayoutConfig);
}

void Sky::CreateTextures()
{
	SamplerConfiguration scatterSampler;
	ImageConfiguration scatterConfig = Texture::ImageStorage(512, 512);
	scatterConfig.format = RGB8;

	scatterTexture.CreateImage(scatterConfig, scatterSampler);
	scatterTexture.TransitionImageLayout(scatterConfig);
}

void Sky::CreateDescriptors()
{
	std::vector<DescriptorConfiguration> skyDescriptorConfig(3);

	skyDescriptorConfig[0].type = INPUT_ATTACHMENT;
	skyDescriptorConfig[0].stages = FRAGMENT_STAGE;
	skyDescriptorConfig[0].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	skyDescriptorConfig[0].imageInfo.imageView = Manager::currentWindow.colorTexture.imageView;

	skyDescriptorConfig[1].type = INPUT_ATTACHMENT;
	skyDescriptorConfig[1].stages = FRAGMENT_STAGE;
	skyDescriptorConfig[1].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	skyDescriptorConfig[1].imageInfo.imageView = Manager::currentWindow.depthTexture.imageView;

	skyDescriptorConfig[2].type = IMAGE_SAMPLER;
	skyDescriptorConfig[2].stages = FRAGMENT_STAGE;
	skyDescriptorConfig[2].imageInfo.imageLayout = LAYOUT_GENERAL;
	skyDescriptorConfig[2].imageInfo.imageView = scatterTexture.imageView;
	skyDescriptorConfig[2].imageInfo.sampler = scatterTexture.sampler;

	skyDescriptor.Create(skyDescriptorConfig, skyPipeline.objectDescriptorSetLayout);

	std::vector<DescriptorConfiguration> scatterDescriptorConfig(1);

	scatterDescriptorConfig[0].type = IMAGE_STORAGE;
	scatterDescriptorConfig[0].stages = COMPUTE_STAGE;
	scatterDescriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	scatterDescriptorConfig[0].imageInfo.imageView = scatterTexture.imageView;

	scatterComputeDescriptor.perFrame = false;
	scatterComputeDescriptor.Create(scatterDescriptorConfig, scatterComputePipeline.objectDescriptorSetLayout);
}

void Sky::Destroy()
{
	DestroyPipelines();
	DestroyMesh();
	DestroyTextures();
	DestroyDescriptors();
}

void Sky::DestroyPipelines()
{
	skyPipeline.Destroy();
	scatterComputePipeline.Destroy();
}

void Sky::DestroyMesh()
{
	skyMesh.Destroy();
}

void Sky::DestroyTextures()
{
	scatterTexture.Destroy();
}

void Sky::DestroyDescriptors()
{
	skyDescriptor.Destroy();
	scatterComputeDescriptor.Destroy();
}

void Sky::Start()
{
	
}

void Sky::Frame()
{
	if (!scatterComputed && Terrain::HeightMapsGenerated())
	{
		scatterComputed = true;
		ComputeInScattering();
	}
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

void Sky::ComputeInScattering()
{
	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	scatterComputePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, scatterComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	scatterComputeDescriptor.Bind(commandBuffer, scatterComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, 64, 64, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

Mesh Sky::skyMesh;

Pipeline Sky::skyPipeline{Manager::currentDevice, Manager::camera};
Pipeline Sky::scatterComputePipeline{Manager::currentDevice, Manager::camera};

Texture Sky::scatterTexture{Manager::currentDevice};

Descriptor Sky::skyDescriptor{Manager::currentDevice};
Descriptor Sky::scatterComputeDescriptor{Manager::currentDevice};

bool Sky::scatterComputed = false;