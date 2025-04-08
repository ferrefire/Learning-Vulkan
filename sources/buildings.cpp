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
	//generationConfig.seed = 1472;
	GenerateBuilding();
	//building.mesh.coordinate = true;
	//building.mesh.normal = true;
	//building.mesh.color = true;
	//building.mesh.shape.coordinate = true;
	//building.mesh.shape.normal = true;
	//building.mesh.shape.color = true;
	//building.mesh.shape.SetShape(PRISM);
	//building.mesh.RecalculateVertices();
	//building.mesh.Create();
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
    std::vector<DescriptorLayoutConfiguration> graphicsDescriptorLayoutConfig(3);
	int i = 0;
	graphicsDescriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	graphicsDescriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	graphicsDescriptorLayoutConfig[i++].count = beamTextures.size();
	graphicsDescriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	graphicsDescriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	graphicsDescriptorLayoutConfig[i++].count = plasteredTextures.size();
	graphicsDescriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	graphicsDescriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	graphicsDescriptorLayoutConfig[i++].count = reedTextures.size();
	PipelineConfiguration graphicsPipelineConfiguration = Pipeline::DefaultConfiguration();
    VertexInfo vertexInfo = building.mesh.MeshVertexInfo();
    graphicsPipeline.CreateGraphicsPipeline("building", graphicsDescriptorLayoutConfig, graphicsPipelineConfiguration, vertexInfo);

	std::vector<DescriptorLayoutConfiguration> shadowDescriptorLayoutConfig(0);
	PipelineConfiguration shadowPipelineConfiguration = Pipeline::DefaultConfiguration();
	shadowPipelineConfiguration.shadow = true;
	shadowPipelineConfiguration.pushConstantCount = 1;
	shadowPipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	shadowPipelineConfiguration.pushConstantSize = sizeof(uint32_t);
    shadowPipeline.CreateGraphicsPipeline("buildingShadow", shadowDescriptorLayoutConfig, shadowPipelineConfiguration, vertexInfo);
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
	shadowPipeline.Destroy();
}

void Buildings::DestroyDescriptors()
{
    graphicsDescriptor.Destroy();
	shadowDescriptor.Destroy();
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
	std::vector<void *> parts = {&floorConfig, &flatWallConfig, &slopedWallConfig, &flatRoofConfig, &slopedRoofConfig, &coneRoofConfig, 
		&coneRoofExtensionConfig, &beamConfig, &slopedBeamConfig, &slightSlopedBeamConfig, &collumnConfig};
	for (void *part : parts)
	{
		PartConfig *p = (PartConfig *)part;
		menu.AddNode(p->name.c_str(), true);
		std::string s = "scale##" + p->name;
		menu.AddDrag(s.c_str(), p->scale, 0.005f);
		std::string r = "rotation##" + p->name;
		menu.AddDrag(r.c_str(), p->rotation, 0.1f);
		std::string o = "offset##" + p->name;
		menu.AddDrag(o.c_str(), p->offset, 0.005f);
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

void Buildings::RecordShadowCommands(VkCommandBuffer commandBuffer, int cascade)
{
	if (cascade > 1) return;

	shadowPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	//shadowDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderShadows(commandBuffer, cascade);
}

void Buildings::RenderBuildings(VkCommandBuffer commandBuffer)
{
    building.mesh.Bind(commandBuffer);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(building.mesh.indices.size()), 1, 0, 0, 0);
}

void Buildings::RenderShadows(VkCommandBuffer commandBuffer, int cascade)
{
	uint32_t cascadeIndex = cascade;

    building.mesh.Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(cascadeIndex), &cascadeIndex);
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

	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofMergePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building.cells.size(); i++)
	{
		for (int x = 0; x < building.cells[i].size(); x++)
		{
			for (int y = 0; y < building.cells[i][x].size(); y++)
			{
				RoofExtendPass(i, x, y);
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

void Buildings::RoofMergePass(int i, int x, int y)
{
	if (!building.cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1);
		bool S_Empty = CellEmpty(i, x, y - 1);
		bool E_Empty = CellEmpty(i, x + 1, y);
		bool W_Empty = CellEmpty(i, x - 1, y);
		bool N_Cone = !N_Empty && building.cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && building.cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && building.cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && building.cells[i][x - 1][y].roof.type == RoofType::cone;

		if (building.cells[i][x][y].roof.type == RoofType::cone)
		{
			if (building.cells[i][x][y].roof.direction == N)
			{
				building.cells[i][x][y].roof.N_Merge = MergePass(i, x, y, N, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
				building.cells[i][x][y].roof.S_Merge = MergePass(i, x, y, S, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
			}
			else if (building.cells[i][x][y].roof.direction == E)
			{
				building.cells[i][x][y].roof.E_Merge = MergePass(i, x, y, E, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
				building.cells[i][x][y].roof.W_Merge = MergePass(i, x, y, W, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
			}
		}
	}
}

void Buildings::RoofExtendPass(int i, int x, int y)
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
		bool NE_UpObstruction = !CellEmpty(i + 1, x + 1, y + 1) && (building.cells[i + 1][x + 1][y + 1].walls.W_Type == WallType::window || building.cells[i + 1][x + 1][y + 1].walls.W_Type == WallType::balcony);
		bool NW_UpObstruction = !CellEmpty(i + 1, x - 1, y + 1) && (building.cells[i + 1][x - 1][y + 1].walls.W_Type == WallType::window || building.cells[i + 1][x - 1][y + 1].walls.W_Type == WallType::balcony);
		bool SW_UpObstruction = !CellEmpty(i + 1, x - 1, y - 1) && (building.cells[i + 1][x - 1][y - 1].walls.W_Type == WallType::window || building.cells[i + 1][x - 1][y - 1].walls.W_Type == WallType::balcony);
		bool SE_UpObstruction = !CellEmpty(i + 1, x + 1, y - 1) && (building.cells[i + 1][x + 1][y - 1].walls.W_Type == WallType::window || building.cells[i + 1][x + 1][y - 1].walls.W_Type == WallType::balcony);

		if (building.cells[i][x][y].roof.type == RoofType::cone)
		{
			building.cells[i][x][y].roof.N_Extend = N_Empty;
			building.cells[i][x][y].roof.S_Extend = S_Empty;
			building.cells[i][x][y].roof.E_Extend = E_Empty;
			building.cells[i][x][y].roof.W_Extend = W_Empty;
		}
		else
		{
			building.cells[i][x][y].roof.N_Extend = (N_Empty && N_UpEmpty) || N_Cone;
			building.cells[i][x][y].roof.S_Extend = (S_Empty && S_UpEmpty) || S_Cone;
			building.cells[i][x][y].roof.E_Extend = (E_Empty && E_UpEmpty) || E_Cone;
			building.cells[i][x][y].roof.W_Extend = (W_Empty && W_UpEmpty) || W_Cone;
		}

		if (building.cells[i][x][y].roof.type == RoofType::flatUp)
		{
			building.cells[i][x][y].roof.N_Extend = building.cells[i][x][y].roof.N_Extend || (N_Empty && N_UpEmpty) ||
				(building.cells[i][x][y + 1].roof.type == RoofType::slope && building.cells[i][x][y + 1].roof.direction != S);
			building.cells[i][x][y].roof.S_Extend = building.cells[i][x][y].roof.S_Extend || (S_Empty && S_UpEmpty) ||
				(building.cells[i][x][y - 1].roof.type == RoofType::slope && building.cells[i][x][y - 1].roof.direction != N);
			building.cells[i][x][y].roof.E_Extend = building.cells[i][x][y].roof.E_Extend || (E_Empty && E_UpEmpty) ||
				(building.cells[i][x + 1][y].roof.type == RoofType::slope && building.cells[i][x + 1][y].roof.direction != W);
			building.cells[i][x][y].roof.W_Extend = building.cells[i][x][y].roof.W_Extend || (W_Empty && W_UpEmpty) ||
				(building.cells[i][x - 1][y].roof.type == RoofType::slope && building.cells[i][x - 1][y].roof.direction != E);
		}
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

bool Buildings::MergePass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone, bool S_Cone, bool E_Cone, bool W_Cone)
{
	bool merge = false;

	if (direction == N)
	{
		if (N_Cone && building.cells[i][x][y + 1].roof.direction == E) 
			merge = true;
		else if (!N_Empty && building.cells[i][x][y + 1].roof.type == RoofType::slope && building.cells[i][x][y + 1].roof.direction == N) 
			merge = true;
	}
	else if (direction == S)
	{
		if (S_Cone && building.cells[i][x][y - 1].roof.direction == E)
			merge = true;
		else if (!S_Empty && building.cells[i][x][y - 1].roof.type == RoofType::slope && building.cells[i][x][y - 1].roof.direction == S)
			merge = true;
	}
	else if (direction == E)
	{
		if (E_Cone && building.cells[i][x + 1][y].roof.direction == N)
			merge = true;
		else if (!E_Empty && building.cells[i][x + 1][y].roof.type == RoofType::slope && building.cells[i][x + 1][y].roof.direction == E)
			merge = true;
	}
	else if (direction == W)
	{
		if (W_Cone && building.cells[i][x - 1][y].roof.direction == N)
			merge = true;
		else if (!W_Empty && building.cells[i][x - 1][y].roof.type == RoofType::slope && building.cells[i][x - 1][y].roof.direction == W)
			merge = true;
	}

	return (merge);
}

bool Buildings::IsRoof(int i, int x, int y)
{
	return (CellValid(i, x, y) && !building.cells[i][x][y].Empty(false) && (!CellValid(i + 1, x, y) || 
		building.cells[i + 1][x][y].Empty(false)));
}

Shape Buildings::GeneratePart(PartType type, int rotate)
{
	Shape part;
	part.coordinate = true;
	part.normal = true;
	part.color = true;

	if (type == PartType::floor)
	{
		Shape floor = Shape(CUBE, true, true, true);
		floor.SetColors(glm::vec3(0));
		floor.Scale(floorConfig.scale * generationConfig.scale);

		part.Join(floor);
	}
	else if (type == PartType::flatWall)
	{
		Shape wall = Shape(CUBE, true, true, true);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(flatWallConfig.scale * generationConfig.scale);
		wall.Move(flatWallConfig.offset * generationConfig.scale);

		part.Join(wall);
	}
	else if (type == PartType::slopedWall)
	{
		Shape wall = Shape(PRISM, true, true, true);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(slopedWallConfig.scale * generationConfig.scale);
		wall.Rotate90(rotate);
		wall.Move(slopedWallConfig.offset * generationConfig.scale);

		part.Join(wall);
	}
	else if (type == PartType::flatRoof)
	{
		Shape roof = Shape(CUBE, true, true, true);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.SwapCoordinates();
		roof.Scale(flatRoofConfig.scale * generationConfig.scale);
		roof.Move(flatRoofConfig.offset * generationConfig.scale);

		part.Join(roof);
	}
	else if (type == PartType::slopedRoof)
	{
		Shape roof = Shape(CUBE, true, true, true);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.Scale(slopedRoofConfig.scale * generationConfig.scale);
		roof.Rotate(slopedRoofConfig.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Move(slopedRoofConfig.offset * generationConfig.scale);

		part.Join(roof);
	}
	else if (type == PartType::coneRoof)
	{
		Shape leftRoof = Shape(CUBE, true, true, true);
		leftRoof.InverseCoordinates(false, true);
		leftRoof.SwapCoordinates();
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(coneRoofConfig.scale * generationConfig.scale);
		leftRoof.Rotate(coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Move(coneRoofConfig.offset * generationConfig.scale);

		Shape rightRoof = Shape(CUBE, true, true, true);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.SwapCoordinates();
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(coneRoofConfig.scale * generationConfig.scale);
		rightRoof.Rotate(-coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Move(coneRoofConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * generationConfig.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::coneRoofExtension)
	{
		Shape leftRoof = Shape(PRISM, true, true, true);
		leftRoof.Rotate90(rotate);
		leftRoof.InverseCoordinates(false, false);
		leftRoof.SwapCoordinates();
		leftRoof.ScaleCoordinates(glm::vec2(0.5f, 1.0f));
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(coneRoofExtensionConfig.scale * generationConfig.scale);
		leftRoof.Rotate90();
		leftRoof.Rotate(-coneRoofExtensionConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Move(coneRoofExtensionConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * generationConfig.scale);

		Shape rightRoof = Shape(PRISM, true, true, true);
		rightRoof.Rotate90(rotate);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.ScaleCoordinates(glm::vec2(0.5f, 1.0f));
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(coneRoofExtensionConfig.scale * generationConfig.scale);
		rightRoof.Rotate90();
		rightRoof.Rotate(coneRoofExtensionConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Move(coneRoofExtensionConfig.offset * generationConfig.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::beam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(3, 0, 0));
		beam.Scale(beamConfig.scale * generationConfig.scale);
		beam.Rotate(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));
		beam.Move(beamConfig.offset * generationConfig.scale);

		part.Join(beam);
	}
	else if (type == PartType::slopedBeam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(3, 0, 0));
		beam.Scale(slopedBeamConfig.scale * generationConfig.scale);
		beam.Rotate(slopedBeamConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate90(rotate);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));
		beam.Move(slopedBeamConfig.offset * generationConfig.scale);

		part.Join(beam);
	}
	else if (type == PartType::slightSlopedBeam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(3, 0, 0));
		beam.Scale(slightSlopedBeamConfig.scale * generationConfig.scale);
		beam.Rotate(slightSlopedBeamConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate90(rotate);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));
		beam.Move(slightSlopedBeamConfig.offset * generationConfig.scale);

		part.Join(beam);
	}
	else if (type == PartType::collumn)
	{
		Shape collumn = Shape(CUBE, true, true, true);
		collumn.SetColors(glm::vec3(0, 0, 0));
		collumn.Scale(collumnConfig.scale * generationConfig.scale);
		collumn.ScaleCoordinates(glm::vec2(0.14f, 1.0f));
		collumn.Move(collumnConfig.offset * generationConfig.scale);

		part.Join(collumn);
	}
	
	return (part);
}

void Buildings::GenerateMesh()
{
	if (building.mesh.created)
		building.mesh.DestroyAtRuntime();

	building.mesh.coordinate = true;
	building.mesh.normal = true;
	building.mesh.color = true;
	building.mesh.shape.coordinate = true;
	building.mesh.shape.normal = true;
	building.mesh.shape.color = true;

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
	Shape floor = GeneratePart(PartType::floor);

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
	Shape wall = GeneratePart(PartType::flatWall);
	Shape beam = GeneratePart(PartType::beam);
	Shape collumn = GeneratePart(PartType::collumn);

	wall.Join(beam);
	wall.Join(collumn);

	if (direction == S) wall.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == E) wall.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == W) wall.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	wall.Move(offset);

	building.mesh.shape.Join(wall);
}

void Buildings::GenerateBeams(int i, int x, int y)
{
	Shape beams;
	beams.normal = true;
	beams.coordinate = true;
	beams.color = true;

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
			Shape beam = GeneratePart(PartType::beam);

			beams.Join(beam);
		}
		if (E_Empty_B)
		{
			Shape beam = GeneratePart(PartType::beam);

			beam.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			beams.Join(beam);
		}
		if (S_Empty)
		{
			Shape beam = GeneratePart(PartType::beam);

			beam.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			beams.Join(beam);
		}
		if (W_Empty)
		{
			Shape beam = GeneratePart(PartType::beam);

			beam.Rotate(270.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			beams.Join(beam);
		}
    }

	if (walls.N_Type == WallType::beams)
	{
		Shape collumn = GeneratePart(PartType::collumn);
		beams.Join(collumn);
	}
	if (walls.E_Type == WallType::beams)
	{
		Shape collumn = GeneratePart(PartType::collumn);
		collumn.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		beams.Join(collumn);
	}
	if (walls.S_Type == WallType::beams)
	{
		Shape collumn = GeneratePart(PartType::collumn);
		collumn.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		beams.Join(collumn);
	}
	if (walls.W_Type == WallType::beams)
	{
		Shape collumn = GeneratePart(PartType::collumn);
		collumn.Rotate(270.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		beams.Join(collumn);
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
				GenerateRoof(level, x, y, building.cells[level][x][y].roof.type,
					building.cells[level][x][y].roof.direction);
			}
		}
	}
}

void Buildings::GenerateRoof(int i, int x, int y, RoofType type, D direction)
{
	Roof roofData = building.cells[i][x][y].roof;

	Shape roof;
	roof.coordinate = true;
	roof.normal = true;
	roof.color = true;

	if (type == RoofType::flatUp)
	{
		roof = GeneratePart(PartType::flatRoof);

		Shape sideBeam = GeneratePart(PartType::beam);
		sideBeam.Rotate90(-1);
		sideBeam.Move(flatRoofConfig.offset * generationConfig.scale);
		roof.Join(sideBeam);

		if (!CellValid(i, x + 1, y) || building.cells[i][x + 1][y].roof.type != RoofType::flatUp ||
        	building.cells[i][x + 1][y].roof.type != RoofType::slope)
		{
			sideBeam = GeneratePart(PartType::beam);
			sideBeam.Rotate90();
			sideBeam.Move(flatRoofConfig.offset * generationConfig.scale);
			roof.Join(sideBeam);
		}

		for (int d = 0; d < 4; d++)
		{
			if (roofData.Extend((D)d))
			{
				Shape wall = GeneratePart(PartType::flatWall);
				wall.Scale(glm::vec3(1.0f, 0.6f, 1.0f));

				Shape beam = GeneratePart(PartType::beam);
				beam.Move(glm::vec3(0.0f, 0.55f, 0.0f) * generationConfig.scale);
				wall.Join(beam);

				Shape collumn = GeneratePart(PartType::collumn);
				collumn.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
				wall.Join(collumn);

				wall.Rotate90(d);
				roof.Join(wall);
			}
		}
	}
	else if (type == RoofType::slope)
	{
		roof = GeneratePart(PartType::slopedRoof);

		if (roofData.LocalExtend(S))
		{
			Shape beam = GeneratePart(PartType::beam);
			beam.Move(glm::vec3(0.0f, -0.1f, 0.0f) * generationConfig.scale);
			roof.Join(beam);
		}

		if (roofData.LocalExtend(W))
		{
			Shape collumn = GeneratePart(PartType::collumn);
			collumn.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
			collumn.Move(glm::vec3(0.0f, 0.0f, -collumnConfig.offset.z * 2.0f) * generationConfig.scale);
			roof.Join(collumn);

			Shape wall = GeneratePart(PartType::slopedWall);
			wall.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
			wall.Rotate90();
			roof.Join(wall);

			Shape beam = GeneratePart(PartType::slopedBeam);
			beam.Rotate90();
			beam.Move(glm::vec3(0.0f, 0.25f, 0.0f) * generationConfig.scale);
			roof.Join(beam);
		}

		if (roofData.LocalExtend(E))
		{
			Shape wall = GeneratePart(PartType::slopedWall);
			wall.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
			wall.Rotate90();
			wall.Move(glm::vec3(-slopedWallConfig.offset.z * 2.0f, 0.0f, 0.0f) * generationConfig.scale);

			Shape beam = GeneratePart(PartType::slopedBeam, 2);
			beam.Rotate90(-1);
			beam.Move(glm::vec3(0.0f, 0.25f, 0.0f) * generationConfig.scale);
			roof.Join(beam);

			roof.Join(wall);
		}
	}
	else if (type == RoofType::cone)
	{
		roof = GeneratePart(PartType::coneRoof);

		//if (roofData.MergeCount(roofData.direction) > 0)
		//{
		//	roof.Scale(glm::vec3(1.0f, 1.0f, 1.0f + roofData.MergeCount(roofData.direction) * 0.5f));
		//	if (roofData.LocalMerge(N)) roof.Move(glm::vec3(0.0f, 0.0f, 0.25f) * generationConfig.scale);
		//	if (roofData.LocalMerge(S)) roof.Move(glm::vec3(0.0f, 0.0f, -0.25f) * generationConfig.scale);
		//}

		for (int d = 0; d <= 2; d += 2)
		{
			if (roofData.Extend((D)(d + roofData.direction)))
			{
				Shape wallLeft = GeneratePart(PartType::slopedWall);
				wallLeft.Scale(glm::vec3(0.5f, 0.3f, 1.0f));
				wallLeft.Move(glm::vec3(-0.25f, 0.0f, 0.0f) * generationConfig.scale);
				wallLeft.Rotate90(d);

				Shape wallRight = GeneratePart(PartType::slopedWall, 2);
				wallRight.Scale(glm::vec3(0.5f, 0.3f, 1.0f));
				wallRight.Move(glm::vec3(0.25f, 0.0f, 0.0f) * generationConfig.scale);
				wallRight.Rotate90(d);

				Shape beamLeft = GeneratePart(PartType::slightSlopedBeam);
				beamLeft.Move(glm::vec3(-0.25f, 0.125f, 0.0f) * generationConfig.scale);
				beamLeft.Rotate90(d);

				Shape beamRight = GeneratePart(PartType::slightSlopedBeam, 2);
				beamRight.Move(glm::vec3(0.25f, 0.125f, 0.0f) * generationConfig.scale);
				beamRight.Rotate90(d);

				roof.Join(wallLeft);
				roof.Join(wallRight);
				roof.Join(beamLeft);
				roof.Join(beamRight);
			}

			if (roofData.LocalMerge(N))
			{
				Shape extension = GeneratePart(PartType::coneRoofExtension);
				extension.Move(glm::vec3(0.0f, 0.0f, 0.5f) * generationConfig.scale);

				roof.Join(extension);
			}
			if (roofData.LocalMerge(S))
			{
				Shape extension = GeneratePart(PartType::coneRoofExtension);
				extension.Move(glm::vec3(0.0f, 0.0f, 0.5f) * generationConfig.scale);
				extension.Rotate90(2);
				//extension.InverseCoordinates(true, false);

				roof.Join(extension);
			}
		}
	}

	if (direction == S) roof.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == E) roof.Rotate(90.0f * (type == RoofType::cone ? 1.0f : -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == W) roof.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	roof.Move(glm::vec3(x, i + 1, y) * generationConfig.scale);

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

std::vector<Texture> Buildings::beamTextures;
std::vector<Texture> Buildings::plasteredTextures;
std::vector<Texture> Buildings::reedTextures;

Pipeline Buildings::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Buildings::shadowPipeline{Manager::currentDevice, Manager::camera};

Descriptor Buildings::graphicsDescriptor{Manager::currentDevice};
Descriptor Buildings::shadowDescriptor{Manager::currentDevice};

GenerationConfig Buildings::generationConfig;

Building Buildings::building;

Random Buildings::random;

PartConfig Buildings::floorConfig{"floor", glm::vec3(1.0f, 0.05f, 1.0f)};
PartConfig Buildings::flatWallConfig{"flat wall", glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.0f), glm::vec3(0.0f, 0.5f, 0.5f)};
PartConfig Buildings::slopedWallConfig{"sloped wall", glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.0f), glm::vec3(0.0f, 0.5f, 0.5f)};
PartConfig Buildings::flatRoofConfig{"flat roof", glm::vec3(1.0f, 0.1f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.625f, 0.0f)};
PartConfig Buildings::slopedRoofConfig{"sloped roof", glm::vec3(1.0f, 0.1f, 1.25f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.3f, 0.0f)};
PartConfig Buildings::coneRoofConfig{"cone roof", glm::vec3(0.635f, 0.1f, 1.0f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(-0.26f, 0.135f, 0.0f)};
PartConfig Buildings::coneRoofExtensionConfig{"cone roof extension", glm::vec3(0.5f, 0.635f, 0.1f), glm::vec3(122.5f, 0.0f, 0.0f), glm::vec3(-0.26f, 0.135f, 0.250f)};
//PartConfig Buildings::beamConfig{"beam", glm::vec3(1.1f, 0.15f, 0.15f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f)};
PartConfig Buildings::beamConfig{"beam", glm::vec3(0.15f, 1.1f, 0.15f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f)};
PartConfig Buildings::slopedBeamConfig{"sloped beam", glm::vec3(0.15f, 1.1f, 0.15f), glm::vec3(-57.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f)};
PartConfig Buildings::slightSlopedBeamConfig{"slight sloped beam", glm::vec3(0.1f, 0.6f, 0.1f), glm::vec3(-57.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f)};
PartConfig Buildings::collumnConfig{"collumn", glm::vec3(0.15f, 1.1f, 0.15f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.499f, 0.5f, 0.499f)};