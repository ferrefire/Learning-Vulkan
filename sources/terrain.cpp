#include "terrain.hpp"

#include "manager.hpp"

void Terrain::Create()
{
    CreateMesh();
    CreatePipeline();

    //object.Resize(glm::vec3(3));
    //object.Rotate(glm::vec3(0, 45.0f, 0));
}

void Terrain::CreateMesh()
{
    mesh.shape.SetShape(CUBE);
    mesh.RecalculateVertices();

	mesh.Create();
}

void Terrain::CreatePipeline()
{
    pipeline.CreateDescriptorSetLayout();
	pipeline.CreateGraphicsPipeline("simple", "simple", Manager::currentWindow.renderPass);

	pipeline.texture.Create("texture.jpg", &Manager::currentDevice);

	pipeline.CreateUniformBuffers();
	pipeline.CreateDescriptorPool();
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

void Terrain::RecordCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    pipeline.Bind(commandBuffer, Manager::currentWindow);
    pipeline.UpdateUniformBuffer(object.Translation(), imageIndex);
    mesh.Bind(commandBuffer);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
}

//Pipeline &Terrain::pipeline = Manager::NewPipeline();
//Mesh &Terrain::mesh = Manager::NewMesh();

Pipeline Terrain::pipeline{Manager::currentDevice, Manager::currentCamera};
Mesh Terrain::mesh{};
Object Terrain::object{&Terrain::mesh, &Terrain::pipeline};