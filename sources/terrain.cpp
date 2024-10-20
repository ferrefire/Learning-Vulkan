#include "terrain.hpp"

#include "manager.hpp"

void Terrain::Create()
{
    CreateTextures();
    CreateMeshes();
    CreatePipeline();

    object.Resize(glm::vec3(25));
    //object.Rotate(glm::vec3(0, 45.0f, 0));
}

void Terrain::CreateTextures()
{
    noise.Create("perlin_noise_256.jpeg");
}

void Terrain::CreateMeshes()
{
    //mesh.coordinate = true;
    mesh.shape.SetShape(PLANE, 100);
    mesh.RecalculateVertices();

	mesh.Create();
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

    descriptorConfiguration[1].type = IMAGE_SAMPLER;
    descriptorConfiguration[1].stages = VERTEX_STAGE | FRAGMENT_STAGE;
    descriptorConfiguration[1].imageInfo.imageLayout = IMAGE_READ_ONLY;
    descriptorConfiguration[1].imageInfo.imageView = noise.imageView;
    descriptorConfiguration[1].imageInfo.sampler = noise.sampler;

    PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();

    pipeline.Create("terrain", pipelineConfiguration, descriptorConfiguration, mesh.MeshVertexInfo());

    //pipeline.CreateDescriptorSetLayout(descriptorConfiguration);
	//pipeline.CreateGraphicsPipeline("terrain", "terrain", mesh.MeshVertexInfo(), Pipeline::DefaultConfiguration());
	//pipeline.CreateUniformBuffers();
	//pipeline.CreateDescriptorPool(descriptorConfiguration);
	//pipeline.CreateDescriptorSets(descriptorConfiguration);
}

void Terrain::Destroy()
{
    DestroyPipeline();
    DestroyMeshes();
    DestroyTextures();
}

void Terrain::DestroyTextures()
{
	noise.Destroy();
}

void Terrain::DestroyMeshes()
{
	mesh.Destroy();
}

void Terrain::DestroyPipeline()
{
    pipeline.DestroyGraphicsPipeline();

	pipeline.texture.Destroy();

	pipeline.DestroyUniformBuffers();
	pipeline.DestroyDescriptorPool();
	pipeline.DestroyDescriptorSetLayout();
}

void Terrain::RecordCommands(VkCommandBuffer commandBuffer)
{
    pipeline.Bind(commandBuffer, Manager::currentWindow);
    pipeline.UpdateUniformBuffer(object.Translation(), Manager::currentFrame);
    mesh.Bind(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
}

//Pipeline &Terrain::pipeline = Manager::NewPipeline();
//Mesh &Terrain::mesh = Manager::NewMesh();

Pipeline Terrain::pipeline{Manager::currentDevice, Manager::currentCamera};
Texture Terrain::noise{Manager::currentDevice};
Mesh Terrain::mesh{};
Object Terrain::object{&Terrain::mesh, &Terrain::pipeline};