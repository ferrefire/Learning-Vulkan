#include "trees.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "input.hpp"
#include "time.hpp"
#include "terrain.hpp"
#include "leaves.hpp"
#include "capture.hpp"

#include <iostream>

void Trees::Create()
{
	treeLod0RenderCount = treeLod0RenderBase * treeLod0RenderBase;
	treeLod1RenderBase = treeLod0RenderBase + treeLod1RenderBase;
	treeLod1RenderCount = treeLod1RenderBase * treeLod1RenderBase - treeLod0RenderCount;
	treeLod2RenderBase = treeLod1RenderBase + treeLod2RenderBase;
	treeLod2RenderCount = treeLod2RenderBase * treeLod2RenderBase - treeLod0RenderCount - treeLod1RenderCount;
	treeLod3RenderBase = treeLod2RenderBase + treeLod3RenderBase;
	treeLod3RenderCount = treeLod3RenderBase * treeLod3RenderBase - treeLod0RenderCount - treeLod1RenderCount - treeLod2RenderCount;
	treeLod4RenderBase = treeLod3RenderBase + treeLod4RenderBase;
	treeLod4RenderCount = treeLod4RenderBase * treeLod4RenderBase - treeLod0RenderCount - treeLod1RenderCount - treeLod2RenderCount - treeLod3RenderCount;
	treeLod5RenderBase = treeLod4RenderBase + treeLod5RenderBase;
	treeLod5RenderCount = treeLod5RenderBase * treeLod5RenderBase - treeLod0RenderCount - treeLod1RenderCount - treeLod2RenderCount - treeLod3RenderCount - treeLod4RenderCount;
	treeTotalRenderBase = treeLod5RenderBase;
	//treeTotalRenderCount = treeTotalRenderBase * treeTotalRenderBase;
	treeTotalRenderCount = treeLod0RenderCount + treeLod1RenderCount + treeLod2RenderCount + treeLod3RenderCount + treeLod4RenderCount + treeLod5RenderCount;

	std::cout << treeTotalRenderCount << std::endl;

	CreateMeshes();
	CreateTextures();
	CreateGraphicsPipeline();
	if (Manager::settings.shadows) CreateShadowPipeline();
	//CreateCullPipeline();
	CreateComputeSetupPipeline();
	CreateComputeRenderPipeline();
	CreateBuffers();
	CreateGraphicsDescriptor();
	if (Manager::settings.shadows) CreateShadowDescriptor();
	//CreateCullDescriptor();
	CreateComputeSetupDescriptor();
	Leaves::CreateBuffers();
	CreateComputeRenderDescriptor();
}

void Trees::CreateMeshes()
{
	//treeMesh.normal = true;
	//treeMesh.shape.normal = true;
	//treeMesh.shape.SetShape(CYLINDER, 9);
	//treeMesh.RecalculateVertices();

	BranchConfiguration branchConfig;
	branchConfig.main = true;
	branchConfig.splitCount = 4;
	//branchConfig.resolution = 32;
	branchConfig.resolution = 24;
	//branchConfig.blendRange = 8;
	//branchConfig.minSize = 0.75;
	branchConfig.minSize = 0.1;
	branchConfig.lod = 0;
	branchConfig.leaves = true;

	Mesh leafPositionsMesh;
	GenerateTrunkMesh(leafPositionsMesh, branchConfig);
	leafPositionsMesh.Create();
	leafPositionsMesh.Destroy();
	//branchConfig.splitCount = 3;
	branchConfig.leaves = false;
	branchConfig.minSize = 0.15;
	//branchConfig.resolution = 24;

	GenerateTrunkMesh(treeLod0Mesh, branchConfig);
	treeLod0Mesh.Create();

	branchConfig.leaves = false;

	//leafPositionsTotal.reserve(leafPositions0.size() + leafPositions1.size());
	//leafPositionsTotal.insert(leafPositionsTotal.end(), leafPositions0.begin(), leafPositions0.end());
	//leafPositionsTotal.insert(leafPositionsTotal.end(), leafPositions1.begin(), leafPositions1.end());
	//treeVariables.leafCountTotal = leafPositionsTotal.size();
	//treeVariables.leafCount0 = leafPositions0.size();
	//treeVariables.leafCount1 = leafPositions1.size();
	//treeVariables.leafCount2 = int(floor(float(treeVariables.leafCount1) / 2.0f));
	//treeVariables.leafCount3 = int(floor(float(treeVariables.leafCount1) / 8.0f));
	//treeVariables.leafCount4 = int(floor(float(treeVariables.leafCount1) / 16.0f));

	//treeVariables.leafCountTotal = leafPositions.size();
	//treeVariables.leafCount0 = int(floor(float(treeVariables.leafCountTotal) / 2.0f));
	//treeVariables.leafCount1 = int(floor(float(treeVariables.leafCountTotal) / 4.0f));
	//treeVariables.leafCount2 = int(floor(float(treeVariables.leafCountTotal) / 8.0f));
	//treeVariables.leafCount3 = int(floor(float(treeVariables.leafCountTotal) / 16.0f));
	//treeVariables.leafCount4 = int(floor(float(treeVariables.leafCountTotal) / 32.0f));

	float decreaseFactor = 2.0;

	treeVariables.leafCounts[0].x = leafPositions.size();
	treeVariables.leafCounts[0].y = 0;
	treeVariables.leafCounts[0].z = 1;
	treeVariables.leafCounts[0].w = 1 * 0.25;

	treeVariables.leafCounts[1].x = int(floor(float(treeVariables.leafCounts[0].x) / (2.0f * decreaseFactor)));
	treeVariables.leafCounts[1].y = treeLod0RenderCount * treeVariables.leafCounts[0].x;
	treeVariables.leafCounts[1].z = 2 * decreaseFactor;
	treeVariables.leafCounts[1].w = 2 * 1.0;

	treeVariables.leafCounts[2].x = int(floor(float(treeVariables.leafCounts[0].x) / (8.0f * decreaseFactor)));
	treeVariables.leafCounts[2].y = treeLod1RenderCount * treeVariables.leafCounts[1].x +
		treeVariables.leafCounts[1].y;
	treeVariables.leafCounts[2].z = 8 * decreaseFactor;
	treeVariables.leafCounts[2].w = 2 * 4;

	treeVariables.leafCounts[3].x = int(floor(float(treeVariables.leafCounts[0].x) / (48.0f * decreaseFactor)));
	treeVariables.leafCounts[3].y = treeLod2RenderCount * treeVariables.leafCounts[2].x +
		treeVariables.leafCounts[2].y;
	treeVariables.leafCounts[3].z = 48 * decreaseFactor;
	treeVariables.leafCounts[3].w = 6 * 3.0;

	treeVariables.leafCounts[4].x = int(floor(float(treeVariables.leafCounts[0].x) / (96.0f * decreaseFactor)));
	treeVariables.leafCounts[4].y = treeLod3RenderCount * treeVariables.leafCounts[3].x +
		treeVariables.leafCounts[3].y;
	treeVariables.leafCounts[4].z = 96 * decreaseFactor;
	treeVariables.leafCounts[4].w = 16 * 2.5;

	treeVariables.leafCounts[5].x = int(floor(float(treeVariables.leafCounts[0].x) / (288.0f * decreaseFactor)));
	treeVariables.leafCounts[5].y = treeLod4RenderCount * treeVariables.leafCounts[4].x +
		treeVariables.leafCounts[4].y;
	treeVariables.leafCounts[5].z = 288 * decreaseFactor;
	treeVariables.leafCounts[5].w = 32 * 1.5;

	totalLeafCount = (treeLod5RenderCount * treeVariables.leafCounts[5].x) + treeVariables.leafCounts[5].y;

	std::cout << "tree leaf count: " << leafPositions.size() << std::endl;

	std::cout << "total leaf count: " << totalLeafCount << std::endl;
	std::cout << "lod 0 leaf count: " << treeVariables.leafCounts[0].x << std::endl;
	std::cout << "lod 1 leaf count: " << treeVariables.leafCounts[1].x << std::endl;
	std::cout << "lod 2 leaf count: " << treeVariables.leafCounts[2].x << std::endl;
	std::cout << "lod 3 leaf count: " << treeVariables.leafCounts[3].x << std::endl;
	std::cout << "lod 4 leaf count: " << treeVariables.leafCounts[4].x << std::endl;
	std::cout << "lod 5 leaf count: " << treeVariables.leafCounts[5].x << std::endl << std::endl;

	std::cout << "lod 0 total leaf count: " << treeVariables.leafCounts[0].x * treeLod0RenderCount << std::endl;
	std::cout << "lod 1 total leaf count: " << treeVariables.leafCounts[1].x * treeLod1RenderCount << std::endl;
	std::cout << "lod 2 total leaf count: " << treeVariables.leafCounts[2].x * treeLod2RenderCount << std::endl;
	std::cout << "lod 3 total leaf count: " << treeVariables.leafCounts[3].x * treeLod3RenderCount << std::endl;
	std::cout << "lod 4 total leaf count: " << treeVariables.leafCounts[4].x * treeLod4RenderCount << std::endl;
	std::cout << "lod 5 total leaf count: " << treeVariables.leafCounts[5].x * treeLod5RenderCount << std::endl << std::endl;

	glm::vec4 sumPosition = glm::vec4(0.0);
	for (int i = 0; i < leafPositions.size(); i++)
	{
		sumPosition += leafPositions[i];
	}
	sumPosition /= leafPositions.size();
	Utilities::PrintVec(sumPosition);

	branchConfig.resolution = 12;
	branchConfig.minSize = 0.15;
	branchConfig.lod = 1;
	GenerateTrunkMesh(treeLod1Mesh, branchConfig);
	treeLod1Mesh.Create();

	branchConfig.resolution = 4;
	branchConfig.minSize = 0.5;
	branchConfig.lod = 2;
	GenerateTrunkMesh(treeLod2Mesh, branchConfig);
	treeLod2Mesh.shape.Scale(glm::vec3(1.0, 1.0, 1.0));
	treeLod2Mesh.RecalculateVertices();
	treeLod2Mesh.Create();

	branchConfig.resolution = 4;
	branchConfig.minSize = 0.75;
	branchConfig.lod = 3;
	GenerateTrunkMesh(treeLod3Mesh, branchConfig);
	treeLod3Mesh.shape.Scale(glm::vec3(2.25, 1.5, 2.25));
	treeLod3Mesh.RecalculateVertices();
	treeLod3Mesh.Create();

	//branchConfig.resolution = 4;
	//branchConfig.minSize = 1.0;
	////branchConfig.minSize = 0.75;
	//branchConfig.lod = 4;
	//GenerateTrunkMesh(treeLod4Mesh, branchConfig);
	//treeLod4Mesh.Create();

	treeLod4Mesh.coordinate = true;
	treeLod4Mesh.normal = true;
	treeLod4Mesh.shape.coordinate = true;
	treeLod4Mesh.shape.normal = true;
	treeLod4Mesh.shape.SetShape(CYLINDER, 4);
	treeLod4Mesh.shape.Scale(glm::vec3(4.0f, 2.5f, 4.0f), true);
	//treeLod4Mesh.shape.Move(glm::vec3(0.0f, 20.0f, 0.0f));
	treeLod4Mesh.RecalculateVertices();
	treeLod4Mesh.Create();

	treeLod5Mesh.coordinate = true;
	treeLod5Mesh.normal = true;
	treeLod5Mesh.shape.coordinate = true;
	treeLod5Mesh.shape.normal = true;
	treeLod5Mesh.shape.SetShape(CROSS, 2);
	treeLod5Mesh.shape.Scale(glm::vec3(150.0f, 100.0f, 150.0f));
	treeLod5Mesh.shape.Move(glm::vec3(0.0f, 35.0f, 0.0f));
	treeLod5Mesh.RecalculateVertices();
	treeLod5Mesh.Create();
}

void Trees::CreateGraphicsPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(3);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;
	descriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE;
	descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[2].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[2].count = treeTextures.size();
	//descriptorLayoutConfig[3].type = IMAGE_SAMPLER;
	//descriptorLayoutConfig[3].stages = FRAGMENT_STAGE;
	//descriptorLayoutConfig[3].count = Capture::captureCount;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = treeLod0Mesh.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("tree", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);

	//pipelineConfiguration.foliage = true;
	//graphicsLodPipeline.CreateGraphicsPipeline("treeLod", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);

	//pipelineConfiguration.foliage = false;
	//pipelineConfiguration.capture = true;
	//capturePipeline.CreateGraphicsPipeline("tree", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Trees::CreateShadowPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;
	descriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.shadow = true;
	pipelineConfiguration.pushConstantCount = 2;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = treeLod0Mesh.MeshVertexInfo();

	shadowPipeline.CreateGraphicsPipeline("treeShadow", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Trees::CreateCullPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;
	descriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.cull = true;

	VertexInfo vertexInfo = treeLod0Mesh.MeshVertexInfo();

	shadowPipeline.CreateGraphicsPipeline("treeCull", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Trees::CreateComputeSetupPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(2);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[1].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[1].stages = COMPUTE_STAGE;

	computeSetupPipeline.CreateComputePipeline("treeSetupCompute", descriptorLayoutConfig);
}

void Trees::CreateComputeRenderPipeline()
{
	int i = 0;

	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(8);
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[i].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[i].type = STORAGE_BUFFER;
	descriptorLayoutConfig[i++].stages = COMPUTE_STAGE;

	PushConstantConfiguration pushConstantConfig;
	pushConstantConfig.pushConstantCount = 1;
	pushConstantConfig.pushConstantStage = COMPUTE_STAGE;
	pushConstantConfig.pushConstantSize = sizeof(uint32_t);

	computeRenderPipeline.CreateComputePipeline("treeRenderCompute", descriptorLayoutConfig, pushConstantConfig);
}

void Trees::CreateTextures()
{
	SamplerConfiguration samplerConfig;
	samplerConfig.repeatMode = REPEAT;
	samplerConfig.mipLodBias = 0.0f;

	treeTextures.resize(3);
	treeTextures[0].CreateTexture("tree_diff.jpg", samplerConfig);
	treeTextures[1].CreateTexture("tree_norm.jpg", samplerConfig);
	treeTextures[2].CreateTexture("tree_ao.jpg", samplerConfig);

	//diffuseTexture.CreateTexture("tree_diff.jpg", samplerConfig);
}

void Trees::CreateBuffers()
{
	BufferConfiguration dataConfiguration;
	dataConfiguration.size = sizeof(TreeData) * treeCount;
	dataConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	dataConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	dataConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	dataConfiguration.mapped = false;

	dataBuffer.Create(dataConfiguration);

	renderBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration renderConfiguration;
	renderConfiguration.size = sizeof(TreeRenderData) * treeTotalRenderCount;
	renderConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	renderConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	renderConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	renderConfiguration.mapped = false;

	for (Buffer &buffer : renderBuffers)
	{
		buffer.Create(renderConfiguration);
	}

	shadowBuffers.resize(Manager::settings.maxFramesInFlight);

	for (Buffer &buffer : shadowBuffers)
	{
		buffer.Create(renderConfiguration);
	}

	countBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration countConfiguration;
	countConfiguration.size = sizeof(TreeCountData);
	countConfiguration.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	countConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	countConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	countConfiguration.mapped = true;

	for (Buffer &buffer : countBuffers)
	{
		buffer.Create(countConfiguration);
	}

	variableBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration variablesConfiguration;
	variablesConfiguration.size = sizeof(TreeVariables);
	variablesConfiguration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	variablesConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	variablesConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	variablesConfiguration.mapped = true;

	for (Buffer &buffer : variableBuffers)
	{
		buffer.Create(variablesConfiguration);
	}

	BufferConfiguration leafPositionsConfiguration;
	leafPositionsConfiguration.size = sizeof(glm::vec4) * treeVariables.leafCounts[0].x;
	leafPositionsConfiguration.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	leafPositionsConfiguration.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	//leafPositionsConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	leafPositionsConfiguration.mapped = false;

	leafPositionsBuffer.Create(leafPositions.data(), leafPositionsConfiguration);
}

void Trees::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(3);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(renderBuffers.size());
	int index = 0;
	for (Buffer &buffer : renderBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(TreeRenderData) * treeTotalRenderCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[1].type = UNIFORM_BUFFER;
	descriptorConfig[1].stages = VERTEX_STAGE;
	descriptorConfig[1].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(TreeVariables);
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[2].type = IMAGE_SAMPLER;
	descriptorConfig[2].stages = FRAGMENT_STAGE;
	descriptorConfig[2].count = treeTextures.size();
	descriptorConfig[2].imageInfos.resize(treeTextures.size());
	for (int i = 0; i < treeTextures.size(); i++)
	{
		descriptorConfig[2].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[2].imageInfos[i].imageView = treeTextures[i].imageView;
		descriptorConfig[2].imageInfos[i].sampler = treeTextures[i].sampler;
	}
	//descriptorConfig[2].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	//descriptorConfig[2].imageInfo.imageView = diffuseTexture.imageView;
	//descriptorConfig[2].imageInfo.sampler = diffuseTexture.sampler;

	//descriptorConfig[3].type = IMAGE_SAMPLER;
	//descriptorConfig[3].stages = FRAGMENT_STAGE;
	//descriptorConfig[3].count = Capture::captureCount;
	//descriptorConfig[3].imageInfos.resize(Capture::captureCount);
	//for (int i = 0; i < Capture::captureTextures.size(); i++)
	//{
	//	descriptorConfig[3].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
	//	descriptorConfig[3].imageInfos[i].imageView = Capture::captureTextures[i].imageView;
	//	descriptorConfig[3].imageInfos[i].sampler = Capture::captureTextures[i].sampler;
	//}

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Trees::CreateShadowDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(2);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(shadowBuffers.size());
	int index = 0;
	for (Buffer &buffer : shadowBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(TreeRenderData) * treeTotalRenderCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[1].type = UNIFORM_BUFFER;
	descriptorConfig[1].stages = VERTEX_STAGE;
	descriptorConfig[1].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(TreeVariables);
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	shadowDescriptor.Create(descriptorConfig, shadowPipeline.objectDescriptorSetLayout);
}

void Trees::CreateCullDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(2);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(renderBuffers.size());
	int index = 0;
	for (Buffer &buffer : renderBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(TreeRenderData) * treeTotalRenderCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[1].type = UNIFORM_BUFFER;
	descriptorConfig[1].stages = VERTEX_STAGE;
	descriptorConfig[1].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(TreeVariables);
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	cullDescriptor.Create(descriptorConfig, cullPipeline.objectDescriptorSetLayout);
}

void Trees::CreateComputeSetupDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(2);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].buffersInfo.resize(1);
	descriptorConfig[0].buffersInfo[0].buffer = dataBuffer.buffer;
	descriptorConfig[0].buffersInfo[0].range = sizeof(TreeData) * treeCount;
	descriptorConfig[0].buffersInfo[0].offset = 0;

	descriptorConfig[1].type = UNIFORM_BUFFER;
	descriptorConfig[1].stages = COMPUTE_STAGE;
	descriptorConfig[1].buffersInfo.resize(1);
	descriptorConfig[1].buffersInfo[0].buffer = variableBuffers[0].buffer;
	descriptorConfig[1].buffersInfo[0].range = sizeof(TreeVariables);
	descriptorConfig[1].buffersInfo[0].offset = 0;

	computeSetupDescriptor.perFrame = false;
	computeSetupDescriptor.Create(descriptorConfig, computeSetupPipeline.objectDescriptorSetLayout);
}

void Trees::CreateComputeRenderDescriptor()
{
	int i = 0;

	std::vector<DescriptorConfiguration> descriptorConfig(8);

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(1);
	descriptorConfig[i].buffersInfo[0].buffer = dataBuffer.buffer;
	descriptorConfig[i].buffersInfo[0].range = sizeof(TreeData) * treeCount;
	descriptorConfig[i].buffersInfo[0].offset = 0;
	i++;

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(renderBuffers.size());
	int index = 0;
	for (Buffer &buffer : renderBuffers)
	{
		descriptorConfig[i].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[i].buffersInfo[index].range = sizeof(TreeRenderData) * treeTotalRenderCount;
		descriptorConfig[i].buffersInfo[index].offset = 0;
		index++;
	}
	i++;

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(shadowBuffers.size());
	index = 0;
	for (Buffer &buffer : shadowBuffers)
	{
		descriptorConfig[i].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[i].buffersInfo[index].range = sizeof(TreeRenderData) * treeTotalRenderCount;
		descriptorConfig[i].buffersInfo[index].offset = 0;
		index++;
	}
	i++;

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(countBuffers.size());
	index = 0;
	for (Buffer &buffer : countBuffers)
	{
		descriptorConfig[i].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[i].buffersInfo[index].range = sizeof(TreeCountData);
		descriptorConfig[i].buffersInfo[index].offset = 0;
		index++;
	}
	i++;

	descriptorConfig[i].type = UNIFORM_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[i].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[i].buffersInfo[index].range = sizeof(TreeVariables);
		descriptorConfig[i].buffersInfo[index].offset = 0;
		index++;
	}
	i++;

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(Leaves::renderBuffers.size());
	index = 0;
	for (Buffer &buffer : Leaves::renderBuffers)
	{
		descriptorConfig[i].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[i].buffersInfo[index].range = sizeof(LeafData) * totalLeafCount;
		descriptorConfig[i].buffersInfo[index].offset = 0;
		index++;
	}
	i++;

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(Leaves::shadowBuffers.size());
	index = 0;
	for (Buffer &buffer : Leaves::shadowBuffers)
	{
		descriptorConfig[i].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[i].buffersInfo[index].range = sizeof(LeafData) * totalLeafCount;
		descriptorConfig[i].buffersInfo[index].offset = 0;
		index++;
	}
	i++;

	descriptorConfig[i].type = STORAGE_BUFFER;
	descriptorConfig[i].stages = COMPUTE_STAGE;
	descriptorConfig[i].buffersInfo.resize(1);
	descriptorConfig[i].buffersInfo[0].buffer = leafPositionsBuffer.buffer;
	descriptorConfig[i].buffersInfo[0].range = sizeof(glm::vec4) * treeVariables.leafCounts[0].x;
	descriptorConfig[i].buffersInfo[0].offset = 0;
	i++;

	computeRenderDescriptor.Create(descriptorConfig, computeRenderPipeline.objectDescriptorSetLayout);
}

void Trees::Destroy()
{
	DestroyPipelines();
	DestroyMeshes();
	DestroyTextures();
	DestroyDescriptors();
	DestroyBuffers();
}

void Trees::DestroyMeshes()
{
	treeLod0Mesh.Destroy();
	treeLod1Mesh.Destroy();
	treeLod2Mesh.Destroy();
	treeLod3Mesh.Destroy();
	treeLod4Mesh.Destroy();
	treeLod5Mesh.Destroy();
}

void Trees::DestroyPipelines()
{
	graphicsPipeline.Destroy();
	//graphicsLodPipeline.Destroy();
	shadowPipeline.Destroy();
	//cullPipeline.Destroy();
	//capturePipeline.Destroy();
	computeSetupPipeline.Destroy();
	computeRenderPipeline.Destroy();
}

void Trees::DestroyTextures()
{
	//diffuseTexture.Destroy();
	for (Texture &texture : treeTextures)
	{
		texture.Destroy();
	}
	treeTextures.clear();
}

void Trees::DestroyBuffers()
{
	dataBuffer.Destroy();

	for (Buffer &buffer : renderBuffers)
	{
		buffer.Destroy();
	}
	renderBuffers.clear();

	for (Buffer &buffer : shadowBuffers)
	{
		buffer.Destroy();
	}
	shadowBuffers.clear();

	for (Buffer &buffer : countBuffers)
	{
		buffer.Destroy();
	}
	countBuffers.clear();

	for (Buffer &buffer : variableBuffers)
	{
		buffer.Destroy();
	}
	variableBuffers.clear();

	leafPositionsBuffer.Destroy();
}

void Trees::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	shadowDescriptor.Destroy();
	//cullDescriptor.Destroy();
	computeSetupDescriptor.Destroy();
	computeRenderDescriptor.Destroy();
}

void Trees::Start()
{
	treeVariables.treeBase = treeBase;
	treeVariables.treeCount = treeCount;

	treeVariables.treeLod0RenderBase = treeLod0RenderBase;
	treeVariables.treeLod0RenderCount = treeLod0RenderCount;
	treeVariables.treeLod1RenderBase = treeLod1RenderBase;
	treeVariables.treeLod1RenderCount = treeLod1RenderCount;
	treeVariables.treeLod2RenderBase = treeLod2RenderBase;
	treeVariables.treeLod2RenderCount = treeLod2RenderCount;
	treeVariables.treeLod3RenderBase = treeLod3RenderBase;
	treeVariables.treeLod3RenderCount = treeLod3RenderCount;
	treeVariables.treeLod4RenderBase = treeLod4RenderBase;
	treeVariables.treeLod4RenderCount = treeLod4RenderCount;
	treeVariables.treeTotalRenderBase = treeTotalRenderBase;
	treeVariables.treeTotalRenderCount = treeTotalRenderCount;

	treeVariables.spacing = 50;
	treeVariables.spacingMult = 1.0 / treeVariables.spacing;

	//treeVariables.leafCount0 = leafPositions0.size();
	//treeVariables.leafCount1 = leafPositions1.size();
	//for (int i = 0; i < Leaves::leafCount; i++) treeVariables.leafPositions[i] = glm::vec4(leafPositions[i], 0.0);
	//std::cout << "leaf count Total: " << treeVariables.leafCountTotal << std::endl;
	//std::cout << "leaf count 0: " << treeVariables.leafCount0 << std::endl;
	//std::cout << "leaf count 1: " << treeVariables.leafCount1 << std::endl;

	treeRenderCounts.resize(Manager::settings.maxFramesInFlight);

	for (Buffer &buffer : variableBuffers)
	{
		memcpy(buffer.mappedBuffer, &treeVariables, sizeof(treeVariables));
	}

	//ComputeTreeSetup();
}

void Trees::Frame()
{
	//if (treesComputed && !Capture::capturing && Time::newTick && shouldUpdateLodTree)
	//{
	//	CaptureTree();
	//	shouldUpdateLodTree = false;
	//}

	if (!treesComputed && Terrain::HeightMapsGenerated())
	{
		treesComputed = true;
		ComputeTreeSetup();
	}

	if (Input::GetKey(GLFW_KEY_G).pressed)
	{
		treeLod0Mesh.DestroyAtRuntime();

		double ctime = Time::GetCurrentTime() * 100.0;
		Utilities::seed = ctime;

		BranchConfiguration branchConfig;
		branchConfig.seed = ctime;
		branchConfig.main = true;
		branchConfig.splitCount = Utilities::RandomInt(2, 4);
		//branchConfig.splitChance = Utilities::RandomFloat(0.0f, 0.75f);
		//branchConfig.fallChance = Utilities::RandomFloat(0.0, 0.25);
		branchConfig.angleRandomness = Utilities::RandomFloat(0.0f, 0.5f);
		branchConfig.reach = Utilities::RandomFloat(1.0f, 3.0f);
		branchConfig.thickness = Utilities::RandomFloat(0.5f, 0.6f);
		branchConfig.length = Utilities::RandomFloat(0.7f, 0.85f);
		//branchConfig.steepness.x = Utilities::RandomFloat(-10.0f, 10.0f);
		//branchConfig.steepness.y = Utilities::RandomFloat(30.0f, 70.0f);
		branchConfig.resolution = 32;
		
		//branchConfig.blendRange = 8;
		//branchConfig.minSize = 0.75;

		//std::cout << branchConfig.seed << std::endl;

		GenerateTrunkMesh(treeLod0Mesh, branchConfig);
		treeLod0Mesh.Create();
	}
}

void Trees::PostFrame()
{
	ComputeTreeRender(nullptr);
}

void Trees::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
	graphicsPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderTrees(commandBuffer);
}

void Trees::RecordComputeCommands(VkCommandBuffer commandBuffer)
{
	ComputeTreeRender(commandBuffer);
}

void Trees::RecordShadowCommands(VkCommandBuffer commandBuffer, int cascade)
{
	if (cascade > 2) return;

	shadowPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	shadowDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderShadows(commandBuffer, cascade);
}

void Trees::RecordCullCommands(VkCommandBuffer commandBuffer)
{
	cullPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, cullPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	cullDescriptor.Bind(commandBuffer, cullPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderCulling(commandBuffer);
}

void Trees::RecordCaptureCommands(VkCommandBuffer commandBuffer)
{
	capturePipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, capturePipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, capturePipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderTrees(commandBuffer);
}

void Trees::RenderTrees(VkCommandBuffer commandBuffer)
{
	uint32_t lod0 = 0;
	uint32_t lod1 = 1;
	uint32_t lod2 = 2;
	uint32_t lod3 = 3;
	uint32_t lod4 = 4;
	uint32_t lod5 = 5;

	treeLod0Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod0Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod0RenderCount, 0, 0, 0);

	treeLod1Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod1Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod1RenderCount, 0, 0, 0);

	treeLod2Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod2), &lod2);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod2Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod2RenderCount, 0, 0, 0);

	treeLod3Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod3), &lod3);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod3Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod3RenderCount, 0, 0, 0);

	treeLod4Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod4), &lod4);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod4Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod4RenderCount, 0, 0, 0);

	//if (Capture::capturing) return ;
	//graphicsLodPipeline.BindGraphics(commandBuffer);
	//Manager::globalDescriptor.Bind(commandBuffer, graphicsLodPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	//graphicsDescriptor.Bind(commandBuffer, graphicsLodPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	//treeLod5Mesh.Bind(commandBuffer);
	//vkCmdPushConstants(commandBuffer, graphicsLodPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod5), &lod5);
	//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod5Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod5Count, 0, 0, 0);
}

void Trees::RenderShadows(VkCommandBuffer commandBuffer, int cascade)
{
	uint32_t lod0 = 0;
	uint32_t lod1 = 1;
	uint32_t lod2 = 2;
	uint32_t lod3 = 3;

	if (cascade == 0)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, sizeof(uint32_t), sizeof(lod0), &lod0);

		treeLod0Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod0Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod0ShadowCount, 0, 0, 0);

		treeLod1Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod1Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod1ShadowCount, 0, 0, 0);

		//treeLod2Mesh.Bind(commandBuffer);
		//vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod2), &lod2);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod2Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod2Count, 0, 0, 0);
	}
	else if (cascade == 1)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, sizeof(uint32_t), sizeof(lod1), &lod1);

		treeLod0Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod0Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod0ShadowCount, 0, 0, 0);

		treeLod1Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod1Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod1ShadowCount, 0, 0, 0);

		//treeLod2Mesh.Bind(commandBuffer);
		//vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod2), &lod2);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod2Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod2Count, 0, 0, 0);

		// treeLod3Mesh.Bind(commandBuffer);
		// vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod3), &lod3);
		// vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod3Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod3Count, 0, 0, 0);
	}
	else if (cascade == 2)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, sizeof(uint32_t), sizeof(lod2), &lod2);

		treeLod0Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod0Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod0ShadowCount, 0, 0, 0);

		treeLod1Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod1Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod1ShadowCount, 0, 0, 0);

		treeLod2Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod2), &lod2);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod2Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod2ShadowCount, 0, 0, 0);

		treeLod3Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod3), &lod3);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod3Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod3ShadowCount, 0, 0, 0);
	}
	else if (cascade == 3)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, sizeof(uint32_t), sizeof(lod3), &lod3);

		//treeLod0Mesh.Bind(commandBuffer);
		//vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod0Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod0Count, 0, 0, 0);

		//treeLod1Mesh.Bind(commandBuffer);
		//vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod1Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod1Count, 0, 0, 0);

		treeLod2Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod2), &lod2);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod2Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod2ShadowCount, 0, 0, 0);

		treeLod3Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod3), &lod3);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod3Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod3ShadowCount, 0, 0, 0);
	}
}

void Trees::RenderCulling(VkCommandBuffer commandBuffer)
{
	//treeMesh.Bind(commandBuffer);
	//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeMesh.indices.size()), treeRenderCounts[Manager::currentFrame], 0, 0, 0);
}

void Trees::ComputeTreeSetup()
{
	uint32_t computeCount = ceil(float(treeBase) / 8.0f);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computeSetupPipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computeSetupPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeSetupDescriptor.Bind(commandBuffer, computeSetupPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, computeCount, computeCount, 1);

	Manager::currentDevice.EndComputeCommand(commandBuffer);
}

void Trees::ComputeTreeRender(VkCommandBuffer commandBuffer, bool capture)
{
	uint32_t computeCount = ceil(float(treeTotalRenderBase) / 8.0f);
	//if (capture) computeCount = 1;

	//VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();
	bool oneTimeBuffer = commandBuffer == nullptr;
	if (oneTimeBuffer) commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computeRenderPipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computeRenderPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeRenderDescriptor.Bind(commandBuffer, computeRenderPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	//uint32_t isCapturing = 0;
	//if (capture) isCapturing = 1;
	//vkCmdPushConstants(commandBuffer, computeRenderPipeline.computePipelineLayout, COMPUTE_STAGE, 0, sizeof(uint32_t), &isCapturing);

	vkCmdDispatch(commandBuffer, computeCount, computeCount, 1);

	if (oneTimeBuffer)
	{
		Manager::currentDevice.EndComputeCommand(commandBuffer);
		treeRenderCounts[Manager::currentFrame] = *(TreeCountData *)countBuffers[Manager::currentFrame].mappedBuffer;
	}

	//if (!Manager::settings.fullscreen && Time::newSecond)
	//{
	//	std::cout << "lod 0 count: " << treeRenderCounts[Manager::currentFrame].lod0Count << std::endl;
	//	std::cout << "lod 1 count: " << treeRenderCounts[Manager::currentFrame].lod1Count << std::endl;
	//	std::cout << "lod 2 count: " << treeRenderCounts[Manager::currentFrame].lod2Count << std::endl;
	//	std::cout << "lod 3 count: " << treeRenderCounts[Manager::currentFrame].lod3Count << std::endl;
	//	std::cout << std::endl;
	//}
}

void Trees::SetData()
{
	treeRenderCounts[Manager::currentFrame] = *(TreeCountData *)countBuffers[Manager::currentFrame].mappedBuffer;
	//if (Time::newSecond)
	//{
	//	std::cout << "lod 0: " << treeRenderCounts[Manager::currentFrame].lod0Count << std::endl;
	//	std::cout << "lod 1: " << treeRenderCounts[Manager::currentFrame].lod1Count << std::endl;
	//	std::cout << "lod 2: " << treeRenderCounts[Manager::currentFrame].lod2Count << std::endl;
	//	std::cout << std::endl;
	//}
}

void Trees::GenerateTrunkMesh(Mesh &mesh, BranchConfiguration config)
{
	//BranchConfiguration branchConfig;
	//branchConfig.main = true;
	//branchConfig.scale = glm::vec2(1.25, 1.0);

	Shape trunkShape = config.Generate();

	mesh.normal = true;
	mesh.coordinate = true;
	mesh.shape.normal = true;
	mesh.shape.coordinate = true;
	mesh.shape.Join(trunkShape);
	mesh.RecalculateVertices();
}

float BranchConfiguration::GetAngle(glm::vec2 dir)
{
	float angle = 0;

	int signX = glm::sign(dir.x);
	int signZ = glm::sign(dir.y);

	float angleX = signX * dir.x;
	float angleZ = signZ * dir.y;

	if (signX == -1 && signZ != -1)
	{
		angle = (1.0 - angleX) * 90.0 + 270.0;
	}
	else if (signX == -1 && signZ == -1)
	{
		angle = angleX * 90.0 + 180.0;
	}
	else if (signZ == -1)
	{
		angle = (1.0 - angleX) * 90.0 + 90.0;
	}
	else
	{
		angle = angleX * 90.0;
	}

	return (angle);
}

Shape BranchConfiguration::Generate()
{
	float branchSeed = angles.length() + base.length() + offset.y + seed;

	Shape branch = Shape(CYLINDER, resolution);
	branch.Scale(glm::vec3(scale.x, scale.y, scale.x));

	float angleDiff = angles.x - angles.y;
	glm::vec2 vec2Offset = glm::vec2(offset.x, offset.z);
	float sideAngle = GetAngle(Utilities::Normalize(vec2Offset));
	float sideAngleStart = angles.z;

	if (sideAngle - sideAngleStart > 180.0) sideAngle = -(360.0 - sideAngle);

	for (int y = 0; y <= resolution; y++)
	{
		float gradient = float(y) / float(resolution);

		float xAngle = angles.y + angleDiff * 0.5 + angleDiff * 0.5 * gradient;
		glm::mat4 xRotationMatrix = Utilities::GetRotationMatrix(xAngle, glm::vec3(1, 0, 0));

		float yAngle = glm::mix(sideAngleStart, sideAngle, gradient);
		//float yAngle = sideAngle;
		glm::mat4 yRotationMatrix = Utilities::GetRotationMatrix(yAngle, glm::vec3(0, 1, 0));

		// glm::mat4 rotationMatrix = Utilities::GetRotationMatrix(glm::vec3(xAngle, yAngle, 0));
		int l = int(glm::clamp(float(resolution) / 4.0f, 1.0f, float(resolution)));
		int leafRes = int(glm::clamp(float(resolution) / 4.0f, 1.0f, float(resolution)));

		for (int x = 0; x <= resolution; x++)
		{
			int branchIndex = branch.GetPositionIndex(y, x);

			branch.positions[branchIndex] = xRotationMatrix * glm::vec4(branch.positions[branchIndex], 1);
			branch.positions[branchIndex] = yRotationMatrix * glm::vec4(branch.positions[branchIndex], 1);

			branch.normals[branchIndex] = xRotationMatrix * glm::vec4(branch.normals[branchIndex], 0);
			branch.normals[branchIndex] = yRotationMatrix * glm::vec4(branch.normals[branchIndex], 0);

			//branch.coordinates[branchIndex] = glm::vec2(glm::mix(sturdiness.x, sturdiness.y, gradient));
			branch.coordinates[branchIndex] = glm::vec2(sturdiness.y);

			if (y == resolution && x == resolution && branch.centerMergePoint != -1)
			{
				branch.positions[branch.centerMergePoint] = xRotationMatrix * glm::vec4(branch.positions[branch.centerMergePoint], 1);
				branch.positions[branch.centerMergePoint] = yRotationMatrix * glm::vec4(branch.positions[branch.centerMergePoint], 1);

				branch.normals[branch.centerMergePoint] = xRotationMatrix * glm::vec4(branch.normals[branch.centerMergePoint], 0);
				branch.normals[branch.centerMergePoint] = yRotationMatrix * glm::vec4(branch.normals[branch.centerMergePoint], 0);

				//branch.coordinates[branch.centerMergePoint] = glm::vec2(glm::mix(sturdiness.x, sturdiness.y, gradient));
				branch.coordinates[branch.centerMergePoint] = glm::vec2(sturdiness.y);
			}

			if (main)
			{
				int p = x % l;
				float lhalf = l * 0.5;
				float lol = abs(float(p) - lhalf);
				float amount = lol / lhalf;
				branch.positions[branchIndex] = branch.positions[branchIndex] * glm::vec3(1.0 + pow(1.0 - gradient, 8.0) * 1.25 * amount);
			}
		}

		if (leaves && iteration > 1)
		{
			int leafIndex = y % leafRes;
			if (leafIndex == 0 || y == resolution)
			{
				//int density = 1 + iteration / 2;
				int density = 1;
				
				//if (y == resolution) offsetFactor = 1.0f;
				for (int i = 0; i < density; i++)
				{
					//float offsetFactor = i * 0.75f;
					//float offsetFactor = glm::clamp((5.0f - iteration) * 2.0f, 0.0f, 5.0f);
					float offsetFactor = 2.0f;
					if (y == resolution) offsetFactor = 0.0f;
					glm::vec3 leafOffset = glm::vec3(0);
					leafOffset.x = Utilities::Random11(branchSeed + y + i) * 2.0f;
					leafOffset.z = Utilities::Random11(leafOffset.x * 0.5 + (branchSeed + y + i) * 2.0 + i);
					leafOffset.y = Utilities::Random11(leafOffset.x * 2.0 + (branchSeed + y + i) * 0.5 + leafOffset.z + i);
					glm::vec4 leafPosition = glm::vec4(branch.positions[branch.GetPositionIndex(y, 0)] + base + offset + 
						leafOffset * offsetFactor, sturdiness.y);
					Trees::leafPositions.push_back(leafPosition);
					//else if (i == 1) Trees::leafPositions1.push_back(leafPosition);
				}
			}
		}
	}

	branch.Move(base + offset);
	//if (lod == 0 && iteration > 1)
	//{
	//	int leafIterations = 1;
	//	for (int i = 0; i < leafIterations; i++)
	//	{
	//		//float offsetMult = (1.0f - pow(1.0f - (float(i) / float(leafIterations)), 3.0f)) * float(7 - iteration);
	//		//float offsetMult = 1.5f;
	//		//glm::vec3 leafOffset = glm::vec3(0);
	//		//leafOffset.x = Utilities::Random11(branchSeed + i);
	//		//leafOffset.z = Utilities::Random11(leafOffset.x * 0.5 + (branchSeed + i) * 2.0);
	//		//leafOffset.y = Utilities::Random11(leafOffset.x * 2.0 + (branchSeed + i) * 0.5 + leafOffset.z);
	//		//Trees::leafPositions.push_back(branch.TopMergePointsCenter() + leafOffset * offsetMult);
	//		Trees::leafPositions.push_back(branch.TopMergePointsCenter());
	//	}
	//}

	if (scale.x <= minSize || iteration >= maxIteration) splitCount = 0;
	if (splitCount <= 0)
	{
		//if (lod == 0) std::cout << iteration << std::endl;
		//if (lod == 0) std::cout << iteration << std::endl;
		return (branch);
	}

	//int baseBranchReduction = iteration == 0 ? 1 : 0;
	int baseBranchReduction = 0;
	float angleSpacing = 360.0 / (splitCount - baseBranchReduction);
	float angleMax = angleSpacing * angleRandomness;
	//float angleMax = 0.0;
	branchSeed = Utilities::Random11(branchSeed);
	float startAngle = branchSeed * 180.0;
	//float startAngle = 0.0;

	for (int i = 0; i < (splitCount - baseBranchReduction); i++)
	{
		int subResolution = glm::clamp(int(glm::ceil(resolution * 0.5)), 4, resolution);

		branchSeed = Utilities::Random11(branchSeed);
		float newSubAngle = startAngle + (i * angleSpacing) + (branchSeed * angleMax);
		glm::vec3 subOffset = glm::vec3(0, 0, -1);
		subOffset = Utilities::RotateVec(subOffset, newSubAngle, glm::vec3(0, 1, 0));
		subOffset = Utilities::Normalize(subOffset);

		glm::vec3 subBase = branch.TopMergePointsCenter();

		branchSeed = Utilities::Random01(branchSeed);
		subOffset.y = 1.0 + branchSeed;
		subOffset *= (scale.x / mainScale.x + scale.y / mainScale.y) * reach;

		branchSeed = Utilities::Random01(branchSeed);
		float scaleMult = glm::mix(0.9, 1.1, branchSeed);
		glm::vec2 subScale = glm::vec2(scale.x * scaleMult * thickness, scale.y * scaleMult * length);

		float subAngle = angles.x;
		branchSeed = Utilities::Random01(branchSeed);
		//if (i == 0) subAngle += glm::mix(5.0, 25.0, branchSeed);
		subAngle += glm::mix(steepness.x, steepness.y, branchSeed);
		glm::vec3 subAngles = glm::vec3(subAngle, angles.x, sideAngle);

		if (!main) subOffset = Utilities::RotateVec(subOffset, subAngle, glm::vec3(1, 0, 0));
		subOffset = Utilities::RotateVec(subOffset, sideAngle, glm::vec3(0, 1, 0));

		//int subBlendRange = glm::floor(blendRange * 0.75f);
		int subBlendRange = blendRange;

		BranchConfiguration subBranchConfig;
		subBranchConfig.seed = seed;
		subBranchConfig.resolution = subResolution;
		subBranchConfig.base = subBase;
		subBranchConfig.offset = subOffset;
		subBranchConfig.angles = subAngles;
		subBranchConfig.splitCount = splitCount;
		subBranchConfig.scale = subScale;
		subBranchConfig.minSize = minSize;
		subBranchConfig.blendRange = subBlendRange;
		subBranchConfig.splitChance = splitChance;
		subBranchConfig.fallChance = fallChance;
		subBranchConfig.angleRandomness = angleRandomness;
		subBranchConfig.reach = reach;
		subBranchConfig.thickness = thickness;
		subBranchConfig.length = length;
		subBranchConfig.steepness = steepness;
		subBranchConfig.mainScale = mainScale;
		subBranchConfig.maxIteration = maxIteration;
		subBranchConfig.iteration = iteration + 1;
		subBranchConfig.main = false;
		subBranchConfig.lod = lod;
		subBranchConfig.leaves = leaves;
		subBranchConfig.sturdiness.x = sturdiness.y;
		subBranchConfig.sturdiness.y = sturdiness.y + 0.1;

		//if (i == 0) std::cout << iteration << std::endl;

		//if (splitChance > 0.0f)
		//{
		//	branchSeed = Utilities::Random01(branchSeed) * pow((splitCount - 1), 2);
		//	if (branchSeed < splitChance) subBranchConfig.splitCount += 1;
		//}
		//if (fallChance > 0.0)
		//{
		//	branchSeed = Utilities::Random01(branchSeed) / float(splitCount);
		//	if (branchSeed < fallChance) subBranchConfig.splitCount -= 1;
		//}

		Shape subBranch = subBranchConfig.Generate();
		branch.Merge(subBranch, main ? 1 : 0, blendRange);

		// futures[i] = promises[i].get_future();
		// threads[i] = std::thread(GenerateBranchThreaded, subResolution, subBase, subOffset, subScale, subAngles, tsbc, false, &promises[i]);
	}

	branch.CloseUnusedPoints();
	return (branch);
}

void Trees::CaptureTree()
{
	Capture::capturing = true;

	glm::vec3 cameraPosition = Manager::camera.Position();
	glm::vec3 cameraRotation = Manager::camera.Angles();
	glm::mat4 cameraProjection = Manager::camera.Projection();

	float angleStep = 360.0f / float(Capture::captureCount);
	for (int i = 0; i < Capture::captureCount; i++)
	{
		Manager::camera.SetPosition(Utilities::RotateVec(glm::vec3(0.0, 32.5, -100.0), float(i) * -angleStep, glm::vec3(0.0, 1.0, 0.0)));
		Manager::camera.SetRotation(glm::vec3(0.0, 90.0 + (angleStep * float(i)), 0.0));

		//if (i == 0) Manager::camera.SetPosition(glm::vec3(0.0, 32.5, -100.0));
		//if (i == 1) Manager::camera.SetPosition(glm::vec3(-100.0, 32.5, 0.0));
		//if (i == 2) Manager::camera.SetPosition(glm::vec3(0.0, 32.5, 100.0));
		//if (i == 3) Manager::camera.SetPosition(glm::vec3(100.0, 32.5, 0.0));

		//if (i == 0) Manager::camera.SetRotation(glm::vec3(0.0, 90.0, 0.0));
		//if (i == 1) Manager::camera.SetRotation(glm::vec3(0.0, 0.0, 0.0));
		//if (i == 2) Manager::camera.SetRotation(glm::vec3(0.0, -90.0, 0.0));
		//if (i == 3) Manager::camera.SetRotation(glm::vec3(0.0, -180.0, 0.0));

		Manager::camera.SetProjection(Capture::captureProjection);
		Manager::UpdateShaderVariables();

		ComputeTreeRender(nullptr, true);
		//VkCommandBuffer shadowCommandBuffer = Manager::currentDevice.BeginGraphicsCommand();
		//Manager::currentGraphics.RenderShadows(shadowCommandBuffer);
		//Manager::currentDevice.EndGraphicsCommand(shadowCommandBuffer);

		Capture::StartCapturing(i);
		RecordCaptureCommands(Capture::captureCommandBuffer);
		Leaves::RecordCaptureCommands(Capture::captureCommandBuffer);
		Capture::StopCapturing();
	}

	Manager::camera.SetPosition(cameraPosition);
	Manager::camera.SetRotation(cameraRotation);
	Manager::camera.SetProjection(cameraProjection);
	Manager::UpdateShaderVariables();

	Capture::capturing = false;
}

uint32_t Trees::treeBase = 2048;
uint32_t Trees::treeCount = Trees::treeBase * Trees::treeBase;

//uint32_t Trees::treeLod0RenderBase = 8;
//uint32_t Trees::treeLod0RenderCount = Trees::treeLod0RenderBase * Trees::treeLod0RenderBase;
//uint32_t Trees::treeLod1RenderBase = 16;
//uint32_t Trees::treeLod1RenderCount = Trees::treeLod1RenderBase * Trees::treeLod1RenderBase;
//uint32_t Trees::treeLod2RenderBase = 32;
//uint32_t Trees::treeLod2RenderCount = Trees::treeLod2RenderBase * Trees::treeLod2RenderBase;
//uint32_t Trees::treeLod3RenderBase = 64;
//uint32_t Trees::treeLod3RenderCount = Trees::treeLod3RenderBase * Trees::treeLod3RenderBase;
//uint32_t Trees::treeTotalRenderBase = Trees::treeLod3RenderBase;
//uint32_t Trees::treeTotalRenderCount = Trees::treeTotalRenderBase * Trees::treeTotalRenderBase;

uint32_t Trees::treeLod0RenderBase = 8;
uint32_t Trees::treeLod0RenderCount = Trees::treeLod0RenderBase * Trees::treeLod0RenderBase;
uint32_t Trees::treeLod1RenderBase = 16;
//uint32_t Trees::treeLod1RenderBase = 8;
uint32_t Trees::treeLod1RenderCount = Trees::treeLod1RenderBase * Trees::treeLod1RenderBase - Trees::treeLod0RenderCount;
uint32_t Trees::treeLod2RenderBase = 32;
//uint32_t Trees::treeLod2RenderBase = 8;
uint32_t Trees::treeLod2RenderCount = Trees::treeLod2RenderBase * Trees::treeLod2RenderBase - Trees::treeLod0RenderCount - Trees::treeLod1RenderCount;
uint32_t Trees::treeLod3RenderBase = 96;
//uint32_t Trees::treeLod3RenderBase = 8;
uint32_t Trees::treeLod3RenderCount = Trees::treeLod3RenderBase * Trees::treeLod3RenderBase - Trees::treeLod0RenderCount - Trees::treeLod1RenderCount - Trees::treeLod2RenderCount;
uint32_t Trees::treeLod4RenderBase = (192 - 32);
//uint32_t Trees::treeLod4RenderBase = 8;
uint32_t Trees::treeLod4RenderCount = Trees::treeLod4RenderBase * Trees::treeLod4RenderBase - Trees::treeLod0RenderCount - Trees::treeLod1RenderCount - Trees::treeLod2RenderCount - Trees::treeLod3RenderCount;
uint32_t Trees::treeLod5RenderBase = (192 + 128);
uint32_t Trees::treeLod5RenderCount = Trees::treeLod5RenderBase * Trees::treeLod5RenderBase - Trees::treeLod0RenderCount - Trees::treeLod1RenderCount - Trees::treeLod2RenderCount - Trees::treeLod3RenderCount - Trees::treeLod4RenderCount;
uint32_t Trees::treeTotalRenderBase = Trees::treeLod5RenderBase;
uint32_t Trees::treeTotalRenderCount = Trees::treeTotalRenderBase * Trees::treeTotalRenderBase;

std::vector<TreeCountData> Trees::treeRenderCounts;

//std::vector<glm::vec4> Trees::leafPositionsTotal;
//std::vector<glm::vec4> Trees::leafPositions0;
//std::vector<glm::vec4> Trees::leafPositions1;

std::vector<glm::vec4> Trees::leafPositions;

Mesh Trees::treeLod0Mesh;
Mesh Trees::treeLod1Mesh;
Mesh Trees::treeLod2Mesh;
Mesh Trees::treeLod3Mesh;
Mesh Trees::treeLod4Mesh;
Mesh Trees::treeLod5Mesh;

Pipeline Trees::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::graphicsLodPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::shadowPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::cullPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::capturePipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::computeSetupPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::computeRenderPipeline{Manager::currentDevice, Manager::camera};

//Texture Trees::diffuseTexture{Manager::currentDevice};
std::vector<Texture> Trees::treeTextures;

Descriptor Trees::graphicsDescriptor{Manager::currentDevice};
Descriptor Trees::shadowDescriptor{Manager::currentDevice};
Descriptor Trees::cullDescriptor{Manager::currentDevice};
Descriptor Trees::computeSetupDescriptor{Manager::currentDevice};
Descriptor Trees::computeRenderDescriptor{Manager::currentDevice};

Buffer Trees::dataBuffer;
Buffer Trees::leafPositionsBuffer;
std::vector<Buffer> Trees::renderBuffers;
std::vector<Buffer> Trees::shadowBuffers;
std::vector<Buffer> Trees::countBuffers;
std::vector<Buffer> Trees::variableBuffers;

TreeVariables Trees::treeVariables;
uint32_t Trees::totalLeafCount = 0;

bool Trees::treesComputed = false;
bool Trees::shouldUpdateLodTree = false;