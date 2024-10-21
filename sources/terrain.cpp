#include "terrain.hpp"

#include "manager.hpp"

void Terrain::Create()
{
    CreateTextures();
    CreateMeshes();
	CreateObjects();
    CreatePipeline();

    object.Resize(glm::vec3(25));
    //object.Rotate(glm::vec3(0, 45.0f, 0));
}

void Terrain::CreateTextures()
{
	noiseTexture.CreateTexture("perlin_noise_256.jpeg");

	ImageConfiguration heightMapConfig;
	heightMapConfig.format = R16;
	heightMapConfig.layout = LAYOUT_GENERAL;
	heightMapConfig.width = 512;
	heightMapConfig.height = 512;

	heightMapTexture.CreateImage(heightMapConfig, true);
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
	object.CreateUniformBuffers();
}

void Terrain::CreatePipeline()
{
    //pipeline.CreateDescriptorSetLayout();
    std::vector<DescriptorConfiguration> descriptorConfiguration;
    descriptorConfiguration.resize(2);

    descriptorConfiguration[0].type = UNIFORM_BUFFER;
    descriptorConfiguration[0].stages = VERTEX_STAGE;
    descriptorConfiguration[0].bufferInfo.offset = 0;
	descriptorConfiguration[0].bufferInfo.range = sizeof(UniformBufferObject);
	descriptorConfiguration[0].buffers = &object.uniformBuffers;

	descriptorConfiguration[1].type = IMAGE_SAMPLER;
    descriptorConfiguration[1].stages = VERTEX_STAGE | FRAGMENT_STAGE;
    descriptorConfiguration[1].imageInfo.imageLayout = IMAGE_READ_ONLY;
    descriptorConfiguration[1].imageInfo.imageView = noiseTexture.imageView;
    descriptorConfiguration[1].imageInfo.sampler = noiseTexture.sampler;

    PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();
    VertexInfo vertexInfo = mesh.MeshVertexInfo();

    pipeline.Create("terrain", pipelineConfiguration, descriptorConfiguration, vertexInfo);

    //pipeline.CreateDescriptorSetLayout(descriptorConfiguration);
	//pipeline.CreateGraphicsPipeline("terrain", "terrain", mesh.MeshVertexInfo(), Pipeline::DefaultConfiguration());
	//pipeline.CreateUniformBuffers();
	//pipeline.CreateDescriptorPool(descriptorConfiguration);
	//pipeline.CreateDescriptorSets(descriptorConfiguration);
}

void Terrain::Destroy()
{
    DestroyPipeline();
	DestroyObjects();
    DestroyMeshes();
    DestroyTextures();
}

void Terrain::DestroyTextures()
{
	noiseTexture.Destroy();
	heightMapTexture.Destroy();
}

void Terrain::DestroyMeshes()
{
	mesh.Destroy();
}

void Terrain::DestroyObjects()
{
	object.DestroyUniformBuffers();
}

void Terrain::DestroyPipeline()
{
    pipeline.DestroyGraphicsPipeline();

	//pipeline.texture.Destroy();

	//pipeline.DestroyUniformBuffers();
	pipeline.DestroyDescriptorPool();
	pipeline.DestroyDescriptorSetLayout();
}

void Terrain::RecordCommands(VkCommandBuffer commandBuffer)
{
    pipeline.Bind(commandBuffer, Manager::currentWindow);
    //pipeline.UpdateUniformBuffer(object.Translation(), Manager::currentFrame);
	object.UpdateUniformBuffer(Manager::currentFrame);
    mesh.Bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
}

//Pipeline &Terrain::pipeline = Manager::NewPipeline();
//Mesh &Terrain::mesh = Manager::NewMesh();

Pipeline Terrain::pipeline{Manager::currentDevice, Manager::currentCamera};
Texture Terrain::noiseTexture{Manager::currentDevice};
Texture Terrain::heightMapTexture{Manager::currentDevice};
Mesh Terrain::mesh{};
Object Terrain::object{&Terrain::mesh, &Terrain::pipeline};