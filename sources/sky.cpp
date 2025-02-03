#include "sky.hpp"

#include "manager.hpp"
#include "terrain.hpp"
#include "input.hpp"
#include "time.hpp"

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

	std::vector<DescriptorLayoutConfiguration> viewDescriptorLayoutConfig(3);

	viewDescriptorLayoutConfig[0].type = IMAGE_STORAGE;
	viewDescriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	viewDescriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	viewDescriptorLayoutConfig[1].stages = COMPUTE_STAGE;
	viewDescriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	viewDescriptorLayoutConfig[2].stages = COMPUTE_STAGE;

	viewPipeline.CreateComputePipeline("viewCompute", viewDescriptorLayoutConfig);
}

void Sky::CreateTextures()
{
	SamplerConfiguration transmittanceSampler;
	ImageConfiguration transmittanceConfig = Texture::ImageStorage(256, 64);
	transmittanceConfig.format = RGB16;

	transmittanceTexture.CreateImage(transmittanceConfig, transmittanceSampler);
	transmittanceTexture.TransitionImageLayout(transmittanceConfig);

	SamplerConfiguration scatterSampler;
	ImageConfiguration scatterConfig = Texture::ImageStorage(32, 32);
	scatterConfig.format = RGB16;

	scatterTexture.CreateImage(scatterConfig, scatterSampler);
	scatterTexture.TransitionImageLayout(scatterConfig);

	SamplerConfiguration viewSampler;
	ImageConfiguration viewConfig = Texture::ImageStorage(192, 128);
	viewConfig.format = RGB16;

	viewTexture.CreateImage(viewConfig, viewSampler);
	viewTexture.TransitionImageLayout(viewConfig);
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
	skyDescriptorConfig[2].imageInfo.imageView = viewTexture.imageView;
	skyDescriptorConfig[2].imageInfo.sampler = viewTexture.sampler;

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

	std::vector<DescriptorConfiguration> viewDescriptorConfig(3);

	viewDescriptorConfig[0].type = IMAGE_STORAGE;
	viewDescriptorConfig[0].stages = COMPUTE_STAGE;
	viewDescriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	viewDescriptorConfig[0].imageInfo.imageView = viewTexture.imageView;
	viewDescriptorConfig[1].type = IMAGE_SAMPLER;
	viewDescriptorConfig[1].stages = COMPUTE_STAGE;
	viewDescriptorConfig[1].imageInfo.imageLayout = LAYOUT_GENERAL;
	viewDescriptorConfig[1].imageInfo.imageView = transmittanceTexture.imageView;
	viewDescriptorConfig[1].imageInfo.sampler = transmittanceTexture.sampler;
	viewDescriptorConfig[2].type = IMAGE_SAMPLER;
	viewDescriptorConfig[2].stages = COMPUTE_STAGE;
	viewDescriptorConfig[2].imageInfo.imageLayout = LAYOUT_GENERAL;
	viewDescriptorConfig[2].imageInfo.imageView = scatterTexture.imageView;
	viewDescriptorConfig[2].imageInfo.sampler = scatterTexture.sampler;

	viewDescriptor.perFrame = false;
	viewDescriptor.Create(viewDescriptorConfig, viewPipeline.objectDescriptorSetLayout);
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
	viewPipeline.Destroy();
}

void Sky::DestroyMesh()
{
	skyMesh.Destroy();
}

void Sky::DestroyTextures()
{
	transmittanceTexture.Destroy();
	scatterTexture.Destroy();
	viewTexture.Destroy();
}

void Sky::DestroyDescriptors()
{
	skyDescriptor.Destroy();
	transmittanceDescriptor.Destroy();
	scatterDescriptor.Destroy();
	viewDescriptor.Destroy();
}

void Sky::Start()
{
	
}

void Sky::Frame()
{
	if (Input::GetKey(GLFW_KEY_L).pressed)
	{
		Recompute();
		return;
	}

	if (!transmittanceComputed && Terrain::HeightMapsGenerated())
	{
		transmittanceComputed = true;
		ComputeTransmittance();
	}

	if (!scatterComputed && transmittanceComputed && transmittanceReady && Terrain::HeightMapsGenerated())
	{
		scatterComputed = true;
		ComputeScattering();
	}

	if (!viewComputed && transmittanceComputed && transmittanceReady && scatterComputed && scatterReady && Terrain::HeightMapsGenerated())
	{
		viewComputed = true;
		ComputeView();
	}

	if (Time::newTick)
	{
		if (shouldUpdate && viewComputed && transmittanceComputed && transmittanceReady && scatterComputed && scatterReady && Terrain::HeightMapsGenerated())
		{
			shouldUpdate = false;
			ComputeView();
		}
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

void Sky::ComputeTransmittance()
{
	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	transmittancePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, transmittancePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	transmittanceDescriptor.Bind(commandBuffer, transmittancePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, 32, 8, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);

	transmittanceReady = true;
}

void Sky::ComputeScattering()
{
	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	scatterPipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, scatterPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	scatterDescriptor.Bind(commandBuffer, scatterPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, 32, 32, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);

	scatterReady = true;
}

void Sky::ComputeView()
{
	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	viewPipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, viewPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	viewDescriptor.Bind(commandBuffer, viewPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, 24, 16, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

void Sky::Recompute()
{
	transmittanceComputed = false;
	transmittanceReady = false;
	scatterComputed = false;
	scatterReady = false;
	viewComputed = false;
}

Mesh Sky::skyMesh;

Pipeline Sky::skyPipeline{Manager::currentDevice, Manager::camera};
Pipeline Sky::transmittancePipeline{Manager::currentDevice, Manager::camera};
Pipeline Sky::scatterPipeline{Manager::currentDevice, Manager::camera};
Pipeline Sky::viewPipeline{Manager::currentDevice, Manager::camera};

Texture Sky::transmittanceTexture{Manager::currentDevice};
Texture Sky::scatterTexture{Manager::currentDevice};
Texture Sky::viewTexture{Manager::currentDevice};

Descriptor Sky::skyDescriptor{Manager::currentDevice};
Descriptor Sky::transmittanceDescriptor{Manager::currentDevice};
Descriptor Sky::scatterDescriptor{Manager::currentDevice};
Descriptor Sky::viewDescriptor{Manager::currentDevice};

bool Sky::transmittanceComputed = false;
bool Sky::transmittanceReady = false;
bool Sky::scatterComputed = false;
bool Sky::scatterReady = false;
bool Sky::viewComputed = false;
bool Sky::shouldUpdate = false;