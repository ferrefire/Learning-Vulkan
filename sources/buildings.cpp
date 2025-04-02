#include "buildings.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "ui.hpp"
#include "time.hpp"

#include <iostream>
#include <cstdlib>

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

	GenerateBuilding();
}

void Buildings::CreateTextures()
{
    SamplerConfiguration beamSamplerConfig;
	beamSamplerConfig.repeatMode = REPEAT;

    beamTextures.resize(3);
	beamTextures[0].CreateTexture("beam_diff.jpg", beamSamplerConfig);
	beamTextures[1].CreateTexture("beam_norm.jpg", beamSamplerConfig);
	beamTextures[2].CreateTexture("beam_ao.jpg", beamSamplerConfig);
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

	building.mesh.Destroy();
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

void Buildings::Start()
{
	Menu &menu = UI::NewMenu("buildings");
	menu.AddNode("generation config", true);
	menu.AddNode("bounds", true);
	menu.AddRange("width", generationConfig.minSize.x, generationConfig.maxSize.x, 1, 10);
	menu.AddRange("length", generationConfig.minSize.z, generationConfig.maxSize.z, 1, 10);
	menu.AddRange("height", generationConfig.minSize.y, generationConfig.maxSize.y, 1, 10);
	menu.AddNode("bounds", false);
	menu.AddNode("factors", true);
	menu.AddSlider("expansion", generationConfig.expansionFactor, 1, 10);
	menu.AddSlider("level", generationConfig.levelFactor, 1, 10);
	menu.AddSlider("scaffolding", generationConfig.scaffoldingReduction, 1, 10);
	menu.AddNode("factors", false);
	menu.AddInput("seed", generationConfig.seed);
	menu.AddCheck("random", generationConfig.random);
	menu.AddButton("generate", GenerateBuilding);
	menu.AddNode("generation config", false);
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
    building.mesh.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(building.mesh.indices.size()), 1, 0, 0, 0);
}

void Buildings::GenerateBuilding()
{
	GenerateCells();
	GenerateMesh();
}

void Buildings::GenerateCells()
{
	building.cells.clear();
	building.size = glm::ivec3(1, 0, 1);
	if (generationConfig.random) generationConfig.seed = int(uint32_t(Time::GetCurrentTime() * 100) % RAND_MAX);
	random.SetSeed(generationConfig.seed);

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

	for (int i = 0; i < generationConfig.maxSize.y; i++)
	{
		if (i < generationConfig.minSize.y || random.Next(0, 9) < generationConfig.levelFactor)
		{
			building.size.y++;
			ExpandLevel(i);
			FillLevel(i);
		}
		else break;
	}

	SetWalls();
}

void Buildings::ExpandLevel(int level)
{
    glm::ivec2 start;

    start.x = int(round(float(generationConfig.maxSize.x - 1) * 0.5));
    start.y = int(round(float(generationConfig.maxSize.z - 1) * 0.5));

	ExpandCell(level, start.x, start.y, glm::clamp(generationConfig.expansionFactor - level, 1, generationConfig.expansionFactor));
}

void Buildings::FillLevel(int level)
{
	for (int x = 0; x < building.cells[level].size(); x++)
	{
		for (int y = 0; y < building.cells[level][x].size(); y++)
		{
			if (CellEmpty(level, x, y))
			{
				bool N_Empty = CellEmpty(level, x, y + 1);
				bool S_Empty = CellEmpty(level, x, y - 1);
				bool E_Empty = CellEmpty(level, x + 1, y);
				bool W_Empty = CellEmpty(level, x - 1, y);

				if (!N_Empty && !S_Empty && !E_Empty && !W_Empty)
				{
					building.cells[level][x][y].floor.type = FloorType::flat;
				}
			}
		}
	}
}

void Buildings::ExpandCell(int i, int x, int y, int factor)
{
    building.cells[i][x][y].floor.type = FloorType::flat;

	int currentFactor = factor - ((CellEmpty(i - 1, x + 1, y) && i != 0) ? generationConfig.scaffoldingReduction : 0);
	if (ExpansionValid(i, x + 1, y, currentFactor, 1)) ExpandCell(i, x + 1, y, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x - 1, y) && i != 0) ? generationConfig.scaffoldingReduction : 0);
	if (ExpansionValid(i, x - 1, y, currentFactor, 1)) ExpandCell(i, x - 1, y, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x, y + 1) && i != 0) ? generationConfig.scaffoldingReduction : 0);
	if (ExpansionValid(i, x, y + 1, currentFactor, -1)) ExpandCell(i, x, y + 1, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x, y - 1) && i != 0) ? generationConfig.scaffoldingReduction : 0);
	if (ExpansionValid(i, x, y - 1, currentFactor, -1)) ExpandCell(i, x, y - 1, currentFactor);
}

bool Buildings::ExpansionValid(int i, int x, int y, int factor, int increase)
{
    if ((increase == 1 && building.size.x < generationConfig.minSize.x - i) || (increase == -1 && building.size.z < generationConfig.minSize.z - i))
        factor = 10;
    
    bool valid = CellValid(i, x, y) && building.cells[i][x][y].Empty() && random.Next(0, 9) < factor;

    if (valid && increase == 1) building.size.x++;
    else if (valid && increase == -1) building.size.z++;

    return (valid);
}

void Buildings::SetWalls()
{
	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				if (!building.cells[i][x][y].floor.Empty())
				{
					bool N_Empty = FloorEmpty(i, x, y + 1);
					bool S_Empty = FloorEmpty(i, x, y - 1);
					bool E_Empty = FloorEmpty(i, x + 1, y);
					bool W_Empty = FloorEmpty(i, x - 1, y);

					if (N_Empty)
					{
						building.cells[i][x][y].walls.N_Type = WallType::flat;
					}

					if (S_Empty)
					{
						building.cells[i][x][y].walls.S_Type = WallType::flat;
					}

					if (E_Empty)
					{
						building.cells[i][x][y].walls.E_Type = WallType::flat;
					}

					if (W_Empty)
					{
						building.cells[i][x][y].walls.W_Type = WallType::flat;
					}
				}
			}
		}
	}
}

void Buildings::GenerateMesh()
{
	if (building.mesh.created)
		building.mesh.DestroyAtRuntime();

	building.mesh.coordinate = false;
	building.mesh.normal = true;
	building.mesh.shape.coordinate = false;
	building.mesh.shape.normal = true;

	for (int i = 0; i < building.cells.size(); i++)
	{
		GenerateFloors(i);
		GenerateWalls(i);
	}

	building.mesh.RecalculateVertices();
	building.mesh.Create();
}

void Buildings::GenerateFloors(int level)
{
	for (int x = 0; x < building.cells[level].size(); x++)
	{
		for (int y = 0; y < building.cells[level][x].size(); y++)
		{
			if (!building.cells[level][x][y].floor.Empty())
			{
				Shape floor = GenerateFloor(building.cells[level][x][y].floor.type);
				floor.Move(glm::vec3(x, level, y) * 5.0f);
				building.mesh.shape.Join(floor);
			}
		}
	}
}

Shape Buildings::GenerateFloor(FloorType type)
{
	Shape floor = Shape(CUBE);
	floor.Scale(glm::vec3(5.0f, 0.5f, 5.0f));
	
	return (floor);
}

void Buildings::GenerateWalls(int level)
{
	for (int x = 0; x < building.cells[level].size(); x++)
	{
		for (int y = 0; y < building.cells[level][x].size(); y++)
		{
			if (building.cells[level][x][y].walls.N_Type != WallType::empty)
			{
				Shape wall = GenerateWall(building.cells[level][x][y].walls.N_Type, N);
				wall.Move(glm::vec3(x, level, y) * 5.0f);
				building.mesh.shape.Join(wall);
			}
			if (building.cells[level][x][y].walls.S_Type != WallType::empty)
			{
				Shape wall = GenerateWall(building.cells[level][x][y].walls.S_Type, S);
				wall.Move(glm::vec3(x, level, y) * 5.0f);
				building.mesh.shape.Join(wall);
			}
			if (building.cells[level][x][y].walls.E_Type != WallType::empty)
			{
				Shape wall = GenerateWall(building.cells[level][x][y].walls.E_Type, E);
				wall.Move(glm::vec3(x, level, y) * 5.0f);
				building.mesh.shape.Join(wall);
			}
			if (building.cells[level][x][y].walls.W_Type != WallType::empty)
			{
				Shape wall = GenerateWall(building.cells[level][x][y].walls.W_Type, W);
				wall.Move(glm::vec3(x, level, y) * 5.0f);
				building.mesh.shape.Join(wall);
			}
		}
	}
}

Shape Buildings::GenerateWall(WallType type, D direction)
{
	Shape wall = Shape(CUBE);
	wall.Scale(glm::vec3(5.0f, 5.0f, 0.5f));
	wall.Move(glm::vec3(0.0f, 2.5f, 2.5f));
	if (direction == S) wall.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == E) wall.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == W) wall.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	return (wall);
}

bool Buildings::CellValid(int i, int x, int y)
{
    return (i >= 0 && i < generationConfig.maxSize.y && x >= 0 && x < generationConfig.maxSize.x && 
		y >= 0 && y < generationConfig.maxSize.z);
}

bool Buildings::CellEmpty(int i, int x, int y)
{
	return (!CellValid(i, x, y) || building.cells[i][x][y].Empty());
}

bool Buildings::FloorEmpty(int i, int x, int y)
{
	return (!CellValid(i, x, y) || building.cells[i][x][y].floor.Empty());
}

Mesh Buildings::mesh;

std::vector<Texture> Buildings::beamTextures;

Pipeline Buildings::graphicsPipeline{Manager::currentDevice, Manager::camera};

Descriptor Buildings::graphicsDescriptor{Manager::currentDevice};

GenerationConfig Buildings::generationConfig;

Building Buildings::building;

Random Buildings::random;