#include "terrain.hpp"

#include "manager.hpp"

#include <stdexcept>

void Terrain::Create()
{
    CreateMeshes();
	CreateGraphicsPipeline();
	CreateTextures();
	CreateObjects();
	CreateDescriptor();
	//CreateComputePipeline();
}

void Terrain::CreateTextures()
{
	if (!graphicsPipeline.descriptorSetLayout)
		throw std::runtime_error("cannot create terrain textures because pipeline descriptor set layout does not exist");

	SamplerConfiguration grassSamplerConfig;
	grassSamplerConfig.repeatMode = REPEAT;
	grassTexture.CreateTexture("sparse_grass_diff.jpg", grassSamplerConfig);

	heightMapTexture.CreateTexture("perlin_noise_256.jpeg");

	//std::vector<DescriptorConfiguration> descriptorConfig(2);
	//descriptorConfig[0].type = IMAGE_SAMPLER;
	//descriptorConfig[0].stages = VERTEX_STAGE | FRAGMENT_STAGE;
	//descriptorConfig[0].imageInfo.imageLayout = IMAGE_READ_ONLY;
	//descriptorConfig[0].imageInfo.imageView = heightMapTexture.imageView;
	//descriptorConfig[0].imageInfo.sampler = heightMapTexture.sampler;
	//descriptorConfig[1].type = IMAGE_SAMPLER;
	//descriptorConfig[1].stages = FRAGMENT_STAGE;
	//descriptorConfig[1].imageInfo.imageLayout = IMAGE_READ_ONLY;
	//descriptorConfig[1].imageInfo.imageView = grassTexture.imageView;
	//descriptorConfig[1].imageInfo.sampler = grassTexture.sampler;

	//descriptor.Create(descriptorConfig, graphicsPipeline.descriptorSetLayout, 1);

	//ImageConfiguration heightMapConfig;
	//heightMapConfig.format = R16;
	//heightMapConfig.layout = LAYOUT_GENERAL;
	//heightMapConfig.width = 512;
	//heightMapConfig.height = 512;
	//heightMapTexture.CreateImage(heightMapConfig, true);
}

void Terrain::CreateMeshes()
{
    //mesh.coordinate = true;
    mesh.shape.SetShape(PLANE, 100);
    mesh.RecalculateVertices();

	mesh.Create();
}

void Terrain::CreateObjects()
{
	object.Create();

	object.Resize(glm::vec3(100));
}

void Terrain::CreateGraphicsPipeline()
{
	/*
    std::vector<DescriptorConfiguration> descriptorConfiguration;
    descriptorConfiguration.resize(3);

    descriptorConfiguration[0].type = UNIFORM_BUFFER;
    descriptorConfiguration[0].stages = VERTEX_STAGE;
    descriptorConfiguration[0].bufferInfo.offset = 0;
	descriptorConfiguration[0].bufferInfo.range = sizeof(UniformBufferObject);
	descriptorConfiguration[0].buffers = &object.uniformBuffers;

	descriptorConfiguration[1].type = IMAGE_SAMPLER;
    descriptorConfiguration[1].stages = VERTEX_STAGE | FRAGMENT_STAGE;
    descriptorConfiguration[1].imageInfo.imageLayout = IMAGE_READ_ONLY;
    descriptorConfiguration[1].imageInfo.imageView = heightMapTexture.imageView;
    descriptorConfiguration[1].imageInfo.sampler = heightMapTexture.sampler;

	descriptorConfiguration[2].type = IMAGE_SAMPLER;
	descriptorConfiguration[2].stages = FRAGMENT_STAGE;
	descriptorConfiguration[2].imageInfo.imageLayout = IMAGE_READ_ONLY;
	descriptorConfiguration[2].imageInfo.imageView = grassTexture.imageView;
	descriptorConfiguration[2].imageInfo.sampler = grassTexture.sampler;
	*/

	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(3);
	descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE;
	descriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[2].stages = FRAGMENT_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();

    VertexInfo vertexInfo = mesh.MeshVertexInfo();

    graphicsPipeline.Create("terrain", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Terrain::CreateComputePipeline()
{
	std::vector<DescriptorConfiguration> descriptorConfiguration;
	descriptorConfiguration.resize(1);

	descriptorConfiguration[0].type = IMAGE_STORAGE;
	descriptorConfiguration[0].stages = COMPUTE_STAGE;
	descriptorConfiguration[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfiguration[0].imageInfo.imageView = heightMapTexture.imageView;
	descriptorConfiguration[0].imageInfo.sampler = heightMapTexture.sampler;

	//PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	//VertexInfo vertexInfo = mesh.MeshVertexInfo();

	//graphicsPipeline.Create("terrain", pipelineConfiguration, descriptorConfiguration, vertexInfo);
}

void Terrain::CreateDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(3);

	descriptorConfig[0].type = UNIFORM_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(object.uniformBuffers.size());
	int i = 0;
	for (Buffer &buffer : object.uniformBuffers)
	{
		descriptorConfig[0].buffersInfo[i].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[i].range = sizeof(UniformBufferObject);
		descriptorConfig[0].buffersInfo[i].offset = 0;
		i++;
	}

	descriptorConfig[1].type = IMAGE_SAMPLER;
	descriptorConfig[1].stages = VERTEX_STAGE | FRAGMENT_STAGE;
	descriptorConfig[1].imageInfo.imageLayout = IMAGE_READ_ONLY;
	descriptorConfig[1].imageInfo.imageView = heightMapTexture.imageView;
	descriptorConfig[1].imageInfo.sampler = heightMapTexture.sampler;

	descriptorConfig[2].type = IMAGE_SAMPLER;
	descriptorConfig[2].stages = FRAGMENT_STAGE;
	descriptorConfig[2].imageInfo.imageLayout = IMAGE_READ_ONLY;
	descriptorConfig[2].imageInfo.imageView = grassTexture.imageView;
	descriptorConfig[2].imageInfo.sampler = grassTexture.sampler;

	descriptor.Create(descriptorConfig, graphicsPipeline.descriptorSetLayout);
}

void Terrain::Destroy()
{
	DestroyPipelines();
    DestroyMeshes();
    DestroyTextures();
	DestroyObjects();
	DestroyDescriptor();
}

void Terrain::DestroyTextures()
{
	//descriptor.Destroy();

	grassTexture.Destroy();
	heightMapTexture.Destroy();
}

void Terrain::DestroyMeshes()
{
	mesh.Destroy();
}

void Terrain::DestroyObjects()
{
	object.Destroy();
}

void Terrain::DestroyPipelines()
{
	graphicsPipeline.Destroy();

    //graphicsPipeline.DestroyGraphicsPipeline();
	//graphicsPipeline.DestroyDescriptorPool();
	//graphicsPipeline.DestroyDescriptorSetLayout();

	//computePipeline.DestroyGraphicsPipeline();
	//computePipeline.DestroyDescriptorPool();
	//computePipeline.DestroyDescriptorSetLayout();
}

void Terrain::DestroyDescriptor()
{
	descriptor.Destroy();
}

void Terrain::RecordCommands(VkCommandBuffer commandBuffer)
{
    graphicsPipeline.Bind(commandBuffer, Manager::currentWindow);
	descriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout);
	object.UpdateUniformBuffer(Manager::currentFrame);
	//descriptor.Bind(commandBuffer, graphicsPipeline.computePipelineLayout, 1);
    mesh.Bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
}

Pipeline Terrain::graphicsPipeline{Manager::currentDevice, Manager::currentCamera};
Pipeline Terrain::computePipeline{Manager::currentDevice, Manager::currentCamera};
Texture Terrain::grassTexture{Manager::currentDevice};
Texture Terrain::heightMapTexture{Manager::currentDevice};
Descriptor Terrain::descriptor{Manager::currentDevice};
Mesh Terrain::mesh{};
Object Terrain::object{&Terrain::mesh, &Terrain::graphicsPipeline};