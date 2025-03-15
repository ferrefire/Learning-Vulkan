#include "water.hpp"
#include "manager.hpp"

void Water::Create()
{
    CreateMeshes();
    CreatePipelines();
}

void Water::CreateMeshes()
{
    waterMesh.shape.SetShape(PLANE, 100);
    waterMesh.RecalculateVertices();
    waterMesh.Create();
}

void Water::CreateTextures()
{
    SamplerConfiguration normalSamplerConfig;
	normalSamplerConfig.repeatMode = REPEAT;
	normalSamplerConfig.mipLodBias = 0.0f;
	normalSamplerConfig.anisotrophic = VK_TRUE;
	normalSamplerConfig.anisotrophicSampleCount = 4.0;
	normalSamplerConfig.mipLodBias = 0.0f;

    normalTextures.resize(1);


}

void Water::CreatePipelines()
{
    std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(0);

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();

	VertexInfo vertexInfo = waterMesh.MeshVertexInfo();

	graphicsPipeline.CreateGraphicsPipeline("water", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Water::CreateDescriptors()
{
    /*std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = STORAGE_BUFFER;
	descriptorConfig[0].stages = VERTEX_STAGE;
	descriptorConfig[0].buffersInfo.resize(dataBuffers.size());
	int index = 0;
	for (Buffer &buffer : dataBuffers)
	{
		descriptorConfig[0].buffersInfo[index].buffer = buffer.buffer;
		descriptorConfig[0].buffersInfo[index].range = sizeof(LeafData) * Trees::totalLeafCount;
		descriptorConfig[0].buffersInfo[index].offset = 0;
		index++;
	}

	graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);*/
}

Mesh Water::waterMesh{};

std::vector<Texture> Water::normalTextures;

Pipeline Water::graphicsPipeline{Manager::currentDevice, Manager::camera};

Descriptor Water::graphicsDescriptor{Manager::currentDevice};