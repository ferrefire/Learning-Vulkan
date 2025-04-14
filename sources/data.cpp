#include "data.hpp"

#include "manager.hpp"
#include "time.hpp"
#include "utilities.hpp"

#include <iostream>

void Data::Create()
{
	generalData.resize(Manager::settings.maxFramesInFlight);

    CreatePipelines();
    CreateBuffers();
    CreateDescriptors();
}

void Data::CreatePipelines()
{
	int i = 0;
    std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("dataCompute", descriptorLayoutConfig);
}

void Data::CreateBuffers()
{
    BufferConfiguration generalBufferConfiguration;
	generalBufferConfiguration.size = sizeof(GeneralData);
	generalBufferConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	generalBufferConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	generalBufferConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	generalBufferConfiguration.mapped = true;

	generalBuffer.Create(generalBufferConfiguration);

	BufferConfiguration heightBufferConfiguration;
	heightBufferConfiguration.size = sizeof(HeightData) * 100;
	heightBufferConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	heightBufferConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	heightBufferConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	heightBufferConfiguration.mapped = true;

	heightBuffer.Create(heightBufferConfiguration);
}

void Data::CreateDescriptors()
{
	int i = 0;
	int index = 0;

    std::vector<DescriptorConfiguration> descriptorConfig(2);

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(1);
	descriptorConfig[i].buffersInfo[0].buffer = generalBuffer.buffer;
	descriptorConfig[i].buffersInfo[0].range = sizeof(GeneralData);
	descriptorConfig[i++].buffersInfo[0].offset = 0;

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(1);
	descriptorConfig[i].buffersInfo[0].buffer = heightBuffer.buffer;
	descriptorConfig[i].buffersInfo[0].range = sizeof(HeightData) * 100;
	descriptorConfig[i++].buffersInfo[0].offset = 0;

	computeDescriptor.perFrame = false;
	computeDescriptor.Create(descriptorConfig, computePipeline.objectDescriptorSetLayout);
}

void Data::Destroy()
{
    DestroyPipelines();
	DestroyBuffers();
	DestroyDescriptors();
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
	generalBuffer.Destroy();
	heightBuffer.Destroy();
}

void Data::Start()
{
	//intersectData.resize(intersectCount);
}

void Data::RecordComputeCommands(VkCommandBuffer commandBuffer)
{
	for (int i = 0; i < requestCount; i++)
	{
		heightData[i].position = glm::vec4(requestData[i].position, 0.0f);
	}
	memcpy(heightBuffer.mappedBuffer, heightData.data(), sizeof(HeightData) * requestCount);

	ComputeGeneralData(commandBuffer);
}

void Data::ComputeGeneralData(VkCommandBuffer commandBuffer)
{
	bool oneTimeBuffer = commandBuffer == nullptr;
	if (oneTimeBuffer) commandBuffer = Manager::currentDevice.BeginComputeCommand();

	int dispatchCount = requestCount + 1;

	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, dispatchCount, 1, 1);

	if (oneTimeBuffer)
	{
		Manager::currentDevice.EndComputeCommand(commandBuffer);
		//computeData[Manager::currentFrame] = *(GeneralData *)computeBuffer[Manager::currentFrame].mappedBuffer;
		generalData[Manager::currentFrame].viewHeight = (*(GeneralData *)generalBuffer.mappedBuffer).viewHeight;
	}
}

void Data::SetData()
{
	generalData[Manager::currentFrame].viewHeight = (*(GeneralData *)generalBuffer.mappedBuffer).viewHeight;

	for (int i = 0; i < requestCount; i++)
	{
		*requestData[i].source += (*((HeightData *)(heightBuffer.mappedBuffer + sizeof(HeightData) * i))).position.w;
		//*requestData[i].source = (*(HeightData *)(heightBuffer.mappedBuffer + (i))).position.w;
		requestData[i].func(requestData[i].index);
	}

	requestCount = 0;
}

void Data::RequestData(glm::vec3 position, float *source, void (*func)(int), int index)
{
	//DataRequest newRequest{position, source, func, index};
	requestData[requestCount].position = position;
	requestData[requestCount].source = source;
	requestData[requestCount].func = func;
	requestData[requestCount].index = index;

	requestCount++;
}

/*void Data::SetData()
{
	//vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	memcpy(intersectBuffer.mappedBuffer, intersectData.data(), sizeof(IntersectData) * intersectCount);

	vkCmdDispatch(commandBuffer, intersectCount, 1, 1);

	Manager::currentDevice.EndComputeCommand(commandBuffer);

	//computeData[Manager::currentFrame] = *(GeneralData *)computeBuffers[Manager::currentFrame].mappedBuffer;

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
}*/

//int Data::AddIntersect(glm::vec3 position)
//{
//	intersectData[activeIntersectCount].position = position;
//	intersectData[activeIntersectCount].active = 1;
//	activeIntersectCount++;
//	return (activeIntersectCount - 1);
//}

GeneralData Data::GetGeneralData()
{
    return (generalData[Manager::currentFrame]);
}

//IntersectData Data::GetIntersectData(int index)
//{
//	return (intersectData[index]);
//}

Pipeline Data::computePipeline{Manager::currentDevice, Manager::camera};

Buffer Data::generalBuffer;
Buffer Data::heightBuffer;

Descriptor Data::computeDescriptor{Manager::currentDevice};

std::vector<GeneralData> Data::generalData;
std::vector<HeightData> Data::heightData = std::vector<HeightData>(100);
std::vector<DataRequest> Data::requestData = std::vector<DataRequest>(100);

int Data::requestCount = 0;