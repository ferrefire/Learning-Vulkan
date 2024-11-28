#include "data.hpp"

#include "manager.hpp"
#include "time.hpp"
#include "utilities.hpp"

#include <iostream>

void Data::Create()
{
    CreatePipelines();
    CreateBuffers();
    CreateDescriptors();
}

void Data::CreatePipelines()
{
	int i = 0;
    std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;
	//descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	//descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("dataCompute", descriptorLayoutConfig);
}

void Data::CreateBuffers()
{
    /*computeBuffers.resize(Manager::settings.maxFramesInFlight);

    BufferConfiguration bufferConfiguration;
	bufferConfiguration.size = sizeof(ComputeData);
	bufferConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	bufferConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	bufferConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferConfiguration.mapped = true;

	for (Buffer &buffer : computeBuffers)
	{
		buffer.Create(bufferConfiguration);
	}*/

	//intersectBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration intersectConfiguration;
	intersectConfiguration.size = sizeof(IntersectData) * intersectCount;
	intersectConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	intersectConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	intersectConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	intersectConfiguration.mapped = true;

	intersectBuffer.Create(intersectConfiguration);

	//for (Buffer &buffer : intersectBuffers)
	//{
	//	buffer.Create(intersectConfiguration);
	//}
}

void Data::CreateDescriptors()
{
	int i = 0;
	int index = 0;

    std::vector<DescriptorConfiguration> descriptorConfig(1);

	//descriptorConfig[i].type = STORAGE_BUFFER;
	//descriptorConfig[i].stages = COMPUTE_STAGE;
	//descriptorConfig[i].buffersInfo.resize(computeBuffers.size());
	//index = 0;
	//for (Buffer &buffer : computeBuffers)
	//{
	//	descriptorConfig[i].buffersInfo[index].buffer = buffer.buffer;
	//	descriptorConfig[i].buffersInfo[index].range = sizeof(ComputeData);
	//	descriptorConfig[i].buffersInfo[index].offset = 0;
	//	index++;
	//}
	//i++;

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(1);
	descriptorConfig[i].buffersInfo[0].buffer = intersectBuffer.buffer;
	descriptorConfig[i].buffersInfo[0].range = sizeof(IntersectData) * intersectCount;
	descriptorConfig[i].buffersInfo[0].offset = 0;
	//index = 0;
	//for (Buffer &buffer : intersectBuffers)
	//{
	//	descriptorConfig[i].buffersInfo[index].buffer = buffer.buffer;
	//	descriptorConfig[i].buffersInfo[index].range = sizeof(IntersectData) * intersectCount;
	//	descriptorConfig[i].buffersInfo[index].offset = 0;
	//	index++;
	//}
	i++;

	computeDescriptor.perFrame = false;
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
    //for (Buffer &buffer : computeBuffers) buffer.Destroy();
	//computeBuffers.clear();

	//for (Buffer &buffer : intersectBuffers) buffer.Destroy();
	//intersectBuffers.clear();
	intersectBuffer.Destroy();
}

void Data::Start()
{
    //computeData.resize(Manager::settings.maxFramesInFlight);
	intersectData.resize(intersectCount);
}

void Data::SetData()
{
	//vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	memcpy(intersectBuffer.mappedBuffer, intersectData.data(), sizeof(IntersectData) * intersectCount);

	vkCmdDispatch(commandBuffer, intersectCount, 1, 1);

	Manager::currentDevice.EndComputeCommand(commandBuffer);

	//computeData[Manager::currentFrame] = *(ComputeData *)computeBuffers[Manager::currentFrame].mappedBuffer;

	for (int i = 0; i < intersectCount; i++)
	{
		intersectData[i] = *((IntersectData *)intersectBuffer.mappedBuffer + i); //maybe i * sizeof

		//if (Time::newSecond && intersectData[i].active == 1)
		//{
		//	//Utilities::PrintVec(intersectData[i].position);
		//	std::cout << glm::distance(intersectData[i].position, Manager::camera.Position()) << std:: endl;
		//	//std::cout << intersectData[i].active << std::endl;
		//}

		intersectData[i].active = 0;
	}
	//if (Time::newSecond) std::cout << std::endl;
	activeIntersectCount = 0;
}

int Data::AddIntersect(glm::vec3 position)
{
	intersectData[activeIntersectCount].position = position;
	intersectData[activeIntersectCount].active = 1;
	activeIntersectCount++;
	return (activeIntersectCount - 1);
}

//ComputeData Data::GetComputeData()
//{
//    return (computeData[Manager::currentFrame]);
//}

IntersectData Data::GetIntersectData(int index)
{
	return (intersectData[index]);
}

Pipeline Data::computePipeline{Manager::currentDevice, Manager::camera};
Descriptor Data::computeDescriptor{Manager::currentDevice};
//std::vector<Buffer> Data::computeBuffers;
//std::vector<Buffer> Data::intersectBuffers;
Buffer Data::intersectBuffer;
//std::vector<ComputeData> Data::computeData;
std::vector<IntersectData> Data::intersectData;
int Data::intersectCount = 10;
int Data::activeIntersectCount = 0;