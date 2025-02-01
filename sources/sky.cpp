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

	std::vector<DescriptorLayoutConfiguration> transmittanceDescriptorLayoutConfig(1);

	transmittanceDescriptorLayoutConfig[0].type = IMAGE_STORAGE;
	transmittanceDescriptorLayoutConfig[0].stages = COMPUTE_STAGE;

	transmittancePipeline.CreateComputePipeline("transmittanceCompute", transmittanceDescriptorLayoutConfig);

	std::vector<DescriptorLayoutConfiguration> scatterDescriptorLayoutConfig(2);

	scatterDescriptorLayoutConfig[0].type = IMAGE_STORAGE;
	scatterDescriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	scatterDescriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	scatterDescriptorLayoutConfig[1].stages = COMPUTE_STAGE;

	scatterPipeline.CreateComputePipeline("scatterCompute", scatterDescriptorLayoutConfig);
}

void Sky::CreateTextures()
{
	SamplerConfiguration transmittanceSampler;
	ImageConfiguration transmittanceConfig = Texture::ImageStorage(256, 64);
	transmittanceConfig.format = RGB8;

	transmittanceTexture.CreateImage(transmittanceConfig, transmittanceSampler);
	transmittanceTexture.TransitionImageLayout(transmittanceConfig);

	SamplerConfiguration scatterSampler;
	ImageConfiguration scatterConfig = Texture::ImageStorage(32, 32);
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

	std::vector<DescriptorConfiguration> transmittanceDescriptorConfig(1);

	transmittanceDescriptorConfig[0].type = IMAGE_STORAGE;
	transmittanceDescriptorConfig[0].stages = COMPUTE_STAGE;
	transmittanceDescriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	transmittanceDescriptorConfig[0].imageInfo.imageView = transmittanceTexture.imageView;

	transmittanceDescriptor.perFrame = false;
	transmittanceDescriptor.Create(transmittanceDescriptorConfig, transmittancePipeline.objectDescriptorSetLayout);

	std::vector<DescriptorConfiguration> scatterDescriptorConfig(2);

	scatterDescriptorConfig[0].type = IMAGE_STORAGE;
	scatterDescriptorConfig[0].stages = COMPUTE_STAGE;
	scatterDescriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	scatterDescriptorConfig[0].imageInfo.imageView = scatterTexture.imageView;
	scatterDescriptorConfig[1].type = IMAGE_SAMPLER;
	scatterDescriptorConfig[1].stages = COMPUTE_STAGE;
	scatterDescriptorConfig[1].imageInfo.imageLayout = LAYOUT_GENERAL;
	scatterDescriptorConfig[1].imageInfo.imageView = transmittanceTexture.imageView;
	scatterDescriptorConfig[1].imageInfo.sampler = transmittanceTexture.sampler;

	scatterDescriptor.perFrame = false;
	scatterDescriptor.Create(scatterDescriptorConfig, scatterPipeline.objectDescriptorSetLayout);
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
	transmittancePipeline.Destroy();
	scatterPipeline.Destroy();
}

void Sky::DestroyMesh()
{
	skyMesh.Destroy();
}

void Sky::DestroyTextures()
{
	transmittanceTexture.Destroy();
	scatterTexture.Destroy();
}

void Sky::DestroyDescriptors()
{
	skyDescriptor.Destroy();
	transmittanceDescriptor.Destroy();
	scatterDescriptor.Destroy();
}

void Sky::Start()
{
	
}

void Sky::Frame()
{
	if (!transmittanceComputed && Terrain::HeightMapsGenerated())
	{
		transmittanceComputed = true;
		ComputeTransmittance();
	}

	//if (!scatterComputed && transmittanceComputed && Terrain::HeightMapsGenerated())
	//{
	//	scatterComputed = true;
	//	ComputeScattering();
	//}
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

void Sky::ComputeTransmittance()
{
	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	transmittancePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, transmittancePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	transmittanceDescriptor.Bind(commandBuffer, transmittancePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, 32, 8, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

void Sky::ComputeScattering()
{
	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	scatterPipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, scatterPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	scatterDescriptor.Bind(commandBuffer, scatterPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, 32, 32, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

Mesh Sky::skyMesh;

Pipeline Sky::skyPipeline{Manager::currentDevice, Manager::camera};
Pipeline Sky::transmittancePipeline{Manager::currentDevice, Manager::camera};
Pipeline Sky::scatterPipeline{Manager::currentDevice, Manager::camera};

Texture Sky::transmittanceTexture{Manager::currentDevice};
Texture Sky::scatterTexture{Manager::currentDevice};

Descriptor Sky::skyDescriptor{Manager::currentDevice};
Descriptor Sky::transmittanceDescriptor{Manager::currentDevice};
Descriptor Sky::scatterDescriptor{Manager::currentDevice};

bool Sky::transmittanceComputed = false;
bool Sky::scatterComputed = false;