#include "trees.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "input.hpp"
#include "time.hpp"
#include "terrain.hpp"

#include <iostream>

void Trees::Create()
{
	CreateMeshes();
	CreateGraphicsPipeline();
	if (Manager::settings.shadows) CreateShadowPipeline();
	//CreateCullPipeline();
	CreateComputeSetupPipeline();
	CreateComputeRenderPipeline();
	CreateTextures();
	CreateBuffers();
	CreateGraphicsDescriptor();
	if (Manager::settings.shadows) CreateShadowDescriptor();
	//CreateCullDescriptor();
	CreateComputeSetupDescriptor();
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
	branchConfig.splitCount = 3;
	branchConfig.resolution = 32;
	//branchConfig.blendRange = 8;
	//branchConfig.minSize = 0.75;

	GenerateTrunkMesh(treeLod0Mesh, branchConfig);
	treeLod0Mesh.Create();

	branchConfig.resolution = 16;
	branchConfig.minSize = 0.15;

	GenerateTrunkMesh(treeLod1Mesh, branchConfig);
	treeLod1Mesh.Create();

	branchConfig.resolution = 8;
	branchConfig.minSize = 0.25;

	GenerateTrunkMesh(treeLod2Mesh, branchConfig);
	treeLod2Mesh.Create();

	branchConfig.resolution = 4;
	branchConfig.minSize = 0.5;

	GenerateTrunkMesh(treeLod3Mesh, branchConfig);
	treeLod3Mesh.Create();
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

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.pushConstantCount = 1;
	pipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	pipelineConfiguration.pushConstantSize = sizeof(uint32_t);

	VertexInfo vertexInfo = treeLod0Mesh.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("tree", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
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
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(4);
	descriptorLayoutConfig[0].type = STORAGE_BUFFER;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[1].type = STORAGE_BUFFER;
	descriptorLayoutConfig[1].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[2].type = STORAGE_BUFFER;
	descriptorLayoutConfig[2].stages = COMPUTE_STAGE;
	descriptorLayoutConfig[3].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[3].stages = COMPUTE_STAGE;

	computeRenderPipeline.CreateComputePipeline("treeRenderCompute", descriptorLayoutConfig);
}

void Trees::CreateTextures()
{
	SamplerConfiguration samplerConfig;
	samplerConfig.repeatMode = REPEAT;
	samplerConfig.mipLodBias = 0.0f;

	diffuseTexture.CreateTexture("tree_diff.jpg", samplerConfig);
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
	descriptorConfig[2].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[2].imageInfo.imageView = diffuseTexture.imageView;
	descriptorConfig[2].imageInfo.sampler = diffuseTexture.sampler;

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Trees::CreateShadowDescriptor()
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
	std::vector<DescriptorConfiguration> descriptorConfig(4);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].buffersInfo.resize(1);
	descriptorConfig[0].buffersInfo[0].buffer = dataBuffer.buffer;
	descriptorConfig[0].buffersInfo[0].range = sizeof(TreeData) * treeCount;
	descriptorConfig[0].buffersInfo[0].offset = 0;

	descriptorConfig[1].type = STORAGE_BUFFER;
	descriptorConfig[1].stages = COMPUTE_STAGE;
	descriptorConfig[1].buffersInfo.resize(renderBuffers.size());
	int index = 0;
	for (Buffer &buffer : renderBuffers)
	{
		descriptorConfig[1].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[1].buffersInfo[index].range = sizeof(TreeRenderData) * treeTotalRenderCount;
		descriptorConfig[1].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[2].type = STORAGE_BUFFER;
	descriptorConfig[2].stages = COMPUTE_STAGE;
	descriptorConfig[2].buffersInfo.resize(countBuffers.size());
	index = 0;
	for (Buffer &buffer : countBuffers)
	{
		descriptorConfig[2].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[2].buffersInfo[index].range = sizeof(TreeCountData);
		descriptorConfig[2].buffersInfo[index].offset = 0;
		index++;
	}

	descriptorConfig[3].type = UNIFORM_BUFFER;
	descriptorConfig[3].stages = COMPUTE_STAGE;
	descriptorConfig[3].buffersInfo.resize(variableBuffers.size());
	index = 0;
	for (Buffer &buffer : variableBuffers)
	{
		descriptorConfig[3].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[3].buffersInfo[index].range = sizeof(TreeVariables);
		descriptorConfig[3].buffersInfo[index].offset = 0;
		index++;
	}

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
}

void Trees::DestroyPipelines()
{
	graphicsPipeline.Destroy();
	shadowPipeline.Destroy();
	//cullPipeline.Destroy();
	computeSetupPipeline.Destroy();
	computeRenderPipeline.Destroy();
}

void Trees::DestroyTextures()
{
	diffuseTexture.Destroy();
}

void Trees::DestroyBuffers()
{
	dataBuffer.Destroy();

	for (Buffer &buffer : renderBuffers)
	{
		buffer.Destroy();
	}
	renderBuffers.clear();

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
	treeVariables.treeTotalRenderBase = treeTotalRenderBase;
	treeVariables.treeTotalRenderCount = treeTotalRenderCount;

	treeVariables.spacing = 50;
	treeVariables.spacingMult = 1.0 / treeVariables.spacing;

	treeRenderCounts.resize(Manager::settings.maxFramesInFlight);

	for (Buffer &buffer : variableBuffers)
	{
		memcpy(buffer.mappedBuffer, &treeVariables, sizeof(treeVariables));
	}

	//ComputeTreeSetup();
}

void Trees::Frame()
{
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
	ComputeTreeRender();
}

void Trees::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
	graphicsPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderTrees(commandBuffer);
}

void Trees::RecordShadowCommands(VkCommandBuffer commandBuffer, int cascade)
{
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

void Trees::RenderTrees(VkCommandBuffer commandBuffer)
{
	uint32_t lod0 = 0;
	uint32_t lod1 = 1;
	uint32_t lod2 = 2;
	uint32_t lod3 = 3;

	treeLod0Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod0Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod0Count, 0, 0, 0);

	treeLod1Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod1Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod1Count, 0, 0, 0);

	treeLod2Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod2), &lod2);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod2Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod2Count, 0, 0, 0);

	treeLod3Mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod3), &lod3);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod3Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod3Count, 0, 0, 0);
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
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod0Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod0Count, 0, 0, 0);

		treeLod1Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod1Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod1Count, 0, 0, 0);
	}
	else if (cascade == 1)
	{
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, sizeof(uint32_t), sizeof(lod1), &lod1);

		treeLod0Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod0), &lod0);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod0Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod0Count, 0, 0, 0);

		treeLod1Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod1), &lod1);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod1Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod1Count, 0, 0, 0);

		treeLod2Mesh.Bind(commandBuffer);
		vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod2), &lod2);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod2Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod2Count, 0, 0, 0);

		//treeLod3Mesh.Bind(commandBuffer);
		//vkCmdPushConstants(commandBuffer, graphicsPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(lod3), &lod3);
		//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(treeLod3Mesh.indices.size()), treeRenderCounts[Manager::currentFrame].lod3Count, 0, 0, 0);
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

void Trees::ComputeTreeRender()
{
	uint32_t computeCount = ceil(float(treeTotalRenderBase) / 8.0f);

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();

	computeRenderPipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computeRenderPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	computeRenderDescriptor.Bind(commandBuffer, computeRenderPipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);

	vkCmdDispatch(commandBuffer, computeCount, computeCount, 1);

	Manager::currentDevice.EndComputeCommand(commandBuffer);

	treeRenderCounts[Manager::currentFrame] = *(TreeCountData *)countBuffers[Manager::currentFrame].mappedBuffer;

	//if (!Manager::settings.fullscreen && Time::newSecond)
	//{
	//	std::cout << "lod 0 count: " << treeRenderCounts[Manager::currentFrame].lod0Count << std::endl;
	//	std::cout << "lod 1 count: " << treeRenderCounts[Manager::currentFrame].lod1Count << std::endl;
	//	std::cout << "lod 2 count: " << treeRenderCounts[Manager::currentFrame].lod2Count << std::endl;
	//	std::cout << "lod 3 count: " << treeRenderCounts[Manager::currentFrame].lod3Count << std::endl;
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

		for (int x = 0; x <= resolution; x++)
		{
			branch.positions[branch.GetPositionIndex(y, x)] = xRotationMatrix * glm::vec4(branch.positions[branch.GetPositionIndex(y, x)], 1);
			branch.positions[branch.GetPositionIndex(y, x)] = yRotationMatrix * glm::vec4(branch.positions[branch.GetPositionIndex(y, x)], 1);

			branch.normals[branch.GetPositionIndex(y, x)] = xRotationMatrix * glm::vec4(branch.normals[branch.GetPositionIndex(y, x)], 0);
			branch.normals[branch.GetPositionIndex(y, x)] = yRotationMatrix * glm::vec4(branch.normals[branch.GetPositionIndex(y, x)], 0);

			if (y == resolution && x == resolution && branch.centerMergePoint != -1)
			{
				branch.positions[branch.centerMergePoint] = xRotationMatrix * glm::vec4(branch.positions[branch.centerMergePoint], 1);
				branch.positions[branch.centerMergePoint] = yRotationMatrix * glm::vec4(branch.positions[branch.centerMergePoint], 1);

				branch.normals[branch.centerMergePoint] = xRotationMatrix * glm::vec4(branch.normals[branch.centerMergePoint], 0);
				branch.normals[branch.centerMergePoint] = yRotationMatrix * glm::vec4(branch.normals[branch.centerMergePoint], 0);
			}
		}
	}

	branch.Move(base + offset);

	if (scale.x <= minSize || iteration >= maxIteration) splitCount = 0;
	if (splitCount <= 0) return (branch);

	float angleSpacing = 360.0 / splitCount;
	float angleMax = angleSpacing * angleRandomness;
	//float angleMax = 0.0;
	branchSeed = Utilities::Random11(branchSeed);
	float startAngle = branchSeed * 180.0;
	//float startAngle = 0.0;

	for (int i = 0; i < splitCount; i++)
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

uint32_t Trees::treeBase = 2048;
uint32_t Trees::treeCount = Trees::treeBase * Trees::treeBase;

uint32_t Trees::treeLod0RenderBase = 8;
uint32_t Trees::treeLod0RenderCount = Trees::treeLod0RenderBase * Trees::treeLod0RenderBase;
uint32_t Trees::treeLod1RenderBase = 16;
uint32_t Trees::treeLod1RenderCount = Trees::treeLod1RenderBase * Trees::treeLod1RenderBase;
uint32_t Trees::treeLod2RenderBase = 32;
uint32_t Trees::treeLod2RenderCount = Trees::treeLod2RenderBase * Trees::treeLod2RenderBase;
uint32_t Trees::treeLod3RenderBase = 64;
uint32_t Trees::treeLod3RenderCount = Trees::treeLod3RenderBase * Trees::treeLod3RenderBase;
uint32_t Trees::treeTotalRenderBase = Trees::treeLod0RenderBase + Trees::treeLod1RenderBase + Trees::treeLod2RenderBase + Trees::treeLod3RenderBase;
uint32_t Trees::treeTotalRenderCount = Trees::treeTotalRenderBase * Trees::treeTotalRenderBase;

//uint32_t Trees::treeLod0RenderBase = 4;
//uint32_t Trees::treeLod0RenderCount = Trees::treeLod0RenderBase * Trees::treeLod0RenderBase;
//uint32_t Trees::treeLod1RenderBase = Trees::treeLod0RenderBase + 8;
//uint32_t Trees::treeLod1RenderCount = Trees::treeLod1RenderBase * Trees::treeLod1RenderBase - Trees::treeLod0RenderCount;
//uint32_t Trees::treeLod2RenderBase = Trees::treeLod1RenderBase + 32;
//uint32_t Trees::treeLod2RenderCount = Trees::treeLod2RenderBase * Trees::treeLod2RenderBase - Trees::treeLod0RenderCount - Trees::treeLod1RenderCount;
//uint32_t Trees::treeLod3RenderBase = Trees::treeLod2RenderBase + 64;
//uint32_t Trees::treeLod3RenderCount = Trees::treeLod3RenderBase * Trees::treeLod3RenderBase - Trees::treeLod0RenderCount - Trees::treeLod1RenderCount - Trees::treeLod2RenderCount;
//uint32_t Trees::treeTotalRenderBase = Trees::treeLod3RenderBase;
//uint32_t Trees::treeTotalRenderCount = Trees::treeTotalRenderBase * Trees::treeTotalRenderBase;

std::vector<TreeCountData> Trees::treeRenderCounts;

Mesh Trees::treeLod0Mesh;
Mesh Trees::treeLod1Mesh;
Mesh Trees::treeLod2Mesh;
Mesh Trees::treeLod3Mesh;

Pipeline Trees::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::shadowPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::cullPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::computeSetupPipeline{Manager::currentDevice, Manager::camera};
Pipeline Trees::computeRenderPipeline{Manager::currentDevice, Manager::camera};

Texture Trees::diffuseTexture{Manager::currentDevice};

Descriptor Trees::graphicsDescriptor{Manager::currentDevice};
Descriptor Trees::shadowDescriptor{Manager::currentDevice};
Descriptor Trees::cullDescriptor{Manager::currentDevice};
Descriptor Trees::computeSetupDescriptor{Manager::currentDevice};
Descriptor Trees::computeRenderDescriptor{Manager::currentDevice};

Buffer Trees::dataBuffer;
std::vector<Buffer> Trees::renderBuffers;
std::vector<Buffer> Trees::countBuffers;
std::vector<Buffer> Trees::variableBuffers;

TreeVariables Trees::treeVariables;

bool Trees::treesComputed = false;