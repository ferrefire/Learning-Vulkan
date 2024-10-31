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

	VertexInfo vertexInfo = grassMesh.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("grass", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Grass::CreateComputePipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("grassCompute", descriptorLayoutConfig);
}

void Grass::CreateBuffers()
{
	dataBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration configuration;
	configuration.size = sizeof(GrassData) * grassTotalCount;
	configuration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	configuration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	configuration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	configuration.mapped = false;

	for (Buffer &buffer : dataBuffers)
	{
		buffer.Create(configuration);
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
	std::vector<DescriptorConfiguration> descriptorConfig(1);

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

}

void Grass::RecordCommandBuffer(VkCommandBuffer commandBuffer)
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

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(grassMesh.indices.size()), grassTotalCount, 0, 0, 0);
}

void Grass::ComputeGrass()
{
	//vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computePipeline.BindCompute(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	Manager::UpdateShaderVariables();

	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	
	vkCmdDispatch(commandBuffer, 16, 16, 1); // Change dispatch count

	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

int Grass::grassCount = 128;
int Grass::grassTotalCount = Grass::grassCount * Grass::grassCount;
int Grass::grassRenderCount = 0;

Mesh Grass::grassMesh;

Pipeline Grass::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Grass::computePipeline{Manager::currentDevice, Manager::camera};

Descriptor Grass::graphicsDescriptor{Manager::currentDevice};
Descriptor Grass::computeDescriptor{Manager::currentDevice};

std::vector<Buffer> Grass::dataBuffers;