#include "buildings.hpp"

#include "manager.hpp"
#include "utilities.hpp"

void Buildings::Create()
{
    CreateMeshes();
    CreateTextures();
    CreatePipelines();
    CreateDescriptors();
}

void Buildings::CreateMeshes()
{
    mesh.coordinate = false;
    mesh.normal = true;
    mesh.shape.SetShape(CUBE);
    mesh.RecalculateVertices();
    mesh.Create();
}

void Buildings::CreateTextures()
{
    SamplerConfiguration beamSamplerConfig;
	beamSamplerConfig.repeatMode = REPEAT;

    beamTextures.resize(1);
    beamTextures[0].CreateTexture("beam_diff.jpg", beamSamplerConfig);
}

void Buildings::CreatePipelines()
{
    std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(1);
	descriptorLayoutConfig[0].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[0].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[0].count = beamTextures.size();

    PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();

    VertexInfo vertexInfo = mesh.MeshVertexInfo();

    graphicsPipeline.CreateGraphicsPipeline("building", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Buildings::CreateDescriptors()
{
    std::vector<DescriptorConfiguration> descriptorConfig(1);

	descriptorConfig[0].type = IMAGE_SAMPLER;
	descriptorConfig[0].stages = FRAGMENT_STAGE;
	descriptorConfig[0].count = beamTextures.size();
	descriptorConfig[0].imageInfos.resize(beamTextures.size());
	for (int i = 0; i < beamTextures.size(); i++)
	{
		descriptorConfig[0].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[0].imageInfos[i].imageView = beamTextures[i].imageView;
		descriptorConfig[0].imageInfos[i].sampler = beamTextures[i].sampler;
	}

    graphicsDescriptor.perFrame = false;
    graphicsDescriptor.Create(descriptorConfig, graphicsPipeline.objectDescriptorSetLayout);
}

void Buildings::Destroy()
{
    DestroyMeshes();
    DestroyTextures();
    DestroyPipelines();
    DestroyDescriptors();
}

void Buildings::DestroyMeshes()
{
    mesh.Destroy();
}

void Buildings::DestroyTextures()
{
    for (Texture &texture : beamTextures) texture.Destroy();
    beamTextures.clear();
}

void Buildings::DestroyPipelines()
{
    graphicsPipeline.Destroy();
}

void Buildings::DestroyDescriptors()
{
    graphicsDescriptor.Destroy();
}

void Buildings::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
    graphicsPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderBuildings(commandBuffer);
}

void Buildings::RenderBuildings(VkCommandBuffer commandBuffer)
{
    mesh.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
}

void Buildings::GenerateBuilding(Building &building)
{

}

void Buildings::GenerateCells(Building &building)
{
    building.cells.resize(generationConfig.maxSize.y);
    for (int i = 0; i < generationConfig.maxSize.y; i++)
    {
        building.cells[i].resize(generationConfig.maxSize.x);
        for (int x = 0; x < generationConfig.maxSize.x; x++)
        {
            building.cells[i][x].resize(generationConfig.maxSize.z);
            for (int y = 0; y < generationConfig.maxSize.z; y++)
            {
                
            }
        }
    }


}

void Buildings::ExpandLevel(int level, Building &building)
{
    glm::ivec2 start;

    start.x = int(round(float(generationConfig.maxSize.x - 1) * 0.5));
    start.y = int(round(float(generationConfig.maxSize.z - 1) * 0.5));
}

void Buildings::ExpandCell(int i, int x, int y, Building &building)
{
    building.cells[i][x][y].floor.type = FloorType::flat;

   
}

bool Buildings::ExpansionValid(int i, int x, int y, int factor, int increase, Building &building)
{
    if ((increase == 1 && building.size.x < generationConfig.minSize.x - i) || (increase == -1 && building.size.z < generationConfig.minSize.z - i))
        factor = 10;
    
    bool valid = CellValid(i, x, y) && building.cells[i][x][y].Empty() && Utilities::RandomInt(0, 9) < factor;

    if (valid && increase == 1) building.size.x++;
    else if (valid && increase == -1) building.size.z++;

    return (valid);
}

bool Buildings::CellValid(int i, int x, int y)
{
    return (i >= 0 && i < generationConfig.maxSize.y && x >= 0 && x < generationConfig.maxSize.x && y >= 0 && y < generationConfig.maxSize.z);
}

Mesh Buildings::mesh;

std::vector<Texture> Buildings::beamTextures;

Pipeline Buildings::graphicsPipeline{Manager::currentDevice, Manager::camera};

Descriptor Buildings::graphicsDescriptor{Manager::currentDevice};

GenerationConfig Buildings::generationConfig;