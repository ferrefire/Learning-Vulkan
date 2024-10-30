#include "terrain.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "time.hpp"

#include <stdexcept>
#include <iostream>

void Terrain::Create()
{
    CreateMeshes();
	CreateGraphicsPipeline();
	CreateComputePipelines();
	CreateTextures();
	CreateObjects();
	CreateBuffers();
	CreateGraphicsDescriptor();
	CreateComputeDescriptors();
}

void Terrain::CreateTextures()
{
	SamplerConfiguration grassSamplerConfig;
	grassSamplerConfig.repeatMode = REPEAT;
	grassSamplerConfig.mipLodBias = 0.0f;

	grassDiffuseTexture.CreateTexture("rocky_grass_diff.jpg", grassSamplerConfig);
	//grassNormalTexture.CreateTexture("rocky_grass_norm.jpg", grassSamplerConfig);
	//grassSpecularTexture.CreateTexture("rocky_grass_spec.jpg", grassSamplerConfig);

	rockDiffuseTexture.CreateTexture("rock_diff.jpg", grassSamplerConfig);

	dirtDiffuseTexture.CreateTexture("dirt_diff.jpg", grassSamplerConfig);

	SamplerConfiguration heightMapSamplerConfig;
	SamplerConfiguration heightMapArraySamplerConfig;

	//ImageConfiguration heightMapConfig = Texture::ImageStorage(1024, 1024);
	//heightMapTexture.CreateImage(heightMapConfig, heightMapSamplerConfig);
	//heightMapTexture.TransitionImageLayout(heightMapConfig);

	ImageConfiguration heightMapArrayConfig = Texture::ImageArrayStorage(1024, 1024, heightMapCount);
	heightMapArrayTexture.CreateImage(heightMapArrayConfig, heightMapArraySamplerConfig);
	heightMapArrayTexture.TransitionImageLayout(heightMapArrayConfig);

	ImageConfiguration heightMapLod1Config = Texture::ImageStorage(1024, 1024);
	heightMapLod1Texture.CreateImage(heightMapLod1Config, heightMapSamplerConfig);
	heightMapLod1Texture.TransitionImageLayout(heightMapLod1Config);

	ImageConfiguration heightMapLod0Config = Texture::ImageStorage(1024, 1024);
	heightMapLod0Texture.CreateImage(heightMapLod0Config, heightMapSamplerConfig);
	heightMapLod0Texture.TransitionImageLayout(heightMapLod0Config);
}

void Terrain::CreateMeshes()
{
    lod0Mesh.shape.SetShape(PLANE, 100);
	lod0Mesh.RecalculateVertices();
	lod0Mesh.Create();

	lod1Mesh.shape.SetShape(PLANE, 10);
	lod1Mesh.RecalculateVertices();
	lod1Mesh.Create();
}

void Terrain::CreateObjects()
{
	//object.Create();
	//object.Resize(glm::vec3(10000));

	terrainChunks.resize(terrainChunkCount);

	for (int x = -terrainChunkRadius; x <= terrainChunkRadius; x++)
	{
		for (int y = -terrainChunkRadius; y <= terrainChunkRadius; y++)
		{
			int i = (x + terrainChunkRadius) * terrainChunkLength + (y + terrainChunkRadius);
			terrainChunks[i].mesh = &lod1Mesh;
			terrainChunks[i].pipeline = &graphicsPipeline;
			terrainChunks[i].Create();
			terrainChunks[i].Resize(glm::vec3(terrainChunkSize));
			terrainChunks[i].Move(glm::vec3(x, 0, y) * terrainChunkSize);
		}
	}
}

void Terrain::CreateGraphicsPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(8);
	descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[0].count = terrainChunkCount;
	descriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[2].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[3].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[3].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[4].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[4].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[5].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[5].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[6].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[6].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[7].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[7].stages = FRAGMENT_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.tesselation = true;
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

    VertexInfo vertexInfo = lod0Mesh.MeshVertexInfo();

    graphicsPipeline.CreateGraphicsPipeline("terrain", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Terrain::CreateComputePipelines()
{
	std::vector<DescriptorLayoutConfiguration> heightMapDescriptorLayoutConfig(2);
	heightMapDescriptorLayoutConfig[0].type = IMAGE_STORAGE;
	heightMapDescriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	heightMapDescriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	heightMapDescriptorLayoutConfig[1].stages = COMPUTE_STAGE;

	heightMapComputePipeline.CreateComputePipeline("heightMapCompute", heightMapDescriptorLayoutConfig);

	std::vector<DescriptorLayoutConfiguration> heightMapArrayDescriptorLayoutConfig(2);
	heightMapArrayDescriptorLayoutConfig[0].type = IMAGE_STORAGE;
	heightMapArrayDescriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	heightMapArrayDescriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	heightMapArrayDescriptorLayoutConfig[1].stages = COMPUTE_STAGE;

	heightMapArrayComputePipeline.CreateComputePipeline("heightMapArrayCompute", heightMapArrayDescriptorLayoutConfig);
}

void Terrain::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(8);

	int bufferCount = terrainChunks[0].uniformBuffers.size();
	int objectCount = terrainChunks.size();

	descriptorConfig[0].type = UNIFORM_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[0].count = objectCount;
	//descriptorConfig[0].buffersInfo.resize(terrainChunks[0].uniformBuffers.size());
	//int i = 0;
	//for (Buffer &buffer : terrainChunks[0].uniformBuffers)
	//{
	//	descriptorConfig[0].buffersInfo[i].buffer = buffer.buffer;
	//	descriptorConfig[0].buffersInfo[i].range = sizeof(UniformBufferObject);
	//	descriptorConfig[0].buffersInfo[i].offset = 0;
	//	i++;
	//}
	
	descriptorConfig[0].buffersInfo.resize(bufferCount * objectCount);
	
	for (int i = 0; i < bufferCount; i++)
	{
		int j = 0;
		for (Object &object : terrainChunks)
		{
			descriptorConfig[0].buffersInfo[i * objectCount + j].buffer = object.uniformBuffers[i].buffer;
			descriptorConfig[0].buffersInfo[i * objectCount + j].range = sizeof(UniformBufferObject);
			descriptorConfig[0].buffersInfo[i * objectCount + j].offset = 0;
			j++;
		}
	}

	descriptorConfig[1].type = IMAGE_SAMPLER;
	descriptorConfig[1].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[1].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[1].imageInfo.imageView = heightMapArrayTexture.imageView;
	descriptorConfig[1].imageInfo.sampler = heightMapArrayTexture.sampler;

	descriptorConfig[2].type = IMAGE_SAMPLER;
	descriptorConfig[2].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[2].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[2].imageInfo.imageView = heightMapLod0Texture.imageView;
	descriptorConfig[2].imageInfo.sampler = heightMapLod0Texture.sampler;

	descriptorConfig[3].type = IMAGE_SAMPLER;
	descriptorConfig[3].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[3].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[3].imageInfo.imageView = heightMapLod1Texture.imageView;
	descriptorConfig[3].imageInfo.sampler = heightMapLod1Texture.sampler;

	descriptorConfig[4].type = UNIFORM_BUFFER;
	descriptorConfig[4].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[4].buffersInfo.resize(heightMapVariablesBuffers.size());
	int i = 0;
	for (Buffer &buffer : heightMapVariablesBuffers)
	{
		descriptorConfig[4].buffersInfo[i].buffer = buffer.buffer;
		descriptorConfig[4].buffersInfo[i].range = sizeof(HeightMapVariables);
		descriptorConfig[4].buffersInfo[i].offset = 0;
		i++;
	}

	descriptorConfig[5].type = IMAGE_SAMPLER;
	descriptorConfig[5].stages = FRAGMENT_STAGE;
	descriptorConfig[5].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[5].imageInfo.imageView = grassDiffuseTexture.imageView;
	descriptorConfig[5].imageInfo.sampler = grassDiffuseTexture.sampler;

	descriptorConfig[6].type = IMAGE_SAMPLER;
	descriptorConfig[6].stages = FRAGMENT_STAGE;
	descriptorConfig[6].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[6].imageInfo.imageView = rockDiffuseTexture.imageView;
	descriptorConfig[6].imageInfo.sampler = rockDiffuseTexture.sampler;

	descriptorConfig[7].type = IMAGE_SAMPLER;
	descriptorConfig[7].stages = FRAGMENT_STAGE;
	descriptorConfig[7].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[7].imageInfo.imageView = dirtDiffuseTexture.imageView;
	descriptorConfig[7].imageInfo.sampler = dirtDiffuseTexture.sampler;

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Terrain::CreateComputeDescriptors()
{
	std::vector<DescriptorConfiguration> heightMapDescriptorConfig(2);

	heightMapDescriptorConfig[0].type = IMAGE_STORAGE;
	heightMapDescriptorConfig[0].stages = COMPUTE_STAGE;
	heightMapDescriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	heightMapDescriptorConfig[0].imageInfo.imageView = heightMapLod0Texture.imageView;
	//heightMapDescriptorConfig[0].imageInfo.sampler = heightMapTexture.sampler;
	heightMapDescriptorConfig[1].type = UNIFORM_BUFFER;
	heightMapDescriptorConfig[1].stages = COMPUTE_STAGE;
	heightMapDescriptorConfig[1].buffersInfo.resize(1);
	heightMapDescriptorConfig[1].buffersInfo[0].buffer = heightMapComputeVariablesBuffer.buffer;
	heightMapDescriptorConfig[1].buffersInfo[0].range = sizeof(HeightMapComputeVariables);
	heightMapDescriptorConfig[1].buffersInfo[0].offset = 0;

	heightMapComputeDescriptor.perFrame = false;
	heightMapComputeDescriptor.Create(heightMapDescriptorConfig, heightMapComputePipeline.objectDescriptorSetLayout);

	std::vector<DescriptorConfiguration> heightMapArrayDescriptorConfig(2);

	heightMapArrayDescriptorConfig[0].type = IMAGE_STORAGE;
	heightMapArrayDescriptorConfig[0].stages = COMPUTE_STAGE;
	heightMapArrayDescriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	heightMapArrayDescriptorConfig[0].imageInfo.imageView = heightMapArrayTexture.imageView;
	// heightMapArrayDescriptorConfig[0].imageInfo.sampler = heightMapArrayTexture.sampler;
	heightMapArrayDescriptorConfig[1].type = UNIFORM_BUFFER;
	heightMapArrayDescriptorConfig[1].stages = COMPUTE_STAGE;
	heightMapArrayDescriptorConfig[1].buffersInfo.resize(1);
	heightMapArrayDescriptorConfig[1].buffersInfo[0].buffer = heightMapArrayComputeVariablesBuffer.buffer;
	heightMapArrayDescriptorConfig[1].buffersInfo[0].range = sizeof(HeightMapArrayComputeVariables);
	heightMapArrayDescriptorConfig[1].buffersInfo[0].offset = 0;

	heightMapArrayComputeDescriptor.perFrame = false;
	heightMapArrayComputeDescriptor.Create(heightMapArrayDescriptorConfig, heightMapArrayComputePipeline.objectDescriptorSetLayout);
}

void Terrain::CreateBuffers()
{
	heightMapVariablesBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration configuration;
	configuration.size = sizeof(HeightMapVariables);
	configuration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	configuration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	configuration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	configuration.mapped = true;

	for (Buffer &buffer : heightMapVariablesBuffers)
	{
		buffer.Create(configuration);
	}

	BufferConfiguration computeConfiguration;
	computeConfiguration.size = sizeof(HeightMapComputeVariables);
	computeConfiguration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	computeConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	computeConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	computeConfiguration.mapped = true;

	heightMapComputeVariablesBuffer.Create(computeConfiguration);

	BufferConfiguration arrayComputeConfiguration;
	arrayComputeConfiguration.size = sizeof(HeightMapArrayComputeVariables);
	arrayComputeConfiguration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	arrayComputeConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	arrayComputeConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	arrayComputeConfiguration.mapped = true;

	heightMapArrayComputeVariablesBuffer.Create(arrayComputeConfiguration);
}

void Terrain::Destroy()
{
	DestroyPipelines();
    DestroyMeshes();
    DestroyTextures();
	DestroyObjects();
	DestroyDescriptors();
	DestroyBuffers();
}

void Terrain::DestroyTextures()
{
	grassDiffuseTexture.Destroy();
	//grassNormalTexture.Destroy();
	//grassSpecularTexture.Destroy();

	rockDiffuseTexture.Destroy();
	dirtDiffuseTexture.Destroy();

	heightMapArrayTexture.Destroy();
	heightMapLod1Texture.Destroy();
	heightMapLod0Texture.Destroy();
}

void Terrain::DestroyMeshes()
{
	lod0Mesh.Destroy();
	lod1Mesh.Destroy();
}

void Terrain::DestroyObjects()
{
	//object.Destroy();

	for (Object &object : terrainChunks)
	{
		object.Destroy();
	}
}

void Terrain::DestroyPipelines()
{
	graphicsPipeline.Destroy();
	heightMapComputePipeline.Destroy();
	heightMapArrayComputePipeline.Destroy();
}

void Terrain::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	heightMapComputeDescriptor.Destroy();
	heightMapArrayComputeDescriptor.Destroy();
}

void Terrain::DestroyBuffers()
{
	for (Buffer &buffer : heightMapVariablesBuffers)
	{
		buffer.Destroy();
	}

	heightMapVariablesBuffers.clear();

	heightMapComputeVariablesBuffer.Destroy();

	heightMapArrayComputeVariablesBuffer.Destroy();
}

void Terrain::Start()
{
	//ComputeHeightMapArray(0);

	for (int i = 0; i < heightMapCount; i++)
	{
		ComputeHeightMapArray(i);
	}

	//ComputeHeightMap(2);
	ComputeHeightMap(1);
	ComputeHeightMap(0);
}

void Terrain::Frame()
{
	if (Time::newSubTick)
	{
		CheckTerrainOffset();
	}
}

void Terrain::RecordCommands(VkCommandBuffer commandBuffer)
{
    graphicsPipeline.BindGraphics(commandBuffer, Manager::currentWindow);
	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	Manager::UpdateShaderVariables();
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	UpdateHeightMapVariables();

	uint32_t closestIndex = 0;
	glm::vec3 viewPosition = Manager::currentCamera.Position();
	int x = floor((viewPosition.x / terrainChunkSize) + 0.5) + terrainChunkRadius;
	int z = floor((viewPosition.z / terrainChunkSize) + 0.5) + terrainChunkRadius;
	closestIndex = x * terrainChunkLength + z;
	uint32_t chunkIndex = closestIndex;

	lod0Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout,
		VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE, 0, sizeof(chunkIndex), &chunkIndex);
	terrainChunks[closestIndex].ModifyPosition().y = 0;
	terrainChunks[closestIndex].UpdateUniformBuffer(Manager::currentFrame);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lod0Mesh.indices.size()), 1, 0, 0, 0);

	lod1Mesh.Bind(commandBuffer);
	chunkIndex = 0;
	for (Object &object : terrainChunks)
	{
		if (chunkIndex != closestIndex)
		{
			vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout,
				VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE, 0, sizeof(chunkIndex), &chunkIndex);
			object.ModifyPosition().y = -10;
			object.UpdateUniformBuffer(Manager::currentFrame);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lod1Mesh.indices.size()), 1, 0, 0, 0);
		}
		chunkIndex++;
	}
}

void Terrain::ComputeHeightMap(uint32_t lod)
{
	//std::cout << "Lod: " << lod << std::endl;
	//float startTime = Time::GetCurrentTime();
	//std::cout << "Start time: " << startTime << std::endl;

	vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();
	heightMapComputePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, heightMapComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	Manager::UpdateShaderVariables();

	if (currentBoundHeightMap != lod)
	{
		heightMapComputeDescriptor.descriptorConfigs[0].imageInfo.imageView = (lod == 0 ? heightMapLod0Texture.imageView : heightMapLod1Texture.imageView);
		heightMapComputeDescriptor.Update();
	}

	heightMapComputeDescriptor.Bind(commandBuffer, heightMapComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	heightMapComputeVariables.mapScale = (lod == 0 ? terrainLod0Size : (lod == 1 ? terrainLod1Size : terrainChunkSize)) / terrainChunkSize;
	heightMapComputeVariables.mapOffset = (lod == 0 ? (terrainLod0Offset / terrainLod0Size) : (terrainLod1Offset / terrainLod1Size));
	memcpy(heightMapComputeVariablesBuffer.mappedBuffer, &heightMapComputeVariables, sizeof(heightMapComputeVariables));
	vkCmdDispatch(commandBuffer, 128, 128, 1); //Change dispatch count
	Manager::currentDevice.EndComputeCommand(commandBuffer);

	currentBoundHeightMap = lod;

	//float endTime = Time::GetCurrentTime();
	//std::cout << "Start time: " << endTime << std::endl;
	//std::cout << "Duration: " << endTime - startTime << std::endl << std::endl;
}

void Terrain::ComputeHeightMapArray(uint32_t index)
{
	// std::cout << "Lod: " << lod << std::endl;
	// float startTime = Time::GetCurrentTime();
	// std::cout << "Start time: " << startTime << std::endl;
	int x = (index % heightMapLength) - heightMapRadius;
	int y = (index / heightMapLength) - heightMapRadius;

	vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();
	heightMapArrayComputePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, heightMapArrayComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	Manager::UpdateShaderVariables();

	//if (currentBoundHeightMap != lod)
	//{
	//	heightMapArrayComputeDescriptor.descriptorConfigs[0].imageInfo.imageView = (lod == 0 ? heightMapLod0Texture.imageView : heightMapLod1Texture.imageView);
	//	heightMapArrayComputeDescriptor.Update();
	//}

	heightMapArrayComputeDescriptor.Bind(commandBuffer, heightMapArrayComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	heightMapArrayComputeVariables.mapScale = 1.0;
	heightMapArrayComputeVariables.currentChunkIndex = index;
	heightMapArrayComputeVariables.mapOffset = glm::vec2(y, x);
	memcpy(heightMapArrayComputeVariablesBuffer.mappedBuffer, &heightMapArrayComputeVariables, sizeof(heightMapArrayComputeVariables));
	vkCmdDispatch(commandBuffer, 128, 128, 1); // Change dispatch count
	Manager::currentDevice.EndComputeCommand(commandBuffer);

	//currentBoundHeightMap = lod;

	// float endTime = Time::GetCurrentTime();
	// std::cout << "Start time: " << endTime << std::endl;
	// std::cout << "Duration: " << endTime - startTime << std::endl << std::endl;
}

void Terrain::CheckTerrainOffset()
{
	float xw = Manager::currentCamera.Position().x;
	float zw = Manager::currentCamera.Position().z;

	float x0 = xw - terrainLod0Offset.x;
	float z0 = zw - terrainLod0Offset.y;
	float x1 = xw - terrainLod1Offset.x;
	float z1 = zw - terrainLod1Offset.y;

	if (abs(x0) >= terrainLod0Size * terrainLod0Step || abs(z0) >= terrainLod0Size * terrainLod0Step)
	{
		glm::vec2 newOffset = glm::vec2(Utilities::Fits(terrainLod0Size * terrainLod0Step, x0), Utilities::Fits(terrainLod0Size * terrainLod0Step, z0)) * terrainLod0Size * terrainLod0Step;
		terrainLod0Offset += newOffset;

		ComputeHeightMap(0);
	}
	else if (abs(x1) >= terrainLod1Size * terrainLod1Step || abs(z1) >= terrainLod1Size * terrainLod1Step)
	{
		glm::vec2 newOffset = glm::vec2(Utilities::Fits(terrainLod1Size * terrainLod1Step, x1), Utilities::Fits(terrainLod1Size * terrainLod1Step, z1)) * terrainLod1Size * terrainLod1Step;
		terrainLod1Offset += newOffset;

		ComputeHeightMap(1);
	}
}

void Terrain::UpdateHeightMapVariables()
{
	heightMapVariables.terrainOffset = terrainOffset;
	heightMapVariables.terrainLod0Offset = terrainLod0Offset;
	heightMapVariables.terrainLod1Offset = terrainLod1Offset;
	memcpy(heightMapVariablesBuffers[Manager::currentFrame].mappedBuffer, &heightMapVariables, sizeof(heightMapVariables));
}

Pipeline Terrain::graphicsPipeline{Manager::currentDevice, Manager::currentCamera};
Pipeline Terrain::heightMapComputePipeline{Manager::currentDevice, Manager::currentCamera};
Pipeline Terrain::heightMapArrayComputePipeline{Manager::currentDevice, Manager::currentCamera};

Texture Terrain::grassDiffuseTexture{Manager::currentDevice};
//Texture Terrain::grassNormalTexture{Manager::currentDevice};
//Texture Terrain::grassSpecularTexture{Manager::currentDevice};

Texture Terrain::rockDiffuseTexture{Manager::currentDevice};
Texture Terrain::dirtDiffuseTexture{Manager::currentDevice};

//Texture Terrain::heightMapTexture{Manager::currentDevice};
Texture Terrain::heightMapArrayTexture{Manager::currentDevice};
Texture Terrain::heightMapLod0Texture{Manager::currentDevice};
Texture Terrain::heightMapLod1Texture{Manager::currentDevice};

HeightMapVariables Terrain::heightMapVariables;
std::vector<Buffer> Terrain::heightMapVariablesBuffers;

HeightMapComputeVariables Terrain::heightMapComputeVariables;
Buffer Terrain::heightMapComputeVariablesBuffer;

HeightMapArrayComputeVariables Terrain::heightMapArrayComputeVariables;
Buffer Terrain::heightMapArrayComputeVariablesBuffer;

Descriptor Terrain::graphicsDescriptor{Manager::currentDevice};
Descriptor Terrain::heightMapComputeDescriptor{Manager::currentDevice};
Descriptor Terrain::heightMapArrayComputeDescriptor{Manager::currentDevice};

Mesh Terrain::lod0Mesh{};
Mesh Terrain::lod1Mesh{};

//Object Terrain::object{&Terrain::mesh, &Terrain::graphicsPipeline};
std::vector<Object> Terrain::terrainChunks;

float Terrain::terrainChunkSize = 10000;
float Terrain::terrainLod0Size = 2500;
float Terrain::terrainLod1Size = 5000;

int Terrain::terrainChunkRadius = 1;
int Terrain::terrainChunkLength = Terrain::terrainChunkRadius * 2 + 1;
int Terrain::terrainChunkCount = Terrain::terrainChunkLength * Terrain::terrainChunkLength;

int Terrain::heightMapRadius = 1;
int Terrain::heightMapLength = Terrain::heightMapRadius * 2 + 1;
int Terrain::heightMapCount = Terrain::heightMapLength * Terrain::heightMapLength;

glm::vec2 Terrain::terrainOffset = glm::vec2(0);
glm::vec2 Terrain::terrainLod0Offset = glm::vec2(0);
glm::vec2 Terrain::terrainLod1Offset = glm::vec2(0);
float Terrain::terrainStep = 1.0f;
float Terrain::terrainLod0Step = 0.125f;
float Terrain::terrainLod1Step = 0.25f;

uint32_t Terrain::currentBoundHeightMap = -1;