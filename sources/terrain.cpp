#include "terrain.hpp"

#include "manager.hpp"

void Terrain::Create()
{
    CreateMesh();
    CreatePipeline();

    object.Resize(glm::vec3(25));
    //object.Rotate(glm::vec3(0, 45.0f, 0));
}

void Terrain::CreateMesh()
{
    //mesh.coordinate = true;
    mesh.shape.SetShape(PLANE, 25);
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
    descriptorConfiguration[1].type = IMAGE_SAMPLER;
    descriptorConfiguration[1].stages = FRAGMENT_STAGE;

    pipeline.CreateDescriptorSetLayout(descriptorConfiguration);

	pipeline.CreateGraphicsPipeline("terrain", "terrain", mesh.MeshVertexInfo(), Pipeline::DefaultConfiguration());

	pipeline.texture.Create("perlin_noise_256.jpeg", &Manager::currentDevice);

	pipeline.CreateUniformBuffers();

	//pipeline.CreateDescriptorPool();
	pipeline.CreateDescriptorPool(descriptorConfiguration);

	pipeline.CreateDescriptorSets();
}

void Terrain::Destroy()
{
    DestroyPipeline();
    DestroyMesh();
}

void Terrain::DestroyMesh()
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
Mesh Terrain::mesh{};
Object Terrain::object{&Terrain::mesh, &Terrain::pipeline};