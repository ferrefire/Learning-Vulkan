#include "grass.hpp"

#include "manager.hpp"
#include "shape.hpp"

void Grass::Create()
{
	CreateMeshes();
	CreateGraphicsPipeline();
	CreateComputePipeline();
	CreateBuffers();
	CreateGraphicsDescriptor();
	CreateComputeDescriptor();
}

void Grass::CreateMeshes()
{
	grassMesh.shape.SetShape(BLADE, 2);
	grassMesh.RecalculateVertices();
	grassMesh.Create();
}

void Grass::CreateGraphicsPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.foliage = true;

	VertexInfo vertexInfo = grassMesh.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("grass", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Grass::CreateComputePipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[1].type = STORAGE_BUFFER;
	descriptorLayoutConfig[1].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("grassCompute", descriptorLayoutConfig);
}

void Grass::CreateBuffers()
{
	dataBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration dataConfiguration;
	dataConfiguration.size = sizeof(GrassData) * grassTotalCount;
	dataConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	dataConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	dataConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	dataConfiguration.mapped = false;

	for (Buffer &buffer : dataBuffers)
	{
		buffer.Create(dataConfiguration);
	}

	countBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration countConfiguration;
	countConfiguration.size = sizeof(uint32_t);
	countConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	countConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	countConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	countConfiguration.mapped = true;

	for (Buffer &buffer : countBuffers)
	{
		buffer.Create(countConfiguration);
	}
}

void Grass::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());

	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(GrassData) * grassTotalCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Grass::CreateComputeDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(2);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());
	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(GrassData) * grassTotalCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[1].type = STORAGE_BUFFER;
	descriptorConfig[1].stages = COMPUTE_STAGE;
	descriptorConfig[1].buffersInfo.resize(countBuffers.size());
	index = 0;
	for (Buffer &buffer : countBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(uint32_t);
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	computeDescriptor.Create(descriptorConfig, computePipeline.objectDescriptorSetLayout);
}

void Grass::Destroy()
{
	DestroyPipelines();
	DestroyMeshes();
	DestroyDescriptors();
	DestroyBuffers();
}

void Grass::DestroyMeshes()
{
	grassMesh.Destroy();
}

void Grass::DestroyPipelines()
{
	graphicsPipeline.Destroy();
	computePipeline.Destroy();
}

void Grass::DestroyBuffers()
{
	for (Buffer &buffer : dataBuffers)
	{
		buffer.Destroy();
	}

	dataBuffers.clear();

	for (Buffer &buffer : countBuffers)
	{
		buffer.Destroy();
	}

	countBuffers.clear();
}

void Grass::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	computeDescriptor.Destroy();
}

void Grass::Start()
{

}

void Grass::Frame()
{
	ComputeGrass();
}

void Grass::RecordCommands(VkCommandBuffer commandBuffer)
{
	graphicsPipeline.BindGraphics(commandBuffer, Manager::currentWindow);

	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	Manager::UpdateShaderVariables();

	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderGrass(commandBuffer);
}

void Grass::RenderGrass(VkCommandBuffer commandBuffer)
{
	grassMesh.Bind(commandBuffer);

	int renderCount = *(int *)countBuffers[Manager::currentFrame].mappedBuffer;
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassMesh.indices.size()), renderCount, 0, 0, 0);
}

void Grass::ComputeGrass()
{
	//vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computePipeline.BindCompute(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	Manager::UpdateShaderVariables();

	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	
	int computeCount = ceil((float)grassCount / 8.0);

	vkCmdDispatch(commandBuffer, computeCount, computeCount, 1); // Change dispatch count

	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

int Grass::grassCount = 2048;
int Grass::grassTotalCount = Grass::grassCount * Grass::grassCount;
int Grass::grassRenderCount = 0;

Mesh Grass::grassMesh;

Pipeline Grass::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Grass::computePipeline{Manager::currentDevice, Manager::camera};

Descriptor Grass::graphicsDescriptor{Manager::currentDevice};
Descriptor Grass::computeDescriptor{Manager::currentDevice};

std::vector<Buffer> Grass::dataBuffers;
std::vector<Buffer> Grass::countBuffers;