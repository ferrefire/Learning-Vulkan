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
    mesh.coordinate = true;
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

	SamplerConfiguration plasteredSamplerConfig;
	plasteredSamplerConfig.repeatMode = REPEAT;

	plasteredTextures.resize(3);
	plasteredTextures[0].CreateTexture("plastered_diff.jpg", plasteredSamplerConfig);
	plasteredTextures[1].CreateTexture("plastered_norm.jpg", plasteredSamplerConfig);
	plasteredTextures[2].CreateTexture("plastered_ao.jpg", plasteredSamplerConfig);

	SamplerConfiguration reedSamplerConfig;
	reedSamplerConfig.repeatMode = REPEAT;

	reedTextures.resize(3);
	reedTextures[0].CreateTexture("reed_diff.jpg", reedSamplerConfig);
	reedTextures[1].CreateTexture("reed_norm.jpg", reedSamplerConfig);
	reedTextures[2].CreateTexture("reed_ao.jpg", reedSamplerConfig);
}

void Buildings::CreatePipelines()
{
    std::vector<DescriptorLayoutConfiguration> descriptorLayoutConfig(3);
	int i = 0;
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[i++].count = beamTextures.size();
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[i++].count = plasteredTextures.size();
	descriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	descriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	descriptorLayoutConfig[i++].count = reedTextures.size();

	PipelineConfiguration pipelineConfiguration = Pipeline::DefaultConfiguration();

    VertexInfo vertexInfo = mesh.MeshVertexInfo();

    graphicsPipeline.CreateGraphicsPipeline("building", descriptorLayoutConfig, pipelineConfiguration, vertexInfo);
}

void Buildings::CreateDescriptors()
{
    std::vector<DescriptorConfiguration> descriptorConfig(3);

	int index = 0;
	descriptorConfig[index].type = IMAGE_SAMPLER;
	descriptorConfig[index].stages = FRAGMENT_STAGE;
	descriptorConfig[index].count = beamTextures.size();
	descriptorConfig[index].imageInfos.resize(beamTextures.size());
	for (int i = 0; i < beamTextures.size(); i++)
	{
		descriptorConfig[index].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[index].imageInfos[i].imageView = beamTextures[i].imageView;
		descriptorConfig[index].imageInfos[i].sampler = beamTextures[i].sampler;
	}
	index++;

	descriptorConfig[index].type = IMAGE_SAMPLER;
	descriptorConfig[index].stages = FRAGMENT_STAGE;
	descriptorConfig[index].count = plasteredTextures.size();
	descriptorConfig[index].imageInfos.resize(plasteredTextures.size());
	for (int i = 0; i < plasteredTextures.size(); i++)
	{
		descriptorConfig[index].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[index].imageInfos[i].imageView = plasteredTextures[i].imageView;
		descriptorConfig[index].imageInfos[i].sampler = plasteredTextures[i].sampler;
	}
	index++;

	descriptorConfig[index].type = IMAGE_SAMPLER;
	descriptorConfig[index].stages = FRAGMENT_STAGE;
	descriptorConfig[index].count = reedTextures.size();
	descriptorConfig[index].imageInfos.resize(reedTextures.size());
	for (int i = 0; i < reedTextures.size(); i++)
	{
		descriptorConfig[index].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		descriptorConfig[index].imageInfos[i].imageView = reedTextures[i].imageView;
		descriptorConfig[index].imageInfos[i].sampler = reedTextures[i].sampler;
	}
	index++;

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

	for (Texture &texture : plasteredTextures) texture.Destroy();
    plasteredTextures.clear();

	for (Texture &texture : reedTextures) texture.Destroy();
    reedTextures.clear();
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
	menu.AddNode("part properties", true);
	std::vector<void *> parts = {&floorConfig, &flatWallConfig, &flatRoofConfig, &slopedRoofConfig, &coneRoofConfig, &beamConfig, &collumnConfig};
	for (void *part : parts)
	{
		PartConfig *p = (PartConfig *)part;
		menu.AddNode(p->name.c_str(), true);
		std::string s = "scale##" + p->name;
		menu.AddDrag(s.c_str(), p->scale);
		std::string r = "rotation##" + p->name;
		menu.AddDrag(r.c_str(), p->rotation);
		std::string o = "offset##" + p->name;
		menu.AddDrag(o.c_str(), p->offset);
		menu.AddNode(p->name.c_str(), false);
	}
	menu.AddNode("part properties", false);
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
	SetBeams();
	SetRoof();
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
				bool N_Empty = CellEmpty(level, x, y + 1, false);
				bool S_Empty = CellEmpty(level, x, y - 1, false);
				bool E_Empty = CellEmpty(level, x + 1, y, false);
				bool W_Empty = CellEmpty(level, x - 1, y, false);

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

	int currentFactor = factor - ((CellEmpty(i - 1, x + 1, y) && i != 0) ? generationConfig.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x + 1, y, currentFactor, 1)) ExpandCell(i, x + 1, y, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x - 1, y) && i != 0) ? generationConfig.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x - 1, y, currentFactor, 1)) ExpandCell(i, x - 1, y, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x, y + 1) && i != 0) ? generationConfig.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x, y + 1, currentFactor, -1)) ExpandCell(i, x, y + 1, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x, y - 1) && i != 0) ? generationConfig.scaffoldingReduction - 1 : 0);
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

void Buildings::SetBeams()
{
	for (int i = building.cells.size() - 1; i >= 0; i--)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				if (building.cells[i][x][y].walls.Empty() && !CellEmpty(i + 1, x, y))
				{
					bool N_Empty = CellEmpty(i, x, y + 1);
                    bool S_Empty = CellEmpty(i, x, y - 1);
                    bool E_Empty = CellEmpty(i, x + 1, y);
                    bool W_Empty = CellEmpty(i, x - 1, y);
                    bool NE_Empty = CellEmpty(i, x + 1, y + 1);
                    bool NW_Empty = CellEmpty(i, x - 1, y + 1);
                    bool SE_Empty = CellEmpty(i, x + 1, y - 1);
                    bool SW_Empty = CellEmpty(i, x - 1, y - 1);

					bool N_Beam = BeamPass(i, x, y, N, N_Empty, S_Empty, E_Empty, W_Empty, NE_Empty, NW_Empty, SE_Empty, SW_Empty);
					bool S_Beam = BeamPass(i, x, y, S, N_Empty, S_Empty, E_Empty, W_Empty, NE_Empty, NW_Empty, SE_Empty, SW_Empty);
					bool E_Beam = BeamPass(i, x, y, E, N_Empty, S_Empty, E_Empty, W_Empty, NE_Empty, NW_Empty, SE_Empty, SW_Empty);
					bool W_Beam = BeamPass(i, x, y, W, N_Empty, S_Empty, E_Empty, W_Empty, NE_Empty, NW_Empty, SE_Empty, SW_Empty);

					building.cells[i][x][y].walls.N_Type = N_Beam ? WallType::beams : WallType::empty;
                    building.cells[i][x][y].walls.S_Type = S_Beam ? WallType::beams : WallType::empty;
                    building.cells[i][x][y].walls.E_Type = E_Beam ? WallType::beams : WallType::empty;
                    building.cells[i][x][y].walls.W_Type = W_Beam ? WallType::beams : WallType::empty;
				}
			}
		}
	}
}

bool Buildings::BeamPass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool NE_Empty, bool NW_Empty, bool SE_Empty, bool SW_Empty)
{
	if (direction == N)
	{
		bool N = N_Empty || building.cells[i][x][y + 1].walls.E_Type == WallType::empty;
        bool E = E_Empty || building.cells[i][x + 1][y].walls.W_Type == WallType::empty;
        bool NE = NE_Empty || building.cells[i][x + 1][y + 1].walls.S_Type == WallType::empty;
        return (N && E && NE);
	}
	else if (direction == S)
	{
		bool S = S_Empty || building.cells[i][x][y - 1].walls.W_Type == WallType::empty;
        bool W = W_Empty || building.cells[i][x - 1][y].walls.E_Type == WallType::empty;
        bool SW = SW_Empty || building.cells[i][x - 1][y - 1].walls.N_Type == WallType::empty;
        return (S && W && SW);
	}
	else if (direction == E)
	{
		bool S = S_Empty || building.cells[i][x][y - 1].walls.N_Type == WallType::empty;
        bool E = E_Empty || building.cells[i][x + 1][y].walls.S_Type == WallType::empty;
        bool SE = SE_Empty || building.cells[i][x + 1][y - 1].walls.W_Type == WallType::empty;
        return (S && E && SE);
	}
	else if (direction == W)
	{
		bool N = N_Empty || building.cells[i][x][y + 1].walls.S_Type == WallType::empty;
        bool W = W_Empty || building.cells[i][x - 1][y].walls.N_Type == WallType::empty;
        bool NW = NW_Empty || building.cells[i][x - 1][y + 1].walls.E_Type == WallType::empty;
        return (N && W && NW);
	}
	return (false);
}

void Buildings::SetRoof()
{
	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofTypePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofTypePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofTypePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofDirectionPass(i, x, y);
			}
		}
	}
}

void Buildings::RoofTypePass(int i, int x, int y)
{
	if (IsRoof(i, x, y))
	{
		bool N_Empty = CellEmpty(i, x, y + 1);
		bool S_Empty = CellEmpty(i, x, y - 1);
		bool E_Empty = CellEmpty(i, x + 1, y);
		bool W_Empty = CellEmpty(i, x - 1, y);
		bool N_UpEmpty = CellEmpty(i + 1, x, y + 1);
		bool S_UpEmpty = CellEmpty(i + 1, x, y - 1);
		bool E_UpEmpty = CellEmpty(i + 1, x + 1, y);
		bool W_UpEmpty = CellEmpty(i + 1, x - 1, y);
		int emptyAmount = ((N_Empty && N_UpEmpty) ? 1 : 0) + ((S_Empty && S_UpEmpty) ? 1 : 0) +
			((E_Empty && E_UpEmpty) ? 1 : 0) + ((W_Empty && W_UpEmpty) ? 1 : 0);
		bool N_Cone = !N_Empty && building.cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && building.cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && building.cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && building.cells[i][x - 1][y].roof.type == RoofType::cone;
		int coneAmount = (N_Cone ? 1 : 0) + (S_Cone ? 1 : 0) + (E_Cone ? 1 : 0) + (W_Cone ? 1 : 0);
		int EW_Empty = ((E_Empty || E_Cone) ? 1 : 0) + ((W_Empty || W_Cone) ? 1 : 0);

		if (building.cells[i][x][y].roof.type == RoofType::cone || emptyAmount >= 3)
			building.cells[i][x][y].roof.type = RoofType::cone;
		else if (emptyAmount + coneAmount >= 1 && EW_Empty != 0)
			building.cells[i][x][y].roof.type = RoofType::slope;
		else 
			building.cells[i][x][y].roof.type = RoofType::flatUp;
	}
}

void Buildings::RoofConePass(int i, int x, int y)
{
	if (!building.cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1);
		bool S_Empty = CellEmpty(i, x, y - 1);
		bool E_Empty = CellEmpty(i, x + 1, y);
		bool W_Empty = CellEmpty(i, x - 1, y);
		int emptyAmount = (N_Empty ? 1 : 0) + (S_Empty ? 1 : 0) + (E_Empty ? 1 : 0) + (W_Empty ? 1 : 0);
		bool N_Cone = !N_Empty && building.cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && building.cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && building.cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && building.cells[i][x - 1][y].roof.type == RoofType::cone;
		int coneAmount = (N_Cone ? 1 : 0) + (S_Cone ? 1 : 0) + (E_Cone ? 1 : 0) + (W_Cone ? 1 : 0);
		bool N_Wall = !N_Empty && !CellEmpty(i + 1, x, y + 1);
		bool S_Wall = !N_Empty && !CellEmpty(i + 1, x, y - 1);
		bool E_Wall = !N_Empty && !CellEmpty(i + 1, x + 1, y);
		bool W_Wall = !N_Empty && !CellEmpty(i + 1, x - 1, y);
		int wallAmount = (N_Wall ? 1 : 0) + (S_Wall ? 1 : 0) + (E_Wall ? 1 : 0) + (W_Wall ? 1 : 0);

		if (building.cells[i][x][y].roof.type != RoofType::cone && ((emptyAmount == 2 && coneAmount == 2) ||
            (emptyAmount == 1 && coneAmount == 3) || (emptyAmount == 2 && coneAmount == 1) || 
            (emptyAmount == 1 && ((N_Cone && S_Cone) || (E_Cone && W_Cone))) ||
            (emptyAmount == 1 && coneAmount == 2 && wallAmount == 1))) 
            building.cells[i][x][y].roof.type = RoofType::cone;
	}
}

void Buildings::RoofDirectionPass(int i, int x, int y)
{
	if (!building.cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1, false);
		bool S_Empty = CellEmpty(i, x, y - 1, false);
		bool E_Empty = CellEmpty(i, x + 1, y, false);
		bool W_Empty = CellEmpty(i, x - 1, y, false);
		bool N_UpEmpty = CellEmpty(i + 1, x, y + 1, false);
		bool S_UpEmpty = CellEmpty(i + 1, x, y - 1, false);
		bool E_UpEmpty = CellEmpty(i + 1, x + 1, y, false);
		bool W_UpEmpty = CellEmpty(i + 1, x - 1, y, false);
		bool N_Cone = !N_Empty && building.cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && building.cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && building.cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && building.cells[i][x - 1][y].roof.type == RoofType::cone;

		building.cells[i][x][y].roof.direction = GetRoofDirection(building.cells[i][x][y].roof.type, N_Empty, S_Empty, E_Empty,
			W_Empty, N_Cone, S_Cone, E_Cone, W_Cone, E_UpEmpty, W_UpEmpty);
	}
}

D Buildings::GetRoofDirection(RoofType type, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone,
	bool S_Cone, bool E_Cone, bool W_Cone, bool E_UpEmpty, bool W_UpEmpty)
{
	int NS = (N_Empty ? 0 : 1) + (S_Empty ? 0 : 1);
	int EW = (E_Empty ? 0 : 1) + (W_Empty ? 0 : 1);

	if (type == RoofType::cone)
	{
		if (NS == EW)
		{
			if (!N_Cone && !N_Empty) return (N);
			else if (!E_Cone && !E_Empty) return (E);
			else if (!S_Cone && !S_Empty) return (N);
			else if (!W_Cone && !W_Empty) return (E);
			else if (N_Cone) return (N);
			else if (E_Cone) return (E);
			else if (S_Cone) return (N);
			else if (W_Cone) return (E);
		}
		else return NS > EW ? N : E;
	}
	else if (type == RoofType::slope)
	{
		EW = ((E_Empty || E_Cone) ? 1 : 0) + ((W_Empty || W_Cone) ? 1 : 0);
		if (E_Cone) return (W);
		else if (W_Cone) return (E);
		else if (!E_Empty) return (E);
		else if (!W_Empty) return (W);
		else
		{
			if (E_Empty && E_UpEmpty) return (W);
			else return (E);
		}
	}

	return (N);
}

bool Buildings::IsRoof(int i, int x, int y)
{
	return (CellValid(i, x, y) && !building.cells[i][x][y].Empty(false) && (!CellValid(i + 1, x, y) || 
		building.cells[i + 1][x][y].Empty(false)));
}

void Buildings::GenerateMesh()
{
	if (building.mesh.created)
		building.mesh.DestroyAtRuntime();

	building.mesh.coordinate = true;
	building.mesh.normal = true;
	building.mesh.shape.coordinate = true;
	building.mesh.shape.normal = true;

	for (int i = 0; i < building.cells.size(); i++)
	{
		GenerateFloors(i);
		GenerateWalls(i);
		GenerateRoofs(i);
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
				GenerateFloor(glm::vec3(x, level, y) * generationConfig.scale, building.cells [level][x][y].floor.type);
			}
		}
	}
}

void Buildings::GenerateFloor(glm::vec3 offset, FloorType type)
{
	Shape floor = Shape(CUBE);
	floor.Scale(floorConfig.scale * generationConfig.scale);
	floor.SetCoordinates(glm::vec2(0));
	floor.Move(offset);

	building.mesh.shape.Join(floor);
}

void Buildings::GenerateWalls(int level)
{
	for (int x = 0; x < building.cells[level].size(); x++)
	{
		for (int y = 0; y < building.cells[level][x].size(); y++)
		{
			if (building.cells[level][x][y].walls.N_Type != WallType::empty && building.cells[level][x][y].walls.N_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * generationConfig.scale, building.cells[level][x][y].walls.N_Type, N);
			}
			if (building.cells[level][x][y].walls.S_Type != WallType::empty && building.cells[level][x][y].walls.S_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * generationConfig.scale, building.cells[level][x][y].walls.S_Type, S);
			}
			if (building.cells[level][x][y].walls.E_Type != WallType::empty && building.cells[level][x][y].walls.E_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * generationConfig.scale, building.cells[level][x][y].walls.E_Type, E);
			}
			if (building.cells[level][x][y].walls.W_Type != WallType::empty && building.cells[level][x][y].walls.W_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * generationConfig.scale, building.cells[level][x][y].walls.W_Type, W);
			}

			if (building.cells[level][x][y].walls.Beams())
			{
				GenerateBeams(level, x, y);
			}
		}
	}
}

void Buildings::GenerateWall(glm::vec3 offset, WallType type, D direction)
{
	Shape wall = Shape(CUBE);
	wall.Scale(flatWallConfig.scale * generationConfig.scale);
	wall.Move(flatWallConfig.offset * generationConfig.scale);

	if (direction == S) wall.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == E) wall.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == W) wall.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	wall.SetCoordinates(glm::vec2(1));
	wall.Move(offset);

	building.mesh.shape.Join(wall);
}

void Buildings::GenerateBeams(int i, int x, int y)
{
	Shape beams;
	beams.normal = true;
	beams.coordinate = true;

	Walls walls = building.cells[i][x][y].walls;

	bool N_Empty_B = CellEmpty(i, x, y + 1, false);
    bool N_Empty = CellEmpty(i, x, y + 1);
    bool S_Empty = CellEmpty(i, x, y - 1);
    bool E_Empty_B = CellEmpty(i, x + 1, y, false);
    bool E_Empty = CellEmpty(i, x + 1, y);
    bool W_Empty = CellEmpty(i, x - 1, y);

	if (i != 0)
    {
        if (N_Empty_B)
		{
			Shape beam = Shape(CUBE);
			beam.SetCoordinates(glm::vec2(0));
			beam.Scale(beamConfig.scale * generationConfig.scale);
			beam.Move(beamConfig.offset * generationConfig.scale);
			beams.Join(beam);
		}
		if (E_Empty_B)
		{
			Shape beam = Shape(CUBE);
			beam.SetCoordinates(glm::vec2(0));
			beam.Scale(beamConfig.scale * generationConfig.scale);
			beam.Move(beamConfig.offset * generationConfig.scale);
			beam.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			beams.Join(beam);
		}
		if (S_Empty)
		{
			Shape beam = Shape(CUBE);
			beam.SetCoordinates(glm::vec2(0));
			beam.Scale(beamConfig.scale * generationConfig.scale);
			beam.Move(beamConfig.offset * generationConfig.scale);
			beam.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			beams.Join(beam);
		}
		if (W_Empty)
		{
			Shape beam = Shape(CUBE);
			beam.SetCoordinates(glm::vec2(0));
			beam.Scale(beamConfig.scale * generationConfig.scale);
			beam.Move(beamConfig.offset * generationConfig.scale);
			beam.Rotate(270.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			beams.Join(beam);
		}
    }

	if (walls.N_Type == WallType::beams)
	{
		Shape beam = Shape(CUBE);
		beam.SetCoordinates(glm::vec2(0));
		beam.Scale(collumnConfig.scale * generationConfig.scale);
		beam.Move(collumnConfig.offset * generationConfig.scale);
		beams.Join(beam);
	}
	if (walls.E_Type == WallType::beams)
	{
		Shape beam = Shape(CUBE);
		beam.SetCoordinates(glm::vec2(0));
		beam.Scale(collumnConfig.scale * generationConfig.scale);
		beam.Move(collumnConfig.offset * generationConfig.scale);
		beam.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		beams.Join(beam);
	}
	if (walls.S_Type == WallType::beams)
	{
		Shape beam = Shape(CUBE);
		beam.SetCoordinates(glm::vec2(0));
		beam.Scale(collumnConfig.scale * generationConfig.scale);
		beam.Move(collumnConfig.offset * generationConfig.scale);
		beam.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		beams.Join(beam);
	}
	if (walls.W_Type == WallType::beams)
	{
		Shape beam = Shape(CUBE);
		beam.SetCoordinates(glm::vec2(0));
		beam.Scale(collumnConfig.scale * generationConfig.scale);
		beam.Move(collumnConfig.offset * generationConfig.scale);
		beam.Rotate(270.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		beams.Join(beam);
	}

	beams.Move(glm::vec3(x, i, y) * generationConfig.scale);
	building.mesh.shape.Join(beams);
}

void Buildings::GenerateRoofs(int level)
{
	for (int x = 0; x < building.cells[level].size(); x++)
	{
		for (int y = 0; y < building.cells[level][x].size(); y++)
		{
			if (!building.cells[level][x][y].roof.Empty())
			{
				GenerateRoof(glm::vec3(x, level + 1, y) * generationConfig.scale, building.cells[level][x][y].roof.type,
					building.cells[level][x][y].roof.direction);
			}
		}
	}
}

void Buildings::GenerateRoof(glm::vec3 offset, RoofType type, D direction)
{
	Shape roof;

	if (type == RoofType::flatUp)
	{
		roof = Shape(CUBE);
		roof.SetCoordinates(glm::vec2(2));
		roof.Scale(flatRoofConfig.scale * generationConfig.scale);
		roof.Move(flatRoofConfig.offset * generationConfig.scale);
	}
	else if (type == RoofType::slope)
	{
		roof = Shape(CUBE);
		roof.SetCoordinates(glm::vec2(2));
		roof.Scale(slopedRoofConfig.scale * generationConfig.scale);
		roof.Rotate(slopedRoofConfig.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Move(slopedRoofConfig.offset * generationConfig.scale);
	}
	else if (type == RoofType::cone)
	{
		Shape leftRoof = Shape(CUBE);
		leftRoof.SetCoordinates(glm::vec2(2));
		leftRoof.Scale(coneRoofConfig.scale * generationConfig.scale);
		leftRoof.Rotate(coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Move(coneRoofConfig.offset * generationConfig.scale);

		Shape rightRoof = Shape(CUBE);
		rightRoof.SetCoordinates(glm::vec2(2));
		rightRoof.Scale(coneRoofConfig.scale * generationConfig.scale);
		rightRoof.Rotate(-coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Move(coneRoofConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * generationConfig.scale);

		roof.normal = true;
		roof.coordinate = true;
		roof.Join(leftRoof);
		roof.Join(rightRoof);
		roof.SetCoordinates(glm::vec2(2));
	}

	if (direction == S) roof.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == E) roof.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == W) roof.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	roof.Move(offset);

	building.mesh.shape.Join(roof);
}

bool Buildings::CellValid(int i, int x, int y)
{
    return (i >= 0 && i < generationConfig.maxSize.y && x >= 0 && x < generationConfig.maxSize.x && 
		y >= 0 && y < generationConfig.maxSize.z);
}

bool Buildings::CellEmpty(int i, int x, int y, bool countBeams)
{
	return (!CellValid(i, x, y) || building.cells[i][x][y].Empty(countBeams));
}

bool Buildings::FloorEmpty(int i, int x, int y)
{
	return (!CellValid(i, x, y) || building.cells[i][x][y].floor.Empty());
}

Mesh Buildings::mesh;

std::vector<Texture> Buildings::beamTextures;
std::vector<Texture> Buildings::plasteredTextures;
std::vector<Texture> Buildings::reedTextures;

Pipeline Buildings::graphicsPipeline{Manager::currentDevice, Manager::camera};

Descriptor Buildings::graphicsDescriptor{Manager::currentDevice};

GenerationConfig Buildings::generationConfig;

Building Buildings::building;

Random Buildings::random;

PartConfig Buildings::floorConfig{"floor", glm::vec3(1.0f, 0.1f, 1.0f)};
PartConfig Buildings::flatWallConfig{"flat wall", glm::vec3(1.0f, 1.0f, 0.1f), glm::vec3(0.0f), glm::vec3(0.0f, 0.5f, 0.5f)};
PartConfig Buildings::flatRoofConfig{"flat roof", glm::vec3(1.0f, 0.1f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.625f, 0.0f)};
PartConfig Buildings::slopedRoofConfig{"sloped roof", glm::vec3(1.0f, 0.1f, 1.25f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.3f, 0.0f)};
PartConfig Buildings::coneRoofConfig{"cone roof", glm::vec3(0.635f, 0.1f, 1.0f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(-0.26f, 0.135f, 0.0f)};
PartConfig Buildings::beamConfig{"beam", glm::vec3(1.25f, 0.15f, 0.15f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.6f)};
PartConfig Buildings::collumnConfig{"collumn", glm::vec3(0.15f, 1.0f, 0.15f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.6f, 0.5f, 0.6f)};