#include "terrain.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "time.hpp"
#include "ui.hpp"

#include <stdexcept>
#include <iostream>

void Terrain::Create()
{
	terrainShadowOffsets.resize(TERRAIN_SHADOW_CASCADES);
	shadowComputeVariables.resize(TERRAIN_SHADOW_CASCADES);

	shadowComputeVariables[0].lod = 0;
	shadowComputeVariables[1].lod = 1;
	shadowComputeVariables[2].lod = 2;

	shadowComputeVariables[0].distance = 2048;
	shadowComputeVariables[1].distance = 8192;
	shadowComputeVariables[2].distance = 32768;

	shadowComputeVariables[0].resolution = 64;
	shadowComputeVariables[1].resolution = 64;
	shadowComputeVariables[2].resolution = 128;

	shadowComputeVariables[0].resolutionMultiplier = 1.0 / float(shadowComputeVariables[0].resolution);
	shadowComputeVariables[1].resolutionMultiplier = 1.0 / float(shadowComputeVariables[1].resolution);
	shadowComputeVariables[2].resolutionMultiplier = 1.0 / float(shadowComputeVariables[2].resolution);

	shadowComputeVariables[0].spacing = shadowComputeVariables[0].resolutionMultiplier * shadowComputeVariables[0].distance;
	shadowComputeVariables[1].spacing = shadowComputeVariables[1].resolutionMultiplier * shadowComputeVariables[1].distance;
	shadowComputeVariables[2].spacing = shadowComputeVariables[2].resolutionMultiplier * shadowComputeVariables[2].distance;

	//Manager::shaderVariables.terrainShadowDistances[0] = shadowComputeVariables.terrainShadowDistances[0];

	CreateMeshes();
	CreateGraphicsPipeline();
	if (Manager::settings.occlussionCulling) CreateCullPipeline();
	CreateComputePipelines();
	CreateShadowPipeline();
	CreateTextures();
	CreateObjects();
	CreateBuffers();
	CreateGraphicsDescriptor();
	if (Manager::settings.occlussionCulling) CreateCullDescriptor();
	CreateComputeDescriptors();
	CreateShadowDescriptor();
}

void Terrain::CreateTextures()
{
	SamplerConfiguration grassSamplerConfig;
	grassSamplerConfig.repeatMode = REPEAT;
	grassSamplerConfig.mipLodBias = 0.0f;
	grassSamplerConfig.anisotrophic = VK_TRUE;
	grassSamplerConfig.anisotrophicSampleCount = 4.0;
	grassSamplerConfig.mipLodBias = 0.0f;

	//grassDiffuseTexture.CreateTexture("rocky_grass_diff.jpg", grassSamplerConfig);
	grassTextures.resize(3);
	grassTextures[0].CreateTexture("leafy_grass_diff.jpg", grassSamplerConfig);
	grassTextures[1].CreateTexture("leafy_grass_norm.jpg", grassSamplerConfig);
	grassTextures[2].CreateTexture("rocky_grass_ao.jpg", grassSamplerConfig);

	//rockDiffuseTexture.CreateTexture("rock_diff.jpg", grassSamplerConfig);
	rockTextures.resize(3);
	rockTextures[0].CreateTexture("large_rock_diff.jpg", grassSamplerConfig);
	rockTextures[1].CreateTexture("large_rock_norm.jpg", grassSamplerConfig);
	rockTextures[2].CreateTexture("rock_ao.jpg", grassSamplerConfig);

	//dirtDiffuseTexture.CreateTexture("dirt_diff.jpg", grassSamplerConfig);
	dirtTextures.resize(3);
	dirtTextures[0].CreateTexture("rocky_dirt_diff.jpg", grassSamplerConfig);
	dirtTextures[1].CreateTexture("rocky_dirt_norm.jpg", grassSamplerConfig);
	dirtTextures[2].CreateTexture("rocky_dirt_ao.jpg", grassSamplerConfig);

	SamplerConfiguration heightMapSamplerConfig;
	//heightMapSamplerConfig.anisotrophic = VK_TRUE;
	//heightMapSamplerConfig.anisotrophicSampleCount = 2.0;
	//heightMapSamplerConfig.minFilter = VK_FILTER_NEAREST;
	//heightMapSamplerConfig.magFilter = VK_FILTER_NEAREST;
	SamplerConfiguration heightMapArraySamplerConfig;

	ImageConfiguration heightMapArrayConfig = Texture::ImageArrayStorage(heightMapResolution, heightMapResolution, heightMapCount);
	heightMapArrayTexture.CreateImage(heightMapArrayConfig, heightMapArraySamplerConfig);
	heightMapArrayTexture.TransitionImageLayout(heightMapArrayConfig);

	ImageConfiguration heightMapLod1Config = Texture::ImageStorage(heightMapLod1Resolution, heightMapLod1Resolution);
	heightMapLod1Config.format = R32;
	heightMapLod1Texture.CreateImage(heightMapLod1Config, heightMapSamplerConfig);
	heightMapLod1Texture.TransitionImageLayout(heightMapLod1Config);

	ImageConfiguration heightMapLod0Config = Texture::ImageStorage(heightMapLod0Resolution, heightMapLod0Resolution);
	heightMapLod0Config.format = R32;
	heightMapLod0Texture.CreateImage(heightMapLod0Config, heightMapSamplerConfig);
	heightMapLod0Texture.TransitionImageLayout(heightMapLod0Config);

	SamplerConfiguration terrainShadowSamplerConfig;
	terrainShadowTextures.resize(TERRAIN_SHADOW_CASCADES);

	for (int i = 0; i < TERRAIN_SHADOW_CASCADES; i++)
	{
		ImageConfiguration terrainShadowConfig = Texture::ImageStorage(shadowComputeVariables[i].resolution, 
			shadowComputeVariables[i].resolution);
		terrainShadowTextures[i].CreateImage(terrainShadowConfig, terrainShadowSamplerConfig);
		terrainShadowTextures[i].TransitionImageLayout(terrainShadowConfig);
	}
}

void Terrain::CreateMeshes()
{
    lod0Mesh.shape.SetShape(PLANE, 100);
	lod0Mesh.RecalculateVertices();
	lod0Mesh.Create();

	lod1Mesh.shape.SetShape(PLANE, 10);
	lod1Mesh.RecalculateVertices();
	lod1Mesh.Create();

	lod2Mesh.shape.SetShape(PLANE, 25);
	lod2Mesh.RecalculateVertices();
	lod2Mesh.Create();
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
			terrainChunks[i].Resize(glm::vec3(terrainMeshSize));
			terrainChunks[i].Move(glm::vec3(x, 0, y) * terrainMeshSize);
		}
	}
}

void Terrain::CreateGraphicsPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(4);
	descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE | TESSELATION_EVALUATION_STAGE;
	descriptorLayoutConfig[0].count = terrainChunkCount;
	descriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[1].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[1].count = 3;
	descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[2].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[2].count = 3;
	descriptorLayoutConfig[3].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[3].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[3].count = 3;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.tesselation = true;
	pipelineConfiguration.pushConstantCount = 2;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE | TESSELATION_EVALUATION_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

    VertexInfo vertexInfo = lod0Mesh.MeshVertexInfo();

    graphicsPipeline.CreateGraphicsPipeline("terrain", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);

	PipelineConfiguration lodPipelineConfiguration = Pipeline::DefaultConfiguration();
	//lodPipelineConfiguration.tesselation = true;
	lodPipelineConfiguration.pushConstantCount = 1;
	lodPipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	lodPipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo lodVertexInfo = lod2Mesh.MeshVertexInfo();

	graphicsLodPipeline.CreateGraphicsPipeline("terrainLod", descriptorLayoutConfig, lodPipelineConfiguration, lodVertexInfo);
}

void Terrain::CreateCullPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE | TESSELATION_EVALUATION_STAGE;
	descriptorLayoutConfig[0].count = terrainChunkCount;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.cull = true;
	pipelineConfiguration.tesselation = true;
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE | TESSELATION_EVALUATION_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = lod0Mesh.MeshVertexInfo();

	cullPipeline.CreateGraphicsPipeline("terrainCull", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
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

void Terrain::CreateShadowPipeline()
{
	std::vector<DescriptorLayoutConfiguration> shadowDescriptorLayoutConfig(2);
	shadowDescriptorLayoutConfig[0].type = IMAGE_STORAGE;
	shadowDescriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	shadowDescriptorLayoutConfig[0].count = TERRAIN_SHADOW_CASCADES;
	shadowDescriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	shadowDescriptorLayoutConfig[1].stages = COMPUTE_STAGE;

	shadowComputePipeline.CreateComputePipeline("terrainShadowCompute", shadowDescriptorLayoutConfig);
}

void Terrain::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(4);

	int bufferCount = terrainChunks[0].uniformBuffers.size();
	int objectCount = terrainChunks.size();

	descriptorConfig[0].type = UNIFORM_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE | TESSELATION_EVALUATION_STAGE;
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
	descriptorConfig[1].count = 3;
	descriptorConfig[1].imageInfos.resize(3);
	for (int i = 0; i < grassTextures.size(); i++) 
	{
		descriptorConfig[1].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[1].imageInfos[i].imageView = grassTextures[i].imageView;
		descriptorConfig[1].imageInfos[i].sampler = grassTextures[i].sampler;
	}
	//descriptorConfig[1].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	//descriptorConfig[1].imageInfo.imageView = grassDiffuseTexture.imageView;
	//descriptorConfig[1].imageInfo.sampler = grassDiffuseTexture.sampler;

	descriptorConfig[2].type = IMAGE_SAMPLER;
	descriptorConfig[2].stages = FRAGMENT_STAGE;
	descriptorConfig[2].count = 3;
	descriptorConfig[2].imageInfos.resize(3);
	for (int i = 0; i < rockTextures.size(); i++) 
	{
		descriptorConfig[2].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[2].imageInfos[i].imageView = rockTextures[i].imageView;
		descriptorConfig[2].imageInfos[i].sampler = rockTextures[i].sampler;
	}
	//descriptorConfig[2].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	//descriptorConfig[2].imageInfo.imageView = rockDiffuseTexture.imageView;
	//descriptorConfig[2].imageInfo.sampler = rockDiffuseTexture.sampler;

	descriptorConfig[3].type = IMAGE_SAMPLER;
	descriptorConfig[3].stages = FRAGMENT_STAGE;
	descriptorConfig[3].count = 3;
	descriptorConfig[3].imageInfos.resize(3);
	for (int i = 0; i < dirtTextures.size(); i++) 
	{
		descriptorConfig[3].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[3].imageInfos[i].imageView = dirtTextures[i].imageView;
		descriptorConfig[3].imageInfos[i].sampler = dirtTextures[i].sampler;
	}
	//descriptorConfig[3].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	//descriptorConfig[3].imageInfo.imageView = dirtDiffuseTexture.imageView;
	//descriptorConfig[3].imageInfo.sampler = dirtDiffuseTexture.sampler;

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Terrain::CreateCullDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(1);

	int bufferCount = terrainChunks[0].uniformBuffers.size();
	int objectCount = terrainChunks.size();

	descriptorConfig[0].type = UNIFORM_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE | TESSELATION_EVALUATION_STAGE;
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

	cullDescriptor.Create(descriptorConfig, cullPipeline.objectDescriptorSetLayout);
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

void Terrain::CreateShadowDescriptor()
{
	std::vector<DescriptorConfiguration> shadowDescriptorConfig(2);

	shadowDescriptorConfig[0].type = IMAGE_STORAGE;
	shadowDescriptorConfig[0].stages = COMPUTE_STAGE;
	shadowDescriptorConfig[0].count = TERRAIN_SHADOW_CASCADES;
	shadowDescriptorConfig[0].imageInfos.resize(TERRAIN_SHADOW_CASCADES);
	for (int i = 0; i < TERRAIN_SHADOW_CASCADES; i++)
	{
		shadowDescriptorConfig[0].imageInfos[i].imageLayout = LAYOUT_GENERAL;
		shadowDescriptorConfig[0].imageInfos[i].imageView = terrainShadowTextures[i].imageView;
		//shadowDescriptorConfig[0].imageInfos[i].sampler = terrainShadowTextures[i].sampler;
	}

	//shadowDescriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	//shadowDescriptorConfig[0].imageInfo.imageView = terrainShadowTextures[0].imageView;
	// shadowDescriptorConfig[0].imageInfo.sampler = heightMapTexture.sampler;

	shadowDescriptorConfig[1].type = UNIFORM_BUFFER;
	shadowDescriptorConfig[1].stages = COMPUTE_STAGE;
	shadowDescriptorConfig[1].buffersInfo.resize(1);
	shadowDescriptorConfig[1].buffersInfo[0].buffer = shadowComputeVariablesBuffer.buffer;
	shadowDescriptorConfig[1].buffersInfo[0].range = sizeof(ShadowComputeVariables);
	shadowDescriptorConfig[1].buffersInfo[0].offset = 0;

	shadowComputeDescriptor.perFrame = false;
	shadowComputeDescriptor.Create(shadowDescriptorConfig, shadowComputePipeline.objectDescriptorSetLayout);
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

	BufferConfiguration shadowComputeConfiguration;
	shadowComputeConfiguration.size = sizeof(ShadowComputeVariables);
	shadowComputeConfiguration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	shadowComputeConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	shadowComputeConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	shadowComputeConfiguration.mapped = true;
	shadowComputeVariablesBuffer.Create(shadowComputeConfiguration);
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
	for (Texture &texture : grassTextures)
	{
		texture.Destroy();
	}
	grassTextures.clear();

	for (Texture &texture : rockTextures)
	{
		texture.Destroy();
	}
	rockTextures.clear();

	for (Texture &texture : dirtTextures)
	{
		texture.Destroy();
	}
	dirtTextures.clear();

	//grassDiffuseTexture.Destroy();
	//rockDiffuseTexture.Destroy();
	//dirtDiffuseTexture.Destroy();

	heightMapArrayTexture.Destroy();
	heightMapLod1Texture.Destroy();
	heightMapLod0Texture.Destroy();

	//terrainShadowTexture.Destroy();
	for (Texture &texture : terrainShadowTextures)
	{
		texture.Destroy();
	}
	terrainShadowTextures.clear();
}

void Terrain::DestroyMeshes()
{
	lod0Mesh.Destroy();
	lod1Mesh.Destroy();
	lod2Mesh.Destroy();
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
	graphicsLodPipeline.Destroy();
	cullPipeline.Destroy();
	heightMapComputePipeline.Destroy();
	heightMapArrayComputePipeline.Destroy();
	shadowComputePipeline.Destroy();
}

void Terrain::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	cullDescriptor.Destroy();
	heightMapComputeDescriptor.Destroy();
	heightMapArrayComputeDescriptor.Destroy();
	shadowComputeDescriptor.Destroy();
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

	shadowComputeVariablesBuffer.Destroy();
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
	Manager::shaderVariables.terrainHeight = terrainHeight;
	Manager::shaderVariables.waterHeight = glm::vec2(waterHeight, waterBlendDistance);

	for (Object &chunk : terrainChunks)
	{
		for (int i = 0; i < Manager::settings.maxFramesInFlight; i++)
		{
			chunk.ModifyPosition().y = 0;
			chunk.UpdateUniformBuffer(i);
		}
	}

	//ComputeHeightMap(nullptr, 1);
	//ComputeHeightMap(nullptr, 0); 

	Menu &menu = UI::NewMenu("terrain");
	menu.AddNode("water", true);
	menu.AddSlider("water height", waterHeight, 0.0f, 2000.0f);
	menu.AddNode("water", false);
	menu.AddNode("biomes", true);
	menu.AddText("test text");
	menu.AddGraph("low lands", 32, lowLandsCurve);
	menu.AddNode("biomes", false);
}

void Terrain::Frame()
{
	if (Time::newFrameTick)
	{
		if (heightMapArrayLayersGenerated < heightMapCount)
		{
			ComputeHeightMapArray(nullptr, heightMapArrayLayersGenerated);
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
	if (!HeightMapsGenerated())
		return;

	//float yw = Manager::camera.Position().y;
	//if (abs(yw) >= terrainHeight * terrainStep)
	//{
	//	glm::vec3 newOffset = glm::vec3(0, -yw, 0);
	//	terrainOffset += newOffset;
	//	Manager::camera.Move(newOffset);
	//	Manager::UpdateShaderVariables();
	//}

	//if (Time::newSubTick)
	{
		CheckTerrainOffset(nullptr);
	}

	//if (Time::newTick)
	{
		CheckTerrainShadowOffset();
	}
}

void Terrain::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
	//graphicsPipeline.BindGraphics(commandBuffer);
	//Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	//graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	RenderTerrain(commandBuffer);
}

void Terrain::RecordComputeCommands(VkCommandBuffer commandBuffer)
{
	if (Time::newFrameTick)
	{
		if (heightMapArrayLayersGenerated < heightMapCount)
		{
			ComputeHeightMapArray(commandBuffer, heightMapArrayLayersGenerated);
			heightMapArrayLayersGenerated++;
			if (heightMapArrayLayersGenerated == heightMapCount)
				std::cout << "All height map array layers generated" << std::endl;
		}
	}

	if (Time::newSubTick)
	{
		CheckTerrainOffset(commandBuffer);
	}
}

void Terrain::RecordCullCommands(VkCommandBuffer commandBuffer)
{
	cullPipeline.BindGraphics(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, cullPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	cullDescriptor.Bind(commandBuffer, cullPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	RenderCulling(commandBuffer);
}

void Terrain::RenderTerrain(VkCommandBuffer commandBuffer)
{
	int chunksRendered = 0;

	uint32_t chunkIndex = 0;

	std::vector<int> lod0Indices;
	std::vector<int> lod1Indices;
	std::vector<int> lod2Indices;

	float xs = Manager::camera.Position().x / float(terrainMeshSize);
	float zs = Manager::camera.Position().z / float(terrainMeshSize);

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
			//bool inView = ChunkInView(terrainChunks[index].GetPosition(), 0, Manager::camera.Projection(), Manager::camera.ViewOffset());
			bool inView = ChunkInView(terrainChunks[index].GetPosition(), 0, Manager::camera.Projection(), Manager::camera.ViewOffset(), true, false);
			//bool inLodView = ChunkInView(terrainChunks[index].GetPosition(), 0, Manager::camera.ProjectionLod(), Manager::camera.ViewOffset(), true, true);

			//if ((inView && distance < 1.0) || distance <= 0.75)
			/*if (distance <= 0.75)
			{
				lod0Indices.push_back(index);
				//terrainChunks[index].ModifyPosition().y = 0;
				//terrainChunks[index].UpdateUniformBuffer(Manager::currentFrame);
			}
			else if (inView)
			{
				//float factor = pow(1.0 - (distance - 1.0) / float(terrainChunkLength - 1), 0.5);
				float factor = (distance - 0.75) / 0.75;
				//float factor = 0;

				lod2Indices.push_back(index);
				//terrainChunks[index].ModifyPosition().y = factor * -50.0;
				//terrainChunks[index].ModifyPosition().y = 0;
				//terrainChunks[index].UpdateUniformBuffer(Manager::currentFrame);
			}*/
			//else if (inView)
			//{
			//	//float factor = (distance - 1.0) / 2.0;
			//	//float factor = (distance - 1.5) / (float(terrainChunkLength) - 1.5);
			//	float factor = 0;
			//	//float lodFactor = 0;
			//	lod2Indices.push_back(index);
			//	//terrainChunks[index].ModifyPosition().y = 0;
			//	//terrainChunks[index].UpdateUniformBuffer(Manager::currentFrame);
			//}

			if (distance <= 0.75)
			{
				lod0Indices.push_back(index);
				//if (terrainOffset.y < -5000.0) lod2Indices.push_back(index);
				//terrainChunks[index].ModifyPosition().y = -250.0;
				//terrainChunks[index].UpdateUniformBuffer(Manager::currentFrame);
			}
			else if (inView)
			{
				lod1Indices.push_back(index);
				//lod2Indices.push_back(index);
			}
			//else if (inView)
			//{
			//	lod2Indices.push_back(index);
			//}
		}
	}

	graphicsPipeline.BindGraphics(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	lod0Mesh.Bind(commandBuffer);
	uint32_t pushLod = 0;
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE | TESSELATION_EVALUATION_STAGE, sizeof(uint32_t), sizeof(pushLod), &pushLod);

	for (int index : lod0Indices)
	{
		chunkIndex = index;
		vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE | TESSELATION_EVALUATION_STAGE, 0, sizeof(chunkIndex), &chunkIndex);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lod0Mesh.indices.size()), 1, 0, 0, 0);
	}

	lod1Mesh.Bind(commandBuffer);
	pushLod = 1;
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE | TESSELATION_EVALUATION_STAGE, sizeof(uint32_t), sizeof(pushLod), &pushLod);

	for (int index : lod1Indices)
	{
		chunkIndex = index;
		vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE | TESSELATION_EVALUATION_STAGE, 0, sizeof(chunkIndex), &chunkIndex);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lod1Mesh.indices.size()), 1, 0, 0, 0);
	}

	/*graphicsLodPipeline.BindGraphics(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, graphicsLodPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsLodPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	lod2Mesh.Bind(commandBuffer);

	for (int index : lod2Indices)
	{
		chunkIndex = index;
		vkCmdPushConstants(commandBuffer, graphicsLodPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(chunkIndex), &chunkIndex);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lod2Mesh.indices.size()), 1, 0, 0, 0);
	}*/
}

void Terrain::RenderCulling(VkCommandBuffer commandBuffer)
{
	int chunksRendered = 0;

	uint32_t chunkIndex = 0;

	std::vector<int> lod0Indices;
	//std::vector<int> lod1Indices;

	float xs = Manager::camera.Position().x / float(terrainMeshSize);
	float zs = Manager::camera.Position().z / float(terrainMeshSize);

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
			// bool inView = ChunkInView(terrainChunks[index].GetPosition(), 0, Manager::camera.Projection(), Manager::camera.ViewOffset());
			//bool inView = ChunkInView(terrainChunks[index].GetPosition(), 0, Manager::camera.Projection(), Manager::camera.ViewOffset());

			if (distance <= 0.75)
			{
				lod0Indices.push_back(index);
				//terrainChunks[index].ModifyPosition().y = 0;
				//terrainChunks[index].UpdateUniformBuffer(Manager::currentFrame);
			}
			/*else if (inView)
			{
				float factor = pow(1.0 - (distance - 1.0) / float(terrainChunkLength - 1), 0.5);

				lod1Indices.push_back(index);
				terrainChunks[index].ModifyPosition().y = factor * -10.0;
				terrainChunks[index].UpdateUniformBuffer(Manager::currentFrame);
			}*/
		}
	}

	lod0Mesh.Bind(commandBuffer);

	for (int index : lod0Indices)
	{
		chunkIndex = index;
		vkCmdPushConstants(commandBuffer, cullPipeline.graphicsPipelineLayout, VERTEX_STAGE | TESSELATION_EVALUATION_STAGE, 0, sizeof(chunkIndex), &chunkIndex);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lod0Mesh.indices.size()), 1, 0, 0, 0);
	}

	/*lod1Mesh.Bind(commandBuffer);

	for (int index : lod1Indices)
	{
		chunkIndex = index;
		vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE, 0, sizeof(chunkIndex), &chunkIndex);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lod1Mesh.indices.size()), 1, 0, 0, 0);
	}*/
}

void Terrain::ComputeHeightMap(VkCommandBuffer commandBuffer, uint32_t lod)
{
	vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);
	int dispatchCount = int(floor(float(lod == 0 ? heightMapLod0Resolution : heightMapLod1Resolution) / 8.0f));
	// VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	bool oneTimeBuffer = commandBuffer == nullptr;
	if (oneTimeBuffer) commandBuffer = Manager::currentDevice.BeginComputeCommand();
	heightMapComputePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, heightMapComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);

	if (currentBoundHeightMap != lod)
	{
		heightMapComputeDescriptor.descriptorConfigs[0].imageInfo.imageView = (lod == 0 ? heightMapLod0Texture.imageView : heightMapLod1Texture.imageView);
		heightMapComputeDescriptor.Update();
	}

	heightMapComputeDescriptor.Bind(commandBuffer, heightMapComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	heightMapComputeVariables.mapScale = (lod == 0 ? terrainLod0Size : (lod == 1 ? terrainLod1Size : terrainChunkSize)) / terrainChunkSize;
	heightMapComputeVariables.mapOffset = (lod == 0 ? ((terrainLod0Offset + XZ2XY(terrainOffset)) / terrainLod0Size) : ((terrainLod1Offset + XZ2XY(terrainOffset)) / terrainLod1Size));
	memcpy(heightMapComputeVariablesBuffer.mappedBuffer, &heightMapComputeVariables, sizeof(heightMapComputeVariables));
	vkCmdDispatch(commandBuffer, dispatchCount, dispatchCount, 1); //Change dispatch count
	if (oneTimeBuffer) Manager::currentDevice.EndComputeCommand(commandBuffer);

	currentBoundHeightMap = lod;
}

void Terrain::ComputeHeightMapArray(VkCommandBuffer commandBuffer, uint32_t index)
{
	int x = (index % heightMapLength) - heightMapRadius;
	int y = (index / heightMapLength) - heightMapRadius;
	int dispatchCount = int(floor(float(heightMapResolution) / 8.0f));

	//vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);

	//VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();
	bool oneTimeBuffer = commandBuffer == nullptr;
	if (oneTimeBuffer) commandBuffer = Manager::currentDevice.BeginComputeCommand();
	heightMapArrayComputePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, heightMapArrayComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	//Manager::UpdateShaderVariables();

	heightMapArrayComputeDescriptor.Bind(commandBuffer, heightMapArrayComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	heightMapArrayComputeVariables.mapScale = 1.0;
	heightMapArrayComputeVariables.currentChunkIndex = index;
	heightMapArrayComputeVariables.mapOffset = glm::vec2(y, x);
	memcpy(heightMapArrayComputeVariablesBuffer.mappedBuffer, &heightMapArrayComputeVariables, sizeof(heightMapArrayComputeVariables));
	vkCmdDispatch(commandBuffer, dispatchCount, dispatchCount, 1); // Change dispatch count
	if (oneTimeBuffer) Manager::currentDevice.EndComputeCommand(commandBuffer);
}

void Terrain::ComputeShadows(uint32_t index)
{
	vkQueueWaitIdle(Manager::currentDevice.graphicsQueue);
	int dispatchCount = int(floor(float(shadowComputeVariables[index].resolution) / 8.0));
	//shadowComputeVariables.index = index;
	
	//terrainShadowOffsets[index] = newOffset;
	//int dispatchCount = 128;

	//Manager::UpdateShaderVariables();

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	shadowComputePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, shadowComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	shadowComputeDescriptor.Bind(commandBuffer, shadowComputePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	memcpy(shadowComputeVariablesBuffer.mappedBuffer, &shadowComputeVariables[index], sizeof(ShadowComputeVariables));

	vkCmdDispatch(commandBuffer, dispatchCount, dispatchCount, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);

	//terrainShadowOffsets[index] = newOffset;

	//Manager::UpdateShaderVariables();
}

int ShouldUpdateShadows(glm::vec2 viewPosition)
{
	for (int i = TERRAIN_SHADOW_CASCADES - 1; i >= 0; i--)
	{
		if (abs(viewPosition.x - Terrain::terrainShadowOffsets[i].x) > Terrain::shadowComputeVariables[i].distance * (0.075 * (i + 1)) ||
			abs(viewPosition.y - Terrain::terrainShadowOffsets[i].y) > Terrain::shadowComputeVariables[i].distance * (0.075 * (i + 1)))
		{
			return (i);
		}
	}

	return (-1);
}

void Terrain::CheckTerrainOffset(VkCommandBuffer commandBuffer)
{
	bool updated = true;

	float xw = Manager::camera.Position().x;
	float yw = Manager::camera.Position().y;
	float zw = Manager::camera.Position().z;

	float x0 = xw - terrainLod0Offset.x;
	float z0 = zw - terrainLod0Offset.y;
	float x1 = xw - terrainLod1Offset.x;
	float z1 = zw - terrainLod1Offset.y;

	if (abs(xw) >= terrainChunkSize * terrainStep || abs(zw) >= terrainChunkSize * terrainStep || abs(yw) >= terrainHeight * terrainStep)
	{
		glm::vec3 newOffset = glm::vec3(0);
		if (abs(xw) >= terrainChunkSize * terrainStep || abs(zw) >= terrainChunkSize * terrainStep)
		{
			glm::vec2 horizontalOffset = glm::vec2(Utilities::Fits(terrainChunkSize * terrainStep, xw), Utilities::Fits(terrainChunkSize * terrainStep, zw)) * terrainChunkSize * terrainStep;
			terrainLod0Offset -= horizontalOffset;
			terrainLod1Offset -= horizontalOffset;
			newOffset.x = horizontalOffset.x;
			newOffset.z = horizontalOffset.y;

			for (int i = TERRAIN_SHADOW_CASCADES - 1; i >= 0; i--)
			{
				terrainShadowOffsets[i] -= horizontalOffset;
			}
		}
		if (abs(yw) >= terrainHeight * terrainStep)
		{
			newOffset.y = yw;
		}

		terrainOffset += newOffset;
		Manager::camera.Move(-newOffset);

		Manager::UpdateShaderVariables();

		return;
	}

	if (!Time::newSubTick) return;

	if (abs(x1) >= terrainLod1Size * terrainLod1Step || abs(z1) >= terrainLod1Size * terrainLod1Step)
	{
		glm::vec2 newOffset = glm::vec2(Utilities::Fits(terrainLod1Size * terrainLod1Step, x1), Utilities::Fits(terrainLod1Size * terrainLod1Step, z1)) * terrainLod1Size * terrainLod1Step;
		terrainLod1Offset += newOffset;

		ComputeHeightMap(commandBuffer, 1);
	}
	else if (abs(x0) >= terrainLod0Size * terrainLod0Step || abs(z0) >= terrainLod0Size * terrainLod0Step)
	{
		glm::vec2 newOffset = glm::vec2(Utilities::Fits(terrainLod0Size * terrainLod0Step, x0), Utilities::Fits(terrainLod0Size * terrainLod0Step, z0)) * terrainLod0Size * terrainLod0Step;
		terrainLod0Offset += newOffset;

		ComputeHeightMap(commandBuffer, 0);
	}
	else
	{
		updated = false;
	}

	if (updated)
	{
		Manager::UpdateShaderVariables();
	}
}

void Terrain::CheckTerrainShadowOffset()
{
	float xw = Manager::camera.Position().x;
	float zw = Manager::camera.Position().z;

	if (updateTerrainShadows || ShouldUpdateShadows(glm::vec2(xw, zw)) >= 0)
	{
		if (updateTerrainShadows)
		{
			updateTerrainShadows = false;
			for (int i = TERRAIN_SHADOW_CASCADES - 1; i >= 0; i--)
			{
				glm::vec2 flooredViewPosition = floor(glm::vec2(xw, zw) / shadowComputeVariables[i].spacing);
    			float u = (flooredViewPosition.x) * shadowComputeVariables[i].spacing;
    			float v = (flooredViewPosition.y) * shadowComputeVariables[i].spacing;
				terrainShadowOffsets[i] = glm::vec2(u, v);
				ComputeShadows(i);
			}
		}
		else
		{
			int i = ShouldUpdateShadows(glm::vec2(xw, zw));
			glm::vec2 flooredViewPosition = floor(glm::vec2(xw, zw) / shadowComputeVariables[i].spacing);
    		float u = (flooredViewPosition.x) * shadowComputeVariables[i].spacing;
    		float v = (flooredViewPosition.y) * shadowComputeVariables[i].spacing;
			terrainShadowOffsets[i] = glm::vec2(u, v);
			ComputeShadows(i);
		}

		Manager::UpdateShaderVariables();
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

bool Terrain::InView(const glm::vec3 &position, float tolerance, const glm::mat4 &projection, const glm::mat4 &view, bool lod)
{
	glm::vec4 viewSpace = projection * view * glm::vec4(position, 1.0);

	glm::vec3 clipSpace = viewSpace;
	clipSpace /= -viewSpace.w;

	clipSpace.x = clipSpace.x * 0.5f + 0.5f;
	clipSpace.y = clipSpace.y * 0.5f + 0.5f;
	clipSpace.z = viewSpace.w;

	//if (clipSpace.z < 0.0 || clipSpace.z > (lod ? Manager::camera.farLod : Manager::camera.far)) return false;
	if (clipSpace.z < 0.0) return false;

	return !(clipSpace.x < -tolerance || clipSpace.x > 1.0f + tolerance || clipSpace.y < -tolerance || clipSpace.y > 1.0f + tolerance);
}

bool Terrain::ChunkInView(glm::vec3 position, float tolerance, glm::mat4 projection, glm::mat4 view, bool main, bool lod)
{
	if (InView(position, tolerance, projection, view, lod)) return true;
	else if (InView(position + glm::vec3(Terrain::terrainMeshSize * 0.5, 0, Terrain::terrainMeshSize * 0.5), 
		tolerance, projection, view, lod)) return true;
	else if (InView(position + glm::vec3(Terrain::terrainMeshSize * -0.5, 0, Terrain::terrainMeshSize * -0.5), 
		tolerance, projection, view, lod)) return true;
	else if (InView(position + glm::vec3(Terrain::terrainMeshSize * -0.5, 0, Terrain::terrainMeshSize * 0.5), 
		tolerance, projection, view, lod)) return true;
	else if (InView(position + glm::vec3(Terrain::terrainMeshSize * 0.5, 0, Terrain::terrainMeshSize * -0.5), 
		tolerance, projection, view, lod)) return true;
	else if (main) return ChunkInView(position + glm::vec3(0, Terrain::terrainHeight * 0.5, 0), tolerance, projection, view, false, lod);
	return false;
}

bool Terrain::HeightMapsGenerated()
{
	return (heightMapArrayLayersGenerated == heightMapCount);
}

Pipeline Terrain::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Terrain::graphicsLodPipeline{Manager::currentDevice, Manager::camera};
Pipeline Terrain::cullPipeline{Manager::currentDevice, Manager::camera};
Pipeline Terrain::heightMapComputePipeline{Manager::currentDevice, Manager::camera};
Pipeline Terrain::heightMapArrayComputePipeline{Manager::currentDevice, Manager::camera};
Pipeline Terrain::shadowComputePipeline{Manager::currentDevice, Manager::camera};

std::vector<Texture> Terrain::grassTextures;
std::vector<Texture> Terrain::rockTextures;
std::vector<Texture> Terrain::dirtTextures;
//Texture Terrain::grassDiffuseTexture{Manager::currentDevice};
//Texture Terrain::rockDiffuseTexture{Manager::currentDevice};
//Texture Terrain::dirtDiffuseTexture{Manager::currentDevice};

Texture Terrain::heightMapArrayTexture{Manager::currentDevice};
Texture Terrain::heightMapLod0Texture{Manager::currentDevice};
Texture Terrain::heightMapLod1Texture{Manager::currentDevice};

std::vector<Texture> Terrain::terrainShadowTextures;

HeightMapComputeVariables Terrain::heightMapComputeVariables;
Buffer Terrain::heightMapComputeVariablesBuffer;

HeightMapArrayComputeVariables Terrain::heightMapArrayComputeVariables;
Buffer Terrain::heightMapArrayComputeVariablesBuffer;

std::vector<ShadowComputeVariables> Terrain::shadowComputeVariables;
Buffer Terrain::shadowComputeVariablesBuffer;

Descriptor Terrain::graphicsDescriptor{Manager::currentDevice};
//Descriptor Terrain::graphicsLodDescriptor{Manager::currentDevice};
Descriptor Terrain::cullDescriptor{Manager::currentDevice};
Descriptor Terrain::heightMapComputeDescriptor{Manager::currentDevice};
Descriptor Terrain::heightMapArrayComputeDescriptor{Manager::currentDevice};
Descriptor Terrain::shadowComputeDescriptor{Manager::currentDevice};

Mesh Terrain::lod0Mesh{};
Mesh Terrain::lod1Mesh{};
Mesh Terrain::lod2Mesh{};

std::vector<Object> Terrain::terrainChunks;

float Terrain::terrainMeshSize = 10000;
float Terrain::terrainChunkSize = 10000;
float Terrain::terrainLod0Size = 2500;
float Terrain::terrainLod1Size = 5000;

int Terrain::heightMapResolution = 1024;
int Terrain::heightMapLod0Resolution = 1024;
int Terrain::heightMapLod1Resolution = 1024;

int Terrain::terrainChunkRadius = 3;
int Terrain::terrainChunkLength = Terrain::terrainChunkRadius * 2 + 1;
int Terrain::terrainChunkCount = Terrain::terrainChunkLength * Terrain::terrainChunkLength;

int Terrain::heightMapRadius = 4;
int Terrain::heightMapLength = Terrain::heightMapRadius * 2 + 1;
int Terrain::heightMapCount = Terrain::heightMapLength * Terrain::heightMapLength;

float Terrain::terrainTotalSize = Terrain::heightMapLength * Terrain::terrainChunkSize;
float Terrain::terrainHeight = 5000;
//float Terrain::waterHeight = 750.0f;
float Terrain::waterHeight = 400.0f;
//float Terrain::waterHeight = 350.0f;
float Terrain::waterBlendDistance = 25.0f;

glm::vec3 Terrain::terrainOffset = glm::vec3(0.0, 2500.0, 0.0);
//glm::vec3 Terrain::terrainOffset = glm::vec3(0.0, 0.0, 0.0);
glm::vec2 Terrain::terrainLod0Offset = glm::vec2(0);
glm::vec2 Terrain::terrainLod1Offset = glm::vec2(0);
std::vector<glm::vec2> Terrain::terrainShadowOffsets;
//float Terrain::terrainStep = 0.25f * 0.25f;
float Terrain::terrainStep = 0.05f;
float Terrain::terrainLod0Step = 0.125f;
float Terrain::terrainLod1Step = 0.25f;

bool Terrain::updateTerrainShadows = true;

uint32_t Terrain::currentBoundHeightMap = -1;

int Terrain::heightMapArrayLayersGenerated = 0;

Curve Terrain::lowLandsCurve = Curve(32, EXPONENT);

//bool Terrain::terrainLod0OffsetReset = false;
//bool Terrain::terrainLod1OffsetReset = false;