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
	//CreateShadowPipeline();
	CreateComputePipelines();
	CreateTextures();
	CreateObjects();
	CreateBuffers();
	CreateGraphicsDescriptor();
	//CreateShadowDescriptor();
	CreateComputeDescriptors();
}

void Terrain::CreateTextures()
{
	SamplerConfiguration grassSamplerConfig;
	grassSamplerConfig.repeatMode = REPEAT;
	grassSamplerConfig.mipLodBias = 0.0f;

	grassDiffuseTexture.CreateTexture("rocky_grass_diff.jpg", grassSamplerConfig);

	rockDiffuseTexture.CreateTexture("rock_diff.jpg", grassSamplerConfig);

	dirtDiffuseTexture.CreateTexture("dirt_diff.jpg", grassSamplerConfig);

	SamplerConfiguration heightMapSamplerConfig;
	SamplerConfiguration heightMapArraySamplerConfig;

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
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(4);
	descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[0].count = terrainChunkCount;

	descriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[1].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[2].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[3].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[3].stages = FRAGMENT_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.tesselation = true;
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

    VertexInfo vertexInfo = lod0Mesh.MeshVertexInfo();

    graphicsPipeline.CreateGraphicsPipeline("terrain", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Terrain::CreateShadowPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[0].count = terrainChunkCount;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.shadow = true;
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = lod0Mesh.MeshVertexInfo();

	shadowPipeline.CreateGraphicsPipeline("terrainShadow", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
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
	std::vector<DescriptorConfiguration> descriptorConfig(4);

	int bufferCount = terrainChunks[0].uniformBuffers.size();
	int objectCount = terrainChunks.size();

	descriptorConfig[0].type = UNIFORM_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[0].count = objectCount;
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
	descriptorConfig[1].stages = FRAGMENT_STAGE;
	descriptorConfig[1].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[1].imageInfo.imageView = grassDiffuseTexture.imageView;
	descriptorConfig[1].imageInfo.sampler = grassDiffuseTexture.sampler;

	descriptorConfig[2].type = IMAGE_SAMPLER;
	descriptorConfig[2].stages = FRAGMENT_STAGE;
	descriptorConfig[2].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[2].imageInfo.imageView = rockDiffuseTexture.imageView;
	descriptorConfig[2].imageInfo.sampler = rockDiffuseTexture.sampler;

	descriptorConfig[3].type = IMAGE_SAMPLER;
	descriptorConfig[3].stages = FRAGMENT_STAGE;
	descriptorConfig[3].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[3].imageInfo.imageView = dirtDiffuseTexture.imageView;
	descriptorConfig[3].imageInfo.sampler = dirtDiffuseTexture.sampler;

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Terrain::CreateShadowDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	int bufferCount = terrainChunks[0].uniformBuffers.size();
	int objectCount = terrainChunks.size();

	descriptorConfig[0].type = UNIFORM_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[0].count = objectCount;
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

	shadowDescriptor.Create(descriptorConfig, shadowPipeline.objectDescriptorSetLayout);
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
	//shadowPipeline.Destroy();
	heightMapComputePipeline.Destroy();
	heightMapArrayComputePipeline.Destroy();
}

void Terrain::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	//shadowDescriptor.Destroy();
	heightMapComputeDescriptor.Destroy();
	heightMapArrayComputeDescriptor.Destroy();
}

void Terrain::DestroyBuffers()
{
	/*
	for (Buffer &buffer : heightMapVariablesBuffers)
	{
		buffer.Destroy();
	}

	heightMapVariablesBuffers.clear();
	*/

	heightMapComputeVariablesBuffer.Destroy();

	heightMapArrayComputeVariablesBuffer.Destroy();
}

void Terrain::Start()
{
	//heightMapVariables.terrainTotalSize = terrainTotalSize;
	//heightMapVariables.terrainTotalSizeMult = 1.0 / terrainTotalSize;
	//heightMapVariables.terrainChunksLength = heightMapLength;
	//heightMapVariables.terrainChunksLengthMult = 1.0 / float(heightMapLength);

	Manager::shaderVariables.terrainTotalSize = terrainTotalSize;
	Manager::shaderVariables.terrainTotalSizeMult = 1.0 / terrainTotalSize;
	Manager::shaderVariables.terrainChunksLength = heightMapLength;
	Manager::shaderVariables.terrainChunksLengthMult = 1.0 / float(heightMapLength);

	ComputeHeightMap(1);
	ComputeHeightMap(0);
}

void Terrain::Frame()
{
	if (Time::newFrameTick)
	{
		if (heightMapArrayLayersGenerated < heightMapCount)
		{
			ComputeHeightMapArray(heightMapArrayLayersGenerated);
			heightMapArrayLayersGenerated++;
			if (heightMapArrayLayersGenerated == heightMapCount)
				std::cout << "All height map array layers generated" << std::endl;
		}
	}

	//if (Time::newSubTick)
	//{
	//	CheckTerrainOffset();
	//}
}

void Terrain::PostFrame()
{
	if (Time::newSubTick)
	{
		CheckTerrainOffset();
	}
}

void Terrain::RecordCommands(VkCommandBuffer commandBuffer)
{
	graphicsPipeline.BindGraphics(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	RenderTerrain(commandBuffer);
}

void Terrain::RenderTerrain(VkCommandBuffer commandBuffer)
{
	int chunksRendered = 0;

	uint32_t chunkIndex = 0;

	std::vector<int> lod0Indices;
	std::vector<int> lod1Indices;

	float xs = Manager::camera.Position().x / float(terrainChunkSize);
	float zs = Manager::camera.Position().z / float(terrainChunkSize);

	for (int xi = -terrainChunkRadius; xi <= terrainChunkRadius; xi++)
	{
		for (int zi = -terrainChunkRadius; zi <= terrainChunkRadius; zi++)
		{
			int xIndex = xi + terrainChunkRadius;
			int zIndex = zi + terrainChunkRadius;
			int index = xIndex * terrainChunkLength + zIndex;
			float xf = float(xi);
			float zf = float(zi);

			float distance = glm::clamp(glm::distance(glm::vec2(xf, zf), glm::vec2(xs, zs)), 0.0f, float(terrainChunkLength));
			//bool inView = ChunkInView(terrainChunks[index].GetPosition(), 0, Manager::camera.Projection(), Manager::camera.View());
			bool inView = ChunkInView(terrainChunks[index].GetPosition(), 0, Manager::camera.Projection(), Manager::camera.View());

			if ((inView && distance < 1.0) || distance <= 0.75)
			{
				lod0Indices.push_back(index);
				terrainChunks[index].ModifyPosition().y = 0;
				terrainChunks[index].UpdateUniformBuffer(Manager::currentFrame);
			}
			else if (inView)
			{
				float factor = pow(1.0 - (distance - 1.0) / float(terrainChunkLength - 1), 0.5);

				lod1Indices.push_back(index);
				terrainChunks[index].ModifyPosition().y = factor * -10.0;
				terrainChunks[index].UpdateUniformBuffer(Manager::currentFrame);
			}
		}
	}

	lod0Mesh.Bind(commandBuffer);

	for (int index : lod0Indices)
	{
		chunkIndex = index;
		vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE | TESSELATION_CONTROL_STAGE | 
			TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE, 0, sizeof(chunkIndex), &chunkIndex);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lod0Mesh.indices.size()), 1, 0, 0, 0);
	}

	lod1Mesh.Bind(commandBuffer);

	for (int index : lod1Indices)
	{
		chunkIndex = index;
		vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE, 0, sizeof(chunkIndex), &chunkIndex);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lod1Mesh.indices.size()), 1, 0, 0, 0);
	}

	//if (Time::newSecond)
	//{
	//	std::cout << "Lod 0 chunks rendered: " << lod0Indices.size() << std::endl;
	//	std::cout << "Lod 1 chunks rendered: " << lod1Indices.size() << std::endl << std::endl;
	//}
}

void Terrain::ComputeHeightMap(uint32_t lod)
{
	//std::cout << "Lod: " << lod << std::endl;
	//float startTime = Time::GetCurrentTime();
	//std::cout << "Start time: " << startTime << std::endl;

	//std::cout << "computing heightmap lod: " << lod << std::endl;

	vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();
	heightMapComputePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, heightMapComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	//Manager::UpdateShaderVariables();

	if (currentBoundHeightMap != lod)
	{
		heightMapComputeDescriptor.descriptorConfigs[0].imageInfo.imageView = (lod == 0 ? heightMapLod0Texture.imageView : heightMapLod1Texture.imageView);
		heightMapComputeDescriptor.Update();
	}

	heightMapComputeDescriptor.Bind(commandBuffer, heightMapComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	heightMapComputeVariables.mapScale = (lod == 0 ? terrainLod0Size : (lod == 1 ? terrainLod1Size : terrainChunkSize)) / terrainChunkSize;
	heightMapComputeVariables.mapOffset = (lod == 0 ? ((terrainLod0Offset + terrainOffset) / terrainLod0Size) : ((terrainLod1Offset + terrainOffset) / terrainLod1Size));
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

	//std::cout << "computing heightmap array index: " << index << std::endl;

	int x = (index % heightMapLength) - heightMapRadius;
	int y = (index / heightMapLength) - heightMapRadius;

	//vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();
	heightMapArrayComputePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, heightMapArrayComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	//Manager::UpdateShaderVariables();

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
	float xw = Manager::camera.Position().x;
	float zw = Manager::camera.Position().z;

	float x0 = xw - terrainLod0Offset.x;
	float z0 = zw - terrainLod0Offset.y;
	float x1 = xw - terrainLod1Offset.x;
	float z1 = zw - terrainLod1Offset.y;

	if (abs(xw) >= terrainChunkSize * terrainStep || abs(zw) >= terrainChunkSize * terrainStep)
	{
		glm::vec2 newOffset = glm::vec2(Utilities::Fits(terrainChunkSize * terrainStep, xw), Utilities::Fits(terrainChunkSize * terrainStep, zw)) * terrainChunkSize * terrainStep;
		terrainOffset += newOffset;

		terrainLod0Offset = glm::vec2(0);
		terrainLod1Offset = glm::vec2(0);

		Manager::camera.Move(-glm::vec3(newOffset.x, 0, newOffset.y));
	}
	else if (abs(x1) >= terrainLod1Size * terrainLod1Step || abs(z1) >= terrainLod1Size * terrainLod1Step)
	{
		glm::vec2 newOffset = glm::vec2(Utilities::Fits(terrainLod1Size * terrainLod1Step, x1), Utilities::Fits(terrainLod1Size * terrainLod1Step, z1)) * terrainLod1Size * terrainLod1Step;
		terrainLod1Offset += newOffset;

		ComputeHeightMap(1);
	}
	else if (abs(x0) >= terrainLod0Size * terrainLod0Step || abs(z0) >= terrainLod0Size * terrainLod0Step)
	{
		glm::vec2 newOffset = glm::vec2(Utilities::Fits(terrainLod0Size * terrainLod0Step, x0), Utilities::Fits(terrainLod0Size * terrainLod0Step, z0)) * terrainLod0Size * terrainLod0Step;
		terrainLod0Offset += newOffset;

		ComputeHeightMap(0);
	}
}

/*
void Terrain::UpdateHeightMapVariables()
{
	heightMapVariables.terrainOffset = terrainOffset;
	heightMapVariables.terrainLod0Offset = terrainLod0Offset;
	heightMapVariables.terrainLod1Offset = terrainLod1Offset;

	memcpy(heightMapVariablesBuffers[Manager::currentFrame].mappedBuffer, &heightMapVariables, sizeof(heightMapVariables));
}
*/

bool Terrain::InView(const glm::vec3 &position, float tolerance, const glm::mat4 &projection, const glm::mat4 &view)
{
	glm::vec4 viewSpace = projection * view * glm::vec4(position, 1.0);

	glm::vec3 clipSpace = viewSpace;
	clipSpace /= -viewSpace.w;

	clipSpace.x = clipSpace.x * 0.5f + 0.5f;
	clipSpace.y = clipSpace.y * 0.5f + 0.5f;
	clipSpace.z = viewSpace.w;

	if (clipSpace.z <= 0.0 || clipSpace.z >= Manager::camera.far) return false;

	return !(clipSpace.x < -tolerance || clipSpace.x > 1.0f + tolerance || clipSpace.y < -tolerance || clipSpace.y > 1.0f + tolerance);
}

bool Terrain::ChunkInView(glm::vec3 position, float tolerance, glm::mat4 projection, glm::mat4 view, bool main)
{
	if (InView(position, tolerance, projection, view)) return true;
	else if (InView(position + glm::vec3(Terrain::terrainChunkSize * 0.5, 0, Terrain::terrainChunkSize * 0.5), 
		tolerance, projection, view)) return true;
	else if (InView(position + glm::vec3(Terrain::terrainChunkSize * -0.5, 0, Terrain::terrainChunkSize * -0.5), 
		tolerance, projection, view)) return true;
	else if (InView(position + glm::vec3(Terrain::terrainChunkSize * -0.5, 0, Terrain::terrainChunkSize * 0.5), 
		tolerance, projection, view)) return true;
	else if (InView(position + glm::vec3(Terrain::terrainChunkSize * 0.5, 0, Terrain::terrainChunkSize * -0.5), 
		tolerance, projection, view)) return true;
	else if (main) return ChunkInView(position + glm::vec3(0, Terrain::terrainHeight * 0.5, 0), tolerance, projection, view, false);
	return false;
}

bool Terrain::HeightMapsGenerated()
{
	return (heightMapArrayLayersGenerated == heightMapCount);
}

Pipeline Terrain::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Terrain::shadowPipeline{Manager::currentDevice, Manager::camera};
Pipeline Terrain::heightMapComputePipeline{Manager::currentDevice, Manager::camera};
Pipeline Terrain::heightMapArrayComputePipeline{Manager::currentDevice, Manager::camera};

Texture Terrain::grassDiffuseTexture{Manager::currentDevice};
Texture Terrain::rockDiffuseTexture{Manager::currentDevice};
Texture Terrain::dirtDiffuseTexture{Manager::currentDevice};

Texture Terrain::heightMapArrayTexture{Manager::currentDevice};
Texture Terrain::heightMapLod0Texture{Manager::currentDevice};
Texture Terrain::heightMapLod1Texture{Manager::currentDevice};

HeightMapComputeVariables Terrain::heightMapComputeVariables;
Buffer Terrain::heightMapComputeVariablesBuffer;

HeightMapArrayComputeVariables Terrain::heightMapArrayComputeVariables;
Buffer Terrain::heightMapArrayComputeVariablesBuffer;

Descriptor Terrain::graphicsDescriptor{Manager::currentDevice};
Descriptor Terrain::shadowDescriptor{Manager::currentDevice};
Descriptor Terrain::heightMapComputeDescriptor{Manager::currentDevice};
Descriptor Terrain::heightMapArrayComputeDescriptor{Manager::currentDevice};

Mesh Terrain::lod0Mesh{};
Mesh Terrain::lod1Mesh{};

std::vector<Object> Terrain::terrainChunks;

float Terrain::terrainChunkSize = 10000;
float Terrain::terrainLod0Size = 2500;
float Terrain::terrainLod1Size = 5000;

int Terrain::terrainChunkRadius = 3;
int Terrain::terrainChunkLength = Terrain::terrainChunkRadius * 2 + 1;
int Terrain::terrainChunkCount = Terrain::terrainChunkLength * Terrain::terrainChunkLength;

int Terrain::heightMapRadius = 4;
int Terrain::heightMapLength = Terrain::heightMapRadius * 2 + 1;
int Terrain::heightMapCount = Terrain::heightMapLength * Terrain::heightMapLength;

float Terrain::terrainTotalSize = Terrain::heightMapLength * Terrain::terrainChunkSize;
float Terrain::terrainHeight = 5000;

glm::vec2 Terrain::terrainOffset = glm::vec2(0);
glm::vec2 Terrain::terrainLod0Offset = glm::vec2(0);
glm::vec2 Terrain::terrainLod1Offset = glm::vec2(0);
float Terrain::terrainStep = 1.0f;
float Terrain::terrainLod0Step = 0.125f;
float Terrain::terrainLod1Step = 0.25f;

uint32_t Terrain::currentBoundHeightMap = -1;

int Terrain::heightMapArrayLayersGenerated = 0;

//bool Terrain::terrainLod0OffsetReset = false;
//bool Terrain::terrainLod1OffsetReset = false;