#include "data.hpp"

#include "manager.hpp"

void Data::Create()
{
    CreatePipelines();
    CreateBuffers();
    CreateDescriptors();
}

void Data::CreatePipelines()
{
    std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("dataCompute", descriptorLayoutConfig);
}

void Data::CreateBuffers()
{
    computeBuffers.resize(Manager::settings.maxFramesInFlight);

    BufferConfiguration bufferConfiguration;
	bufferConfiguration.size = sizeof(ComputeData);
	bufferConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	bufferConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	bufferConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferConfiguration.mapped = true;

	for (Buffer &buffer : computeBuffers)
	{
		buffer.Create(bufferConfiguration);
	}
}

void Data::CreateDescriptors()
{
    std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].buffersInfo.resize(computeBuffers.size());
	int index = 0;
	for (Buffer &buffer : computeBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(ComputeData);
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

    computeDescriptor.Create(descriptorConfig, computePipeline.objectDescriptorSetLayout);
}

void Data::Destroy()
{
    DestroyPipelines();
    DestroyDescriptors();
    DestroyBuffers();
}

void Data::DestroyPipelines()
{
    computePipeline.Destroy();
}

void Data::DestroyDescriptors()
{
    computeDescriptor.Destroy();
}

void Data::DestroyBuffers()
{
    for (Buffer &buffer : computeBuffers) buffer.Destroy();
	computeBuffers.clear();
}

void Data::Start()
{
    computeData.resize(Manager::settings.maxFramesInFlight);
}

void Data::SetData()
{
	//vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	//memcpy(variableBuffers[Manager::currentFrame].mappedBuffer, &grassVariables, sizeof(grassVariables));
	
	vkCmdDispatch(commandBuffer, 1, 1, 1);

	Manager::currentDevice.EndComputeCommand(commandBuffer);

	computeData[Manager::currentFrame] = *(ComputeData *)computeBuffers[Manager::currentFrame].mappedBuffer;
}

ComputeData Data::GetData()
{
    return (computeData[Manager::currentFrame]);
}

Pipeline Data::computePipeline{Manager::currentDevice, Manager::camera};
Descriptor Data::computeDescriptor{Manager::currentDevice};
std::vector<Buffer> Data::computeBuffers;
std::vector<ComputeData> Data::computeData;