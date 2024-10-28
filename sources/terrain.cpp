#include "terrain.hpp"

#include "manager.hpp"

#include <stdexcept>
#include <iostream>

void Terrain::Create()
{
    CreateMeshes();
	CreateGraphicsPipeline();
	CreateComputePipeline();
	CreateTextures();
	CreateObjects();
	CreateGraphicsDescriptor();
	CreateComputeDescriptor();
}

void Terrain::CreateTextures()
{
	SamplerConfiguration grassSamplerConfig;
	grassSamplerConfig.repeatMode = REPEAT;
	grassTexture.CreateTexture("sparse_grass_diff.jpg", grassSamplerConfig);
	//grassTexture.CreateTexture("texture.jpg", grassSamplerConfig);

	//heightMapTexture.CreateTexture("perlin_noise_256.jpeg");

	ImageConfiguration heightMapConfig = Texture::ImageStorage(1024, 1024);
	SamplerConfiguration heightMapSamplerConfig;
	heightMapTexture.CreateImage(heightMapConfig, heightMapSamplerConfig);
	heightMapTexture.TransitionImageLayout(heightMapConfig);
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

	object.Resize(glm::vec3(10000));
}

void Terrain::CreateGraphicsPipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(3);
	descriptorLayoutConfig[0].type = UNIFORM_BUFFER;
	descriptorLayoutConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE;
	descriptorLayoutConfig[1].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[1].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorLayoutConfig[2].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[2].stages = FRAGMENT_STAGE;

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
	pipelineConfiguration.tesselation = true;

    VertexInfo vertexInfo = mesh.MeshVertexInfo();

    graphicsPipeline.CreateGraphicsPipeline("terrain", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Terrain::CreateComputePipeline()
{
	std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = IMAGE_STORAGE;
	descriptorLayoutConfig[0].stages = COMPUTE_STAGE;

	computePipeline.CreateComputePipeline("heightMapCompute", descriptorLayoutConfig);
}

void Terrain::CreateGraphicsDescriptor()
{
	std::vector<DescriptorConfiguration> descriptorConfig(3);

	descriptorConfig[0].type = UNIFORM_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE;
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
	descriptorConfig[1].stages = VERTEX_STAGE | TESSELATION_CONTROL_STAGE | TESSELATION_EVALUATION_STAGE | FRAGMENT_STAGE;
	descriptorConfig[1].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[1].imageInfo.imageView = heightMapTexture.imageView;
	descriptorConfig[1].imageInfo.sampler = heightMapTexture.sampler;

	descriptorConfig[2].type = IMAGE_SAMPLER;
	descriptorConfig[2].stages = FRAGMENT_STAGE;
	descriptorConfig[2].imageInfo.imageLayout = LAYOUT_READ_ONLY;
	descriptorConfig[2].imageInfo.imageView = grassTexture.imageView;
	descriptorConfig[2].imageInfo.sampler = grassTexture.sampler;

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Terrain::CreateComputeDescriptor()
{
	computeDescriptor.perFrame = false;

	std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = IMAGE_STORAGE;
	descriptorConfig[0].stages = COMPUTE_STAGE;
	descriptorConfig[0].imageInfo.imageLayout = LAYOUT_GENERAL;
	descriptorConfig[0].imageInfo.imageView = heightMapTexture.imageView;
	descriptorConfig[0].imageInfo.sampler = heightMapTexture.sampler;

	computeDescriptor.Create(descriptorConfig, computePipeline.objectDescriptorSetLayout);
}

void Terrain::Destroy()
{
	DestroyPipelines();
    DestroyMeshes();
    DestroyTextures();
	DestroyObjects();
	DestroyDescriptors();
}

void Terrain::DestroyTextures()
{
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
	computePipeline.Destroy();
}

void Terrain::DestroyDescriptors()
{
	graphicsDescriptor.Destroy();
	computeDescriptor.Destroy();
}

void Terrain::Start()
{
	ComputeHeightMap();
}

void Terrain::RecordCommands(VkCommandBuffer commandBuffer)
{
    graphicsPipeline.BindGraphics(commandBuffer, Manager::currentWindow);
	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	Manager::UpdateShaderVariables();
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);
	object.UpdateUniformBuffer(Manager::currentFrame);
    mesh.Bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
}

void Terrain::ComputeHeightMap()
{
	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginComputeCommand();
	computePipeline.BindCompute(commandBuffer);
	Manager::globalDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 0);
	Manager::UpdateShaderVariables();
	computeDescriptor.Bind(commandBuffer, computePipeline.computePipelineLayout, COMPUTE_BIND_POINT, 1);
	vkCmdDispatch(commandBuffer, 1024, 1024, 1);
	Manager::currentDevice.EndComputeCommand(commandBuffer);

	//std::cout << "Computed!" << std::endl;
}

Pipeline Terrain::graphicsPipeline{Manager::currentDevice, Manager::currentCamera};
Pipeline Terrain::computePipeline{Manager::currentDevice, Manager::currentCamera};
Texture Terrain::grassTexture{Manager::currentDevice};
Texture Terrain::heightMapTexture{Manager::currentDevice};
Descriptor Terrain::graphicsDescriptor{Manager::currentDevice};
Descriptor Terrain::computeDescriptor{Manager::currentDevice};
Mesh Terrain::mesh{};
Object Terrain::object{&Terrain::mesh, &Terrain::graphicsPipeline};