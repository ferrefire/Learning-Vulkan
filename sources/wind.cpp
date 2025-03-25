#include "wind.hpp"
#include "manager.hpp"
#include "ui.hpp"

void Wind::Create()
{
    CreateTextures();
    CreatePipelines();
    CreateDescriptors();
}

void Wind::CreateTextures()
{
    SamplerConfiguration samplerConfig;
    samplerConfig.repeatMode = REPEAT;

    ImageConfiguration imageConfig = Texture::ImageStorage(resolution, resolution);
    windTexture.CreateImage(imageConfig, samplerConfig);
    windTexture.TransitionImageLayout(imageConfig);
}

void Wind::CreatePipelines()
{
    std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = IMAGE_STORAGE;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("windCompute", descriptorLayoutConfig);
}

void Wind::CreateDescriptors()
{
    std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = IMAGE_STORAGE;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[0].imageInfo.imageView = windTexture.imageView;

	computeDescriptor.perFrame = false;
	computeDescriptor.Create(descriptorConfig, computePipeline.objectDescriptorSetLayout);
}

void Wind::Destroy()
{
    DestroyTextures();
    DestroyPipelines();
    DestroyDescriptors();
}

void Wind::DestroyTextures()
{
    windTexture.Destroy();
}

void Wind::DestroyPipelines()
{
    computePipeline.Destroy();
}

void Wind::DestroyDescriptors()
{
    computeDescriptor.Destroy();
}

void Wind::Start()
{
    Menu &menu = UI::NewMenu("wind");
    menu.AddText("wind values");
    menu.AddSlider("wind strength", windStrength, 0.0f, 25.0f);
    menu.AddSlider("wind distance", windDistance, 0.0f, 1000.0f);
}

void Wind::RecordComputeCommands(VkCommandBuffer commandBuffer)
{
    ComputeWind(commandBuffer);
}

void Wind::ComputeWind(VkCommandBuffer commandBuffer)
{
    int dispatchCount = int(floor(float(resolution) / 8.0f));

    computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
    computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

    vkCmdDispatch(commandBuffer, dispatchCount, dispatchCount, 1);
}

int Wind::resolution = 256;

float Wind::windStrength = 5.0;
float Wind::windDistance = 200.0;

Texture Wind::windTexture;

Pipeline Wind::computePipeline{Manager::currentDevice, Manager::camera};

Descriptor Wind::computeDescriptor{Manager::currentDevice};