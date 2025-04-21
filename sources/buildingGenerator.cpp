#include "buildingGenerator.hpp"

#include "time.hpp"

BuildingGenerator::BuildingGenerator()
{

}

BuildingGenerator::BuildingGenerator(GenerationConfig config)
{
    this->config = config;
}

BuildingGenerator::BuildingGenerator(GenerationConfig config, std::vector<std::vector<std::vector<BuildingCell>>> cells)
{
    this->config = config;
    this->cells = cells;
    cellsGenerated = true;
}

BuildingGenerator::~BuildingGenerator()
{
    
}

void BuildingGenerator::SetConfig(GenerationConfig config)
{
    this->config = config;
}

void BuildingGenerator::SetCells(std::vector<std::vector<std::vector<BuildingCell>>> cells)
{
    this->cells = cells;
}

void BuildingGenerator::GenerateCells()
{
	cells.clear();
	//building->size = generationConfig.maxSize;
	config.currentSize = glm::ivec3(0, 0, 0);
	if (config.random) config.seed = int(uint32_t(Time::GetCurrentTime() * 10000) % RAND_MAX);
	random.SetSeed(config.seed);

	cells.resize(config.maxSize.y);
    for (int i = 0; i < config.maxSize.y; i++)
    {
        cells[i].resize(config.maxSize.x);
        for (int x = 0; x < config.maxSize.x; x++)
        {
            cells[i][x].resize(config.maxSize.z);
        }
    }

	for (int i = 0; i < config.maxSize.y; i++)
	{
		if (i < config.minSize.y || random.Next(0, 9) < config.levelFactor)
		{
			ExpandLevel(i);
			FillLevel(i);
			config.currentSize = glm::ivec3(0, config.currentSize.y + 1, 0);
		}
		else break;
	}

	SetWalls();
	SetBeams();
	SetRoof();

    cellsGenerated = true;
}

std::vector<std::vector<std::vector<BuildingCell>>> BuildingGenerator::GetCells()
{
    if (!cellsGenerated) GenerateCells();

    return (cells);
}

void BuildingGenerator::ExpandLevel(int level)
{
    glm::ivec2 start;

    start.x = int(floor(float(config.maxSize.x) * 0.5f));
    start.y = int(floor(float(config.maxSize.z) * 0.5f));

	ExpandCell(level, start.x, start.y, glm::clamp(config.expansionFactor - level, 1, config.expansionFactor));
}

void BuildingGenerator::ExpandCell(int i, int x, int y, int factor)
{
    cells[i][x][y].floor.type = FloorType::flat;

	int currentFactor = factor - ((CellEmpty(i - 1, x + 1, y) && i != 0) ? config.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x + 1, y, currentFactor, 1)) ExpandCell(i, x + 1, y, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x - 1, y) && i != 0) ? config.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x - 1, y, currentFactor, 1)) ExpandCell(i, x - 1, y, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x, y + 1) && i != 0) ? config.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x, y + 1, currentFactor, -1)) ExpandCell(i, x, y + 1, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x, y - 1) && i != 0) ? config.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x, y - 1, currentFactor, -1)) ExpandCell(i, x, y - 1, currentFactor);
}

bool BuildingGenerator::ExpansionValid(int i, int x, int y, int factor, int increase)
{
    if ((increase == 1 && config.currentSize.x < config.minSize.x - i) || (increase == -1 && config.currentSize.z < config.minSize.z - i)) factor = 10;
    
    bool valid = CellValid(i, x, y) && cells[i][x][y].Empty() && random.Next(0, 9) < factor;

    if (valid && increase == 1) config.currentSize.x++;
    else if (valid && increase == -1) config.currentSize.z++;

    return (valid);
}

void BuildingGenerator::FillLevel(int level)
{
	for (int x = 0; x < cells[level].size(); x++)
	{
		for (int y = 0; y < cells[level][x].size(); y++)
		{
			if (CellEmpty(level, x, y))
			{
				bool N_Empty = CellEmpty(level, x, y + 1, false);
				bool S_Empty = CellEmpty(level, x, y - 1, false);
				bool E_Empty = CellEmpty(level, x + 1, y, false);
				bool W_Empty = CellEmpty(level, x - 1, y, false);

				if (!N_Empty && !S_Empty && !E_Empty && !W_Empty)
				{
					cells[level][x][y].floor.type = FloorType::flat;
				}
			}
		}
	}
}

/*NeighbourData BuildingGenerator::GetNeighbourData(int i, int x, int y)
{
    NeighbourData neighbourData;
    neighbourData.N_Empty = CellEmpty(i, x, y + 1, false);
	neighbourData.S_Empty = CellEmpty(i, x, y - 1, false);
	neighbourData.E_Empty = CellEmpty(i, x + 1, y, false);
	neighbourData.W_Empty = CellEmpty(i, x - 1, y, false);

    return (neighbourData);
}*/

bool BuildingGenerator::CellEmpty(int i, int x, int y, bool countBeams)
{
	return (!CellValid(i, x, y) || cells[i][x][y].Empty(countBeams));
}

bool BuildingGenerator::FloorEmpty(int i, int x, int y)
{
	return (!CellValid(i, x, y) || cells[i][x][y].floor.Empty());
}

bool BuildingGenerator::CellValid(int i, int x, int y)
{
    return (i >= 0 && i < config.maxSize.y && x >= 0 && x < config.maxSize.x && y >= 0 && y < config.maxSize.z);
}

bool BuildingGenerator::IsRoof(int i, int x, int y)
{
	return (CellValid(i, x, y) && !cells[i][x][y].Empty(false) && (!CellValid(i + 1, x, y) || cells[i + 1][x][y].Empty(false)));
}

void BuildingGenerator::SetWalls()
{
	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				if (!cells[i][x][y].floor.Empty())
				{
					bool N_Empty = FloorEmpty(i, x, y + 1);
					bool S_Empty = FloorEmpty(i, x, y - 1);
					bool E_Empty = FloorEmpty(i, x + 1, y);
					bool W_Empty = FloorEmpty(i, x - 1, y);
					bool N_DownEmpty = FloorEmpty(i - 1, x, y + 1);
					bool S_DownEmpty = FloorEmpty(i - 1, x, y - 1);
					bool E_DownEmpty = FloorEmpty(i - 1, x + 1, y);
					bool W_DownEmpty = FloorEmpty(i - 1, x - 1, y);
					bool NE_DownEmpty = FloorEmpty(i - 1, x + 1, y + 1) || !IsRoof(i - 1, x + 1, y + 1);
                    bool NW_DownEmpty = FloorEmpty(i - 1, x - 1, y + 1) || !IsRoof(i - 1, x - 1, y + 1);
                    bool SE_DownEmpty = FloorEmpty(i - 1, x + 1, y - 1) || !IsRoof(i - 1, x + 1, y - 1);
                    bool SW_DownEmpty = FloorEmpty(i - 1, x - 1, y - 1) || !IsRoof(i - 1, x - 1, y - 1);
					bool S_Balcony = i != 0 && SE_DownEmpty && SW_DownEmpty;
                    bool N_Balcony = i != 0 && NE_DownEmpty && NW_DownEmpty;
                    bool E_Balcony = i != 0 && NE_DownEmpty && SE_DownEmpty;
                    bool W_Balcony = i != 0 && NW_DownEmpty && SW_DownEmpty;

					if (N_Empty)
					{
						cells[i][x][y].walls.N_Type = WallType::flat;
						cells[i][x][y].walls.N_Variant = random.Next(0, 21 - config.decoratedFactor);
						if (N_DownEmpty && (x + i + config.seed) % 2 == 0)
						{
							if (N_Balcony && random.Next(0, 9) < config.balconyFactor)
							{
								cells[i][x][y].walls.N_Type = WallType::balcony;
							}
							else
							{
								cells[i][x][y].walls.N_Type = WallType::window;
							}
						}
					}

					if (S_Empty)
					{
						cells[i][x][y].walls.S_Type = WallType::flat;
						cells[i][x][y].walls.S_Variant = random.Next(0, 21 - config.decoratedFactor);
						if (S_DownEmpty && (x + 1 + i + config.seed) % 2 == 0)
						{
							if (S_Balcony && random.Next(0, 9) < config.balconyFactor)
							{
								cells[i][x][y].walls.S_Type = WallType::balcony;
							}
							else
							{
								cells[i][x][y].walls.S_Type = WallType::window;
							}
						}
					}

					if (E_Empty)
					{
						cells[i][x][y].walls.E_Type = WallType::flat;
						cells[i][x][y].walls.E_Variant = random.Next(0, 21 - config.decoratedFactor);
						if (E_DownEmpty && (y + i + config.seed) % 2 == 0)
						{
							if (E_Balcony && random.Next(0, 9) < config.balconyFactor)
							{
								cells[i][x][y].walls.E_Type = WallType::balcony;
							}
							else
							{
								cells[i][x][y].walls.E_Type = WallType::window;
							}
						}
					}

					if (W_Empty)
					{
						cells[i][x][y].walls.W_Type = WallType::flat;
						cells[i][x][y].walls.W_Variant = random.Next(0, 21 - config.decoratedFactor);
						if (W_DownEmpty && (y + 1 + i + config.seed) % 2 == 0)
						{
							if (W_Balcony && random.Next(0, 9) < config.balconyFactor)
							{
								cells[i][x][y].walls.W_Type = WallType::balcony;
							}
							else
							{
								cells[i][x][y].walls.W_Type = WallType::window;
							}
						}
					}
				}
			}
		}
	}
}

void BuildingGenerator::SetBeams()
{
	for (int i = cells.size() - 1; i >= 0; i--)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				if (cells[i][x][y].walls.Empty() && !CellEmpty(i + 1, x, y))
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

					cells[i][x][y].walls.N_Type = N_Beam ? WallType::beams : WallType::empty;
                    cells[i][x][y].walls.S_Type = S_Beam ? WallType::beams : WallType::empty;
                    cells[i][x][y].walls.E_Type = E_Beam ? WallType::beams : WallType::empty;
                    cells[i][x][y].walls.W_Type = W_Beam ? WallType::beams : WallType::empty;
				}
			}
		}
	}
}

bool BuildingGenerator::BeamPass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool NE_Empty, bool NW_Empty, bool SE_Empty, bool SW_Empty)
{
	if (direction == N)
	{
		bool N = N_Empty || cells[i][x][y + 1].walls.E_Type == WallType::empty;
        bool E = E_Empty || cells[i][x + 1][y].walls.W_Type == WallType::empty;
        bool NE = NE_Empty || cells[i][x + 1][y + 1].walls.S_Type == WallType::empty;
        return (N && E && NE);
	}
	else if (direction == S)
	{
		bool S = S_Empty || cells[i][x][y - 1].walls.W_Type == WallType::empty;
        bool W = W_Empty || cells[i][x - 1][y].walls.E_Type == WallType::empty;
        bool SW = SW_Empty || cells[i][x - 1][y - 1].walls.N_Type == WallType::empty;
        return (S && W && SW);
	}
	else if (direction == E)
	{
		bool S = S_Empty || cells[i][x][y - 1].walls.N_Type == WallType::empty;
        bool E = E_Empty || cells[i][x + 1][y].walls.S_Type == WallType::empty;
        bool SE = SE_Empty || cells[i][x + 1][y - 1].walls.W_Type == WallType::empty;
        return (S && E && SE);
	}
	else if (direction == W)
	{
		bool N = N_Empty || cells[i][x][y + 1].walls.S_Type == WallType::empty;
        bool W = W_Empty || cells[i][x - 1][y].walls.N_Type == WallType::empty;
        bool NW = NW_Empty || cells[i][x - 1][y + 1].walls.E_Type == WallType::empty;
        return (N && W && NW);
	}
	return (false);
}

void BuildingGenerator::SetRoof()
{
	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofTypePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofTypePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofTypePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofDirectionPass(i, x, y);
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofMergePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int x = 0; x < cells[i].size(); x++)
		{
			for (int y = 0; y < cells[i][x].size(); y++)
			{
				RoofExtendPass(i, x, y);
			}
		}
	}
}

void BuildingGenerator::RoofTypePass(int i, int x, int y)
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
		bool N_Cone = !N_Empty && cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && cells[i][x - 1][y].roof.type == RoofType::cone;
		int coneAmount = (N_Cone ? 1 : 0) + (S_Cone ? 1 : 0) + (E_Cone ? 1 : 0) + (W_Cone ? 1 : 0);
		int EW_Empty = ((E_Empty || E_Cone) ? 1 : 0) + ((W_Empty || W_Cone) ? 1 : 0);

		if (cells[i][x][y].roof.type == RoofType::cone || emptyAmount >= 3)
		{
			cells[i][x][y].roof.type = RoofType::cone;
		}
		else if (emptyAmount + coneAmount >= 1 && EW_Empty != 0)
		{
			cells[i][x][y].roof.type = RoofType::slope;
		}
		else 
		{
			cells[i][x][y].roof.type = RoofType::flatUp;
		}
	}
}

void BuildingGenerator::RoofConePass(int i, int x, int y)
{
	if (!cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1);
		bool S_Empty = CellEmpty(i, x, y - 1);
		bool E_Empty = CellEmpty(i, x + 1, y);
		bool W_Empty = CellEmpty(i, x - 1, y);
		int emptyAmount = (N_Empty ? 1 : 0) + (S_Empty ? 1 : 0) + (E_Empty ? 1 : 0) + (W_Empty ? 1 : 0);
		bool N_Cone = !N_Empty && cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && cells[i][x - 1][y].roof.type == RoofType::cone;
		int coneAmount = (N_Cone ? 1 : 0) + (S_Cone ? 1 : 0) + (E_Cone ? 1 : 0) + (W_Cone ? 1 : 0);
		bool N_Wall = !N_Empty && !CellEmpty(i + 1, x, y + 1);
		bool S_Wall = !N_Empty && !CellEmpty(i + 1, x, y - 1);
		bool E_Wall = !N_Empty && !CellEmpty(i + 1, x + 1, y);
		bool W_Wall = !N_Empty && !CellEmpty(i + 1, x - 1, y);
		int wallAmount = (N_Wall ? 1 : 0) + (S_Wall ? 1 : 0) + (E_Wall ? 1 : 0) + (W_Wall ? 1 : 0);

		if (cells[i][x][y].roof.type != RoofType::cone && ((emptyAmount == 2 && coneAmount == 2) ||
            (emptyAmount == 1 && coneAmount == 3) || (emptyAmount == 2 && coneAmount == 1) || 
            (emptyAmount == 1 && ((N_Cone && S_Cone) || (E_Cone && W_Cone))) ||
            (emptyAmount == 1 && coneAmount == 2 && wallAmount == 1))) 
            cells[i][x][y].roof.type = RoofType::cone;
	}
}

void BuildingGenerator::RoofDirectionPass(int i, int x, int y)
{
	if (!cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1, false);
		bool S_Empty = CellEmpty(i, x, y - 1, false);
		bool E_Empty = CellEmpty(i, x + 1, y, false);
		bool W_Empty = CellEmpty(i, x - 1, y, false);
		bool N_UpEmpty = CellEmpty(i + 1, x, y + 1, false);
		bool S_UpEmpty = CellEmpty(i + 1, x, y - 1, false);
		bool E_UpEmpty = CellEmpty(i + 1, x + 1, y, false);
		bool W_UpEmpty = CellEmpty(i + 1, x - 1, y, false);
		bool N_Cone = !N_Empty && cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && cells[i][x - 1][y].roof.type == RoofType::cone;

		cells[i][x][y].roof.direction = GetRoofDirection(cells[i][x][y].roof.type, N_Empty, S_Empty, E_Empty,
			W_Empty, N_Cone, S_Cone, E_Cone, W_Cone, E_UpEmpty, W_UpEmpty);
	}
}

void BuildingGenerator::RoofMergePass(int i, int x, int y)
{
	if (!cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1);
		bool S_Empty = CellEmpty(i, x, y - 1);
		bool E_Empty = CellEmpty(i, x + 1, y);
		bool W_Empty = CellEmpty(i, x - 1, y);
		bool N_Cone = !N_Empty && cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && cells[i][x - 1][y].roof.type == RoofType::cone;

		if (cells[i][x][y].roof.type == RoofType::cone)
		{
			if (cells[i][x][y].roof.direction == N)
			{
				cells[i][x][y].roof.N_Merge = MergePass(i, x, y, N, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
				cells[i][x][y].roof.S_Merge = MergePass(i, x, y, S, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
			}
			else if (cells[i][x][y].roof.direction == E)
			{
				cells[i][x][y].roof.E_Merge = MergePass(i, x, y, E, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
				cells[i][x][y].roof.W_Merge = MergePass(i, x, y, W, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
			}
		}
	}
}

void BuildingGenerator::RoofExtendPass(int i, int x, int y)
{
	if (!cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1, false);
		bool S_Empty = CellEmpty(i, x, y - 1, false);
		bool E_Empty = CellEmpty(i, x + 1, y, false);
		bool W_Empty = CellEmpty(i, x - 1, y, false);
		bool N_UpEmpty = CellEmpty(i + 1, x, y + 1, false);
		bool S_UpEmpty = CellEmpty(i + 1, x, y - 1, false);
		bool E_UpEmpty = CellEmpty(i + 1, x + 1, y, false);
		bool W_UpEmpty = CellEmpty(i + 1, x - 1, y, false);
		bool N_Cone = !N_Empty && cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && cells[i][x - 1][y].roof.type == RoofType::cone;
		bool NE_UpObstruction = !CellEmpty(i + 1, x + 1, y + 1) && (cells[i + 1][x + 1][y + 1].walls.W_Type == WallType::window || cells[i + 1][x + 1][y + 1].walls.W_Type == WallType::balcony);
		bool NW_UpObstruction = !CellEmpty(i + 1, x - 1, y + 1) && (cells[i + 1][x - 1][y + 1].walls.W_Type == WallType::window || cells[i + 1][x - 1][y + 1].walls.W_Type == WallType::balcony);
		bool SW_UpObstruction = !CellEmpty(i + 1, x - 1, y - 1) && (cells[i + 1][x - 1][y - 1].walls.W_Type == WallType::window || cells[i + 1][x - 1][y - 1].walls.W_Type == WallType::balcony);
		bool SE_UpObstruction = !CellEmpty(i + 1, x + 1, y - 1) && (cells[i + 1][x + 1][y - 1].walls.W_Type == WallType::window || cells[i + 1][x + 1][y - 1].walls.W_Type == WallType::balcony);

		if (cells[i][x][y].roof.type == RoofType::cone)
		{
			cells[i][x][y].roof.N_Extend = N_Empty;
			cells[i][x][y].roof.S_Extend = S_Empty;
			cells[i][x][y].roof.E_Extend = E_Empty;
			cells[i][x][y].roof.W_Extend = W_Empty;
		}
		else
		{
			cells[i][x][y].roof.N_Extend = (N_Empty && N_UpEmpty) || N_Cone;
			cells[i][x][y].roof.S_Extend = (S_Empty && S_UpEmpty) || S_Cone;
			cells[i][x][y].roof.E_Extend = (E_Empty && E_UpEmpty) || E_Cone;
			cells[i][x][y].roof.W_Extend = (W_Empty && W_UpEmpty) || W_Cone;
		}

		if (cells[i][x][y].roof.type == RoofType::flatUp)
		{
			cells[i][x][y].roof.N_Extend = cells[i][x][y].roof.N_Extend || (N_Empty && N_UpEmpty) ||
				(cells[i][x][y + 1].roof.type == RoofType::slope && cells[i][x][y + 1].roof.direction != S);
			cells[i][x][y].roof.S_Extend = cells[i][x][y].roof.S_Extend || (S_Empty && S_UpEmpty) ||
				(cells[i][x][y - 1].roof.type == RoofType::slope && cells[i][x][y - 1].roof.direction != N);
			cells[i][x][y].roof.E_Extend = cells[i][x][y].roof.E_Extend || (E_Empty && E_UpEmpty) ||
				(cells[i][x + 1][y].roof.type == RoofType::slope && cells[i][x + 1][y].roof.direction != W);
			cells[i][x][y].roof.W_Extend = cells[i][x][y].roof.W_Extend || (W_Empty && W_UpEmpty) ||
				(cells[i][x - 1][y].roof.type == RoofType::slope && cells[i][x - 1][y].roof.direction != E);
		}
	}
}

bool BuildingGenerator::MergePass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone, bool S_Cone, bool E_Cone, bool W_Cone)
{
	bool merge = false;

	if (direction == N)
	{
		if (N_Cone && cells[i][x][y + 1].roof.direction == E) 
			merge = true;
		else if (!N_Empty && cells[i][x][y + 1].roof.type == RoofType::slope && cells[i][x][y + 1].roof.direction == N) 
			merge = true;
	}
	else if (direction == S)
	{
		if (S_Cone && cells[i][x][y - 1].roof.direction == E)
			merge = true;
		else if (!S_Empty && cells[i][x][y - 1].roof.type == RoofType::slope && cells[i][x][y - 1].roof.direction == S)
			merge = true;
	}
	else if (direction == E)
	{
		if (E_Cone && cells[i][x + 1][y].roof.direction == N)
			merge = true;
		else if (!E_Empty && cells[i][x + 1][y].roof.type == RoofType::slope && cells[i][x + 1][y].roof.direction == E)
			merge = true;
	}
	else if (direction == W)
	{
		if (W_Cone && cells[i][x - 1][y].roof.direction == N)
			merge = true;
		else if (!W_Empty && cells[i][x - 1][y].roof.type == RoofType::slope && cells[i][x - 1][y].roof.direction == W)
			merge = true;
	}

	return (merge);
}

D BuildingGenerator::GetRoofDirection(RoofType type, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone, bool S_Cone, bool E_Cone, bool W_Cone, bool E_UpEmpty, bool W_UpEmpty)
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
		else
		{
			//NS = (N_Empty ? 0 : (N_Cone ? 1 : 3)) + (S_Empty ? 0 : (S_Cone ? 1 : 3));
			//EW = (E_Empty ? 0 : (E_Cone ? 1 : 3)) + (W_Empty ? 0 : (W_Cone ? 1 : 3));
			return NS > EW ? N : E;
		}
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

void BuildingGenerator::GenerateShape()
{
    shape.coordinate = true;
	shape.normal = true;
	shape.color = true;

	for (int i = 0; i < cells.size(); i++)
	{
		GenerateFloors(i);
		GenerateWalls(i);
		GenerateRoofs(i);
	}

	float xOffset = float(config.maxSize.x - 1) * 0.5;
	float zOffset = float(config.maxSize.z - 1) * 0.5;
	shape.Move(glm::vec3(-config.scale * xOffset, 0.375f * config.scale, -config.scale * zOffset));
}

Shape BuildingGenerator::GetShape()
{
    if (!cellsGenerated) GenerateCells();

    if (!shapeGenerated) GenerateShape();

    return (shape);
}

Shape BuildingGenerator::GeneratePart(PartType type, glm::vec3 scale, glm::vec3 offset, glm::vec3 rotation)
{
	Shape part;
	part.coordinate = true;
	part.normal = true;
	part.color = true;

	if (type == PartType::floor)
	{
		//Shape floor = Shape(CUBE, true, true, true);
		Shape floor = Shape(NO_SHAPE, true, true, true);
		floor.SetCube(false, false, false, false, true, true);
		floor.SetColors(glm::vec3(0));
		floor.Scale(Buildings::floorConfig.scale * config.scale);
		floor.Scale(scale);
		floor.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		floor.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		floor.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		floor.Move(offset * config.scale);

		part.Join(floor);
	}
	else if (type == PartType::foundation)
	{
		//Shape floor = Shape(CUBE, true, true, true);
		Shape floor = Shape(NO_SHAPE, true, true, true);
		floor.SetCube(true, true, true, true, false, false);
		floor.SetColors(glm::vec3(3, 0, 0));
		floor.Scale(Buildings::foundationConfig.scale * config.scale);
		floor.Scale(scale);
		floor.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		floor.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		floor.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		floor.Move(Buildings::foundationConfig.offset * config.scale);
		floor.Move(offset * config.scale);

		part.Join(floor);
	}
	else if (type == PartType::flatWall)
	{
		//Shape wall = Shape(CUBE, true, true, true);
		Shape wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, !config.lod, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::flatWallConfig.scale * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move(Buildings::flatWallConfig.offset * config.scale);
		wall.Move(offset * config.scale);

		part.Join(wall);
	}
	else if (type == PartType::windowedWall)
	{
		Shape wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::windowedWallConfig.scale * glm::vec3(1.0f, 0.25f, 1.0f) * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((Buildings::windowedWallConfig.offset + glm::vec3(0.0f, -0.375f, 0.0f)) * config.scale);
		wall.Move(offset * config.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::windowedWallConfig.scale * glm::vec3(1.0f, 0.25f, 1.0f) * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((Buildings::windowedWallConfig.offset + glm::vec3(0.0f, 0.375f, 0.0f)) * config.scale);
		wall.Move(offset * config.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::windowedWallConfig.scale * glm::vec3(0.25f, 0.5f, 1.0f) * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((Buildings::windowedWallConfig.offset + glm::vec3(0.375f, 0.0f, 0.0f)) * config.scale);
		wall.Move(offset * config.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::windowedWallConfig.scale * glm::vec3(0.25f, 0.5f, 1.0f) * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((Buildings::windowedWallConfig.offset + glm::vec3(-0.375f, 0.0f, 0.0f)) * config.scale);
		wall.Move(offset * config.scale);
		part.Join(wall);
	}
	else if (type == PartType::dooredWall)
	{
		Shape wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::dooredWallConfig.scale * glm::vec3(0.5f, 0.25f, 1.0f) * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((Buildings::dooredWallConfig.offset + glm::vec3(0.0f, 0.375f, 0.0f)) * config.scale);
		wall.Move(offset * config.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::dooredWallConfig.scale * glm::vec3(0.25f, 1.0f, 1.0f) * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((Buildings::dooredWallConfig.offset + glm::vec3(0.375f, 0.0f, 0.0f)) * config.scale);
		wall.Move(offset * config.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::dooredWallConfig.scale * glm::vec3(0.25f, 1.0f, 1.0f) * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((Buildings::dooredWallConfig.offset + glm::vec3(-0.375f, 0.0f, 0.0f)) * config.scale);
		wall.Move(offset * config.scale);
		part.Join(wall);
	}
	else if (type == PartType::slopedWall)
	{
		Shape wall = Shape(PRISM, true, true, true);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::slopedWallConfig.scale * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move(Buildings::slopedWallConfig.offset * config.scale);
		wall.Move(offset * config.scale);

		part.Join(wall);
	}
	else if (type == PartType::slopedLowWall)
	{
		Shape wall = Shape(PRISM, true, true, true);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(Buildings::slopedLowWallConfig.scale * config.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move(Buildings::slopedLowWallConfig.offset * config.scale);
		wall.Move(offset * config.scale);

		part.Join(wall);
	}
	else if (type == PartType::flatRoof)
	{
		//Shape roof = Shape(CUBE, true, true, true);
		Shape roof = Shape(NO_SHAPE, true, true, true);
		roof.SetCube(true, true, true, true, true, !config.lod);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.SwapCoordinates();
		roof.Scale(Buildings::flatRoofConfig.scale * config.scale);
		roof.Scale(scale);
		roof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		roof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		roof.Move(Buildings::flatRoofConfig.offset * config.scale);
		roof.Move(offset * config.scale);

		part.Join(roof);
	}
	else if (type == PartType::slopedRoof)
	{
		Shape roof = Shape(NO_SHAPE, true, true, true);
		roof.SetCube(true, false, true, true, !config.lod, true);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.Scale(Buildings::slopedRoofConfig.scale * config.scale);
		roof.Scale(scale);
		roof.Rotate(Buildings::slopedRoofConfig.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		roof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		roof.Move(Buildings::slopedRoofConfig.offset * config.scale);
		roof.Move(offset * config.scale);

		part.Join(roof);
	}
	else if (type == PartType::slopedLowRoof)
	{
		Shape roof = Shape(CUBE, true, true, true);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.Scale(Buildings::slopedLowRoofConfig.scale * config.scale);
		roof.Scale(scale);
		roof.Rotate(Buildings::slopedLowRoofConfig.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		roof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		roof.Move(Buildings::slopedLowRoofConfig.offset * config.scale);
		roof.Move(offset * config.scale);

		part.Join(roof);
	}
	else if (type == PartType::coneRoof)
	{
		Shape leftRoof = Shape(NO_SHAPE, true, true, true);
		leftRoof.SetCube(true, true, false, true, !config.lod, true);
		leftRoof.InverseCoordinates(false, true);
		leftRoof.SwapCoordinates();
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(Buildings::coneRoofConfig.scale * config.scale);
		leftRoof.Scale(scale);
		leftRoof.Rotate(Buildings::coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		leftRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		leftRoof.Move(Buildings::coneRoofConfig.offset * config.scale);
		leftRoof.Move(offset * config.scale);

		Shape rightRoof = Shape(NO_SHAPE, true, true, true);
		rightRoof.SetCube(true, true, true, false, !config.lod, true);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.SwapCoordinates();
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(Buildings::coneRoofConfig.scale * config.scale);
		rightRoof.Scale(scale);
		rightRoof.Rotate(-Buildings::coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rightRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rightRoof.Move(Buildings::coneRoofConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * config.scale);
		rightRoof.Move(offset * config.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::coneLowRoof)
	{
		Shape leftRoof = Shape(CUBE, true, true, true);
		leftRoof.InverseCoordinates(false, true);
		leftRoof.SwapCoordinates();
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(Buildings::coneLowRoofConfig.scale * config.scale);
		leftRoof.Scale(scale);
		leftRoof.Rotate(Buildings::coneLowRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		leftRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		leftRoof.Move(Buildings::coneLowRoofConfig.offset * config.scale);
		leftRoof.Move(offset * config.scale);

		Shape rightRoof = Shape(CUBE, true, true, true);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.SwapCoordinates();
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(Buildings::coneLowRoofConfig.scale * config.scale);
		rightRoof.Scale(scale);
		rightRoof.Rotate(-Buildings::coneLowRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rightRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rightRoof.Move(Buildings::coneLowRoofConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * config.scale);
		rightRoof.Move(offset * config.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::coneRoofExtension)
	{
		Shape leftRoof = Shape(PRISM, true, true, true);
		leftRoof.InverseCoordinates(false, false);
		leftRoof.SwapCoordinates();
		leftRoof.ScaleCoordinates(glm::vec2(0.5f, 1.0f));
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(Buildings::coneRoofExtensionConfig.scale * config.scale);
		leftRoof.Scale(scale);
		leftRoof.Rotate90();
		leftRoof.Rotate(-Buildings::coneRoofExtensionConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		leftRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		leftRoof.Move(Buildings::coneRoofExtensionConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * config.scale);
		leftRoof.Move(offset * config.scale);

		Shape rightRoof = Shape(PRISM, true, true, true);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.ScaleCoordinates(glm::vec2(0.5f, 1.0f));
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(Buildings::coneRoofExtensionConfig.scale * config.scale);
		rightRoof.Scale(scale);
		rightRoof.Rotate90();
		rightRoof.Rotate(Buildings::coneRoofExtensionConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rightRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rightRoof.Move(Buildings::coneRoofExtensionConfig.offset * config.scale);
		rightRoof.Move(offset * config.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::beam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(4, 0, 0));
		beam.Scale(Buildings::beamConfig.scale * config.scale);
		beam.Scale(scale);
		beam.Rotate(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		beam.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		beam.Move(Buildings::beamConfig.offset * config.scale);
		beam.Move(offset * config.scale);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));

		part.Join(beam);
	}
	else if (type == PartType::slopedBeam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(4, 0, 0));
		beam.Scale(Buildings::slopedBeamConfig.scale * config.scale);
		beam.Scale(scale);
		beam.Rotate(Buildings::slopedBeamConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		beam.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		beam.Move(Buildings::slopedBeamConfig.offset * config.scale);
		beam.Move(offset * config.scale);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));

		part.Join(beam);
	}
	else if (type == PartType::slightSlopedBeam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(4, 0, 0));
		beam.Scale(Buildings::slightSlopedBeamConfig.scale * config.scale);
		beam.Scale(scale);
		beam.Rotate(Buildings::slightSlopedBeamConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		beam.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		beam.Move(Buildings::slightSlopedBeamConfig.offset * config.scale);
		beam.Move(offset * config.scale);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));

		part.Join(beam);
	}
	else if (type == PartType::collumn)
	{
		//Shape collumn = Shape(CUBE, true, true, true);
		Shape collumn = Shape(NO_SHAPE, true, true, true);
		collumn.SetCube(true, true, true, true, false, false);
		collumn.SetColors(glm::vec3(0, 0, 0));
		collumn.Scale(Buildings::collumnConfig.scale * config.scale);
		collumn.Scale(scale);
		collumn.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		collumn.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		collumn.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		collumn.Move(Buildings::collumnConfig.offset * config.scale);
		collumn.Move(offset * config.scale);
		collumn.ScaleCoordinates(glm::vec2(0.14f, 1.0f));

		part.Join(collumn);
	}
	
	return (part);
}

void BuildingGenerator::GenerateFloors(int level)
{
	for (int x = 0; x < cells[level].size(); x++)
	{
		for (int y = 0; y < cells[level][x].size(); y++)
		{
			if (!cells[level][x][y].Empty())
			{
				GenerateFloor(level, x , y, cells [level][x][y].floor.type);
			}
		}
	}
}

void BuildingGenerator::GenerateFloor(int i, int x, int y, FloorType type)
{
	Shape floor;
	floor.normal = true;
	floor.coordinate = true;
	floor.color = true;

	if (type == FloorType::flat && !config.lod)
	{
		Shape flatFloor = GeneratePart(PartType::floor);
		floor.Join(flatFloor);
	}

	if (i == 0 && !cells[i][x][y].walls.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1, false);
		bool S_Empty = CellEmpty(i, x, y - 1, false);
		bool E_Empty = CellEmpty(i, x + 1, y, false);
		bool W_Empty = CellEmpty(i, x - 1, y, false);

		if ((N_Empty + S_Empty + E_Empty + W_Empty) > 0 && !cells[i][x][y].walls.Empty(false))
		{
			Shape foundation = GeneratePart(PartType::foundation);
			floor.Join(foundation);
		}

		if (!config.lod)
		{
			bool empties[] = {N_Empty, E_Empty, S_Empty, W_Empty};

			for (int d = 0; d < 4; d++)
			{
				if (empties[d] && empties[(d + 1) % 4])
				{
					Shape collumn = GeneratePart(PartType::collumn);
					collumn.Rotate90(d);
					collumn.Move(glm::vec3(0.0f, -1.0f, 0.0f) * config.scale);
					floor.Join(collumn);
					//break;
				}
			}
		}
	}

	floor.Move(glm::vec3(x, i, y) * config.scale);

	shape.Join(floor);
}

void BuildingGenerator::GenerateWalls(int level)
{
	for (int x = 0; x < cells[level].size(); x++)
	{
		for (int y = 0; y < cells[level][x].size(); y++)
		{
			Walls walls = cells[level][x][y].walls;

			if (walls.N_Type != WallType::empty && walls.N_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * config.scale, walls.N_Type, N, walls.N_Variant);
			}
			if (walls.S_Type != WallType::empty && walls.S_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * config.scale, walls.S_Type, S, walls.S_Variant);
			}
			if (walls.E_Type != WallType::empty && walls.E_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * config.scale, walls.E_Type, E, walls.E_Variant);
			}
			if (walls.W_Type != WallType::empty && walls.W_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * config.scale, walls.W_Type, W, walls.W_Variant);
			}

			if (walls.Beams() && !config.lod)
			{
				GenerateBeams(level, x, y);
			}
		}
	}
}

void BuildingGenerator::GenerateWall(glm::vec3 offset, WallType type, D direction, int variant)
{
	PartType partType = PartType::flatWall;
	if (type == WallType::window && !config.lod) partType = PartType::windowedWall;
	else if (type == WallType::balcony && !config.lod) partType = PartType::dooredWall;
	Shape wall = GeneratePart(partType);

	if (!config.lod)
	{
		Shape beamLow = GeneratePart(PartType::beam);
		wall.Join(beamLow);

		Shape beamHigh = GeneratePart(PartType::beam);
		beamHigh.Move(glm::vec3(0.0f, 1.0f, 0.0f) * config.scale);
		wall.Join(beamHigh);

		Shape collumn = GeneratePart(PartType::collumn);
		wall.Join(collumn);
	}

	if (type == WallType::flat && variant > 0 && !config.lod)
	{
		if (variant <= 6)
		{
			float angle = 22.5f;
			float height = 1.0f - 0.225f;
			if (variant % 2 == 0)
			{
				height = 0.225f;
				angle = -22.5f;
			}
			Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.95f, 0.75f), glm::vec3(0.0f, height, 0.0f), glm::vec3(0.0f, 0.0f, angle));
			wall.Join(slopeBeam);
		}
		if (variant <= 4 || variant == 7 || variant == 8)
		{
			float angle = -22.5f;
			float height = 0.225f;
			if (variant % 2 == 0 && variant != 8)
			{
				height = 1.0f - 0.225f;
				angle = 22.5f;
			}
			Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.95f, 0.75f), glm::vec3(0.0f, height, 0.0f), glm::vec3(0.0f, 0.0f, angle));
			wall.Join(slopeBeam);
		}
		if (variant >= 3 && variant <= 9)
		{
			Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.95f, 0.75f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			wall.Join(slopeBeam);
		}
		if (variant == 10 || variant == 11)
		{
			Shape slopeBeam = GeneratePart(PartType::collumn, glm::vec3(0.75f, 0.95f, 0.75f), glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			wall.Join(slopeBeam);
		}
		if (variant >= 10 && variant % 2 == 0)
		{
			Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.375f, 0.75f), glm::vec3(-0.375f, 0.85f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f));
			wall.Join(slopeBeam);
			slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.375f, 0.75f), glm::vec3(0.375f, 0.85f, 0.0f), glm::vec3(0.0f, 0.0f, -45.0f));
			wall.Join(slopeBeam);
		}
	}

	if ((type == WallType::window || type == WallType::balcony) && !config.lod)
	{
		Shape frame = GeneratePart(PartType::beam, glm::vec3(0.7f, 0.5f, 0.7f), glm::vec3(0.0f, 0.725f, 0.0f), glm::vec3(0.0f));
		wall.Join(frame);
		frame = GeneratePart(PartType::collumn, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(-0.75f, 0.0f, 0.0f), glm::vec3(0.0f));
		wall.Join(frame);
		frame = GeneratePart(PartType::collumn, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(-0.25f, 0.0f, 0.0f), glm::vec3(0.0f));
		wall.Join(frame);

		if (type == WallType::window)
		{
			frame = GeneratePart(PartType::beam, glm::vec3(0.7f, 0.5f, 0.7f), glm::vec3(0.0f, 0.275f, 0.0f), glm::vec3(0.0f));
			wall.Join(frame);
		}
	}

	if (type == WallType::balcony && !config.lod)
	{
		Shape balcony = GeneratePart(PartType::floor, glm::vec3(1.0f, 1.0f, 0.5f), glm::vec3(0.0f, 0.0f, 0.75f), glm::vec3(0.0f));

		Shape beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.5f, 0.75f), glm::vec3(0.5f, 0.0f, 0.25f), glm::vec3(0.0f, 90.0f, 0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.5f, 0.75f), glm::vec3(-0.5f, 0.0f, 0.25f), glm::vec3(0.0f, -90.0f, 0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(0.0f, 0.25f, 0.5f), glm::vec3(0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.5f, 0.75f), glm::vec3(0.5f, 0.25f, 0.25f), glm::vec3(0.0f, 90.0f, 0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.5f, 0.75f), glm::vec3(-0.5f, 0.25f, 0.25f), glm::vec3(0.0f, -90.0f, 0.0f));
		balcony.Join(beam);

		Shape tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(0.0f, -0.375f, 0.25f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-1.0f, -0.375f, 0.25f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(0.0f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-1.0f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-0.25f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-0.75f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-0.5f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);

		Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.55f, 0.75f), glm::vec3(0.375f, -0.15f, 0.0f), glm::vec3(0.0f, 0.0f, -30.0f));
		slopeBeam.Rotate90(1);
		slopeBeam.Move(glm::vec3(0.0f, 0.0f, 1.125f) * config.scale);
		balcony.Join(slopeBeam);
		slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.55f, 0.75f), glm::vec3(0.375f, -0.15f, 0.0f), glm::vec3(0.0f, 0.0f, -30.0f));
		slopeBeam.Rotate90(1);
		slopeBeam.Move(glm::vec3(-1.0f, 0.0f, 1.125f) * config.scale);
		balcony.Join(slopeBeam);

		wall.Join(balcony);
	}

	if (direction == S) wall.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == E) wall.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == W) wall.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	wall.Move(offset);

	shape.Join(wall);
}

void BuildingGenerator::GenerateBeams(int i, int x, int y)
{
	Shape beams;
	beams.normal = true;
	beams.coordinate = true;
	beams.color = true;

	Walls walls = cells[i][x][y].walls;

	bool N_Empty_B = CellEmpty(i, x, y + 1, false);
    bool N_Empty = CellEmpty(i, x, y + 1);
    bool S_Empty = CellEmpty(i, x, y - 1);
    bool E_Empty_B = CellEmpty(i, x + 1, y, false);
    bool E_Empty = CellEmpty(i, x + 1, y);
    bool W_Empty = CellEmpty(i, x - 1, y);

	bool empties[] = {N_Empty, E_Empty, S_Empty, W_Empty};
	WallType types[] = {walls.N_Type, walls.E_Type, walls.S_Type, walls.W_Type};

	for (int d = 0; d < 4; d++)
	{
		if (types[d] == WallType::beams)
		{
			Shape collumn = GeneratePart(PartType::collumn);
			if (empties[d] && empties[(d + 1) % 4])
			{
				
				Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.375f, 0.75f), glm::vec3(0.375f, 0.85f, 0.0f), glm::vec3(0.0f, 0.0f, -45.0f));
				collumn.Join(slopeBeam);
				slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.375f, 0.75f), glm::vec3(-0.375f, 0.85f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f));
				slopeBeam.Rotate90(1);
				collumn.Join(slopeBeam);
			}
			collumn.Rotate90(d);
			beams.Join(collumn);
		}
	}

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

	beams.Move(glm::vec3(x, i, y) * config.scale);
	shape.Join(beams);
}

void BuildingGenerator::GenerateRoofs(int level)
{
	for (int x = 0; x < cells[level].size(); x++)
	{
		for (int y = 0; y < cells[level][x].size(); y++)
		{
			if (!cells[level][x][y].roof.Empty())
			{
				GenerateRoof(level, x, y, cells[level][x][y].roof.type, cells[level][x][y].roof.direction);
			}
		}
	}
}

void BuildingGenerator::GenerateRoof(int i, int x, int y, RoofType type, D direction)
{
	Roof roofData = cells[i][x][y].roof;

	Shape roof;
	roof.coordinate = true;
	roof.normal = true;
	roof.color = true;

	if (type == RoofType::flatUp)
	{
		roof = GeneratePart(PartType::flatRoof);
		float extendScale = 1.0f;
		float extendOffset = 0.0f;
		if (roofData.LocalExtend(N))
		{
			extendScale += 0.2f;
			extendOffset += 0.1f;
		}
		if (roofData.LocalExtend(S))
		{
			extendScale += 0.2f;
			extendOffset -= 0.1f;
		}
		roof.Scale(glm::vec3(1.0f, 1.0f, extendScale));
		roof.Move(glm::vec3(0.0f, 0.0f, extendOffset) * config.scale);

		if (!config.lod)
		{
			Shape sideBeam = GeneratePart(PartType::beam);
			sideBeam.Rotate90(-1);
			sideBeam.Scale(glm::vec3(1.0f, 1.0f, extendScale));
			sideBeam.Move(glm::vec3(0.0f, 0.0f, extendOffset) * config.scale);
			sideBeam.Move(Buildings::flatRoofConfig.offset * config.scale);
			roof.Join(sideBeam);

			if (!CellValid(i, x + 1, y) || (cells[i][x + 1][y].roof.type != RoofType::flatUp &&
				cells[i][x + 1][y].roof.type != RoofType::slope))
			{
				sideBeam = GeneratePart(PartType::beam);
				sideBeam.Rotate90();
				sideBeam.Scale(glm::vec3(1.0f, 1.0f, extendScale));
				sideBeam.Move(glm::vec3(0.0f, 0.0f, extendOffset) * config.scale);
				sideBeam.Move(Buildings::flatRoofConfig.offset * config.scale);
				roof.Join(sideBeam);
			}
		}

		for (int d = 0; d < 4; d++)
		{
			if (roofData.Extend((D)d))
			{
				Shape wall = GeneratePart(PartType::flatWall);

				if (!config.lod)
				{
					Shape beam = GeneratePart(PartType::beam);
					beam.Move(glm::vec3(0.0f, 0.95f, 0.0f) * config.scale);
					wall.Join(beam);

					Shape collumn = GeneratePart(PartType::collumn);
					wall.Join(collumn);

					beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(0.0f, 0.0f, 0.075f), glm::vec3(0.0f));
					beam.Move(glm::vec3(0.0f, 0.95f, 0.0f) * config.scale);
					wall.Join(beam);
				}

				wall.Rotate90(d);
				roof.Join(wall);
			}
		}
	}
	else if (type == RoofType::slope || type == RoofType::slopeLow)
	{
		PartType slopedRoof = type == RoofType::slope ? PartType::slopedRoof : PartType::slopedLowRoof;
		PartType slopedWall = type == RoofType::slope ? PartType::slopedWall : PartType::slopedLowWall;

		roof = GeneratePart(slopedRoof);
		float extendScale = 1.0f;
		float extendOffset = 0.0f;
		if (roofData.LocalExtend(E))
		{
			extendScale += 0.2f;
			extendOffset -= 0.1f;
		}
		if (roofData.LocalExtend(W))
		{
			extendScale += 0.2f;
			extendOffset += 0.1f;
		}
		roof.Scale(glm::vec3(extendScale, 1.0f, 1.0f));
		roof.Move(glm::vec3(extendOffset, 0.0f, 0.0f) * config.scale);

		if (!config.lod)
		{
			if (!CellValid(i, x + 1, y) || (cells[i][x + 1][y].roof.type != RoofType::flatUp &&
				cells[i][x + 1][y].roof.type != RoofType::slope))
			{
				Shape sideBeam = GeneratePart(PartType::beam);
				sideBeam.Scale(glm::vec3(extendScale, 1.0f, 1.0f));
				sideBeam.Move(glm::vec3(extendOffset, 0.0f, 0.0f) * config.scale);
				sideBeam.Move(Buildings::flatRoofConfig.offset * config.scale);
				sideBeam.Move(glm::vec3(0.0f, 0.0f, Buildings::beamConfig.offset.z) * -2.0f * config.scale);
				roof.Join(sideBeam);
			}

			if (roofData.Extend(E))
			{
				Shape sideBeam = GeneratePart(PartType::beam);
				sideBeam.Scale(glm::vec3(extendScale, 1.0f, 1.0f));
				sideBeam.Move(glm::vec3(extendOffset, 0.0f, 0.0f) * config.scale);
				sideBeam.Move(Buildings::flatRoofConfig.offset * config.scale);
				sideBeam.Move(glm::vec3(0.0f, 0.0f, Buildings::beamConfig.offset.z) * -2.0f * config.scale);
				roof.Join(sideBeam);
			}

			Shape midBeam = GeneratePart(PartType::slopedBeam, glm::vec3(0.75f, 1.3f, 0.75f), glm::vec3(-0.05f, -0.035f, -0.5f), glm::vec3(0.0f));
			midBeam.Rotate90();
			midBeam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * config.scale);
			roof.Join(midBeam);
		}

		if (roofData.LocalExtend(W))
		{
			Shape wall = GeneratePart(slopedWall);
			wall.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
			wall.Rotate90();
			roof.Join(wall);

			if (!config.lod)
			{
				Shape collumn = GeneratePart(PartType::collumn);
				collumn.Move(glm::vec3(0.0f, 0.0f, -Buildings::collumnConfig.offset.z * 2.0f) * config.scale);
				roof.Join(collumn);

				Shape beam = GeneratePart(PartType::slopedBeam);
				beam.Rotate90();
				beam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * config.scale);
				roof.Join(beam);

				beam = GeneratePart(PartType::slopedBeam, glm::vec3(0.75f, 1.3f, 0.75f), glm::vec3(-0.05f, -0.035f, 0.075f), glm::vec3(0.0f));
				beam.Rotate90();
				beam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * config.scale);
				roof.Join(beam);
			}
		}

		if (roofData.LocalExtend(E))
		{
			Shape wall = GeneratePart(slopedWall);
			wall.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
			wall.Rotate90();
			wall.Move(glm::vec3(-Buildings::slopedWallConfig.offset.z * 2.0f, 0.0f, 0.0f) * config.scale);

			if (!config.lod)
			{
				Shape beam = GeneratePart(PartType::slopedBeam, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 180.0f, 0.0f));
				beam.Rotate90(-1);
				beam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * config.scale);
				roof.Join(beam);

				beam = GeneratePart(PartType::slopedBeam, glm::vec3(0.75f, 1.3f, 0.75f), glm::vec3(0.05f, -0.035f, 0.075f), glm::vec3(0.0f, 180.0f, 0.0f));
				beam.Rotate90(-1);
				beam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * config.scale);
				roof.Join(beam);
			}

			roof.Join(wall);
		}
	}
	else if (type == RoofType::cone)
	{
		roof = GeneratePart(PartType::coneRoof);
		float extendScale = 1.0f;
		float extendOffset = 0.0f;
		if (roofData.LocalExtend(N))
		{
			extendScale += 0.2f;
			extendOffset += 0.1f;
		}
		if (roofData.LocalExtend(S))
		{
			extendScale += 0.2f;
			extendOffset -= 0.1f;
		}
		roof.Scale(glm::vec3(1.0f, 1.0f, extendScale));
		roof.Move(glm::vec3(0.0f, 0.0f, extendOffset) * config.scale);

		if (roofData.LocalMerge(N))
		{
			extendScale += 0.4f;
			extendOffset += 0.2f;
		}
		if (roofData.LocalMerge(S))
		{
			extendScale += 0.4f;
			extendOffset -= 0.2f;
		}

		if (!config.lod)
		{
			Shape sideBeam = GeneratePart(PartType::beam, glm::vec3(1.0f, extendScale, 0.75f), glm::vec3(0.0f, 0.3125f, -Buildings::beamConfig.offset.z + extendOffset), glm::vec3(0.0f, -90.0f, 0.0f));
			sideBeam.Scale(glm::vec3(1.0f, 1.5f, 1.0f));
			roof.Join(sideBeam);

			Shape midBeamLeft = GeneratePart(PartType::slightSlopedBeam, glm::vec3(0.75f, 1.35f, 0.75f), glm::vec3(-0.1f, -0.05f, -0.5f), glm::vec3(0.0f));
			midBeamLeft.Move(glm::vec3(-0.25f, 0.125f, 0.0f) * config.scale);
			roof.Join(midBeamLeft);

			Shape midBeamRight = GeneratePart(PartType::slightSlopedBeam, glm::vec3(0.75f, 1.35f, 0.75f), glm::vec3(0.1f, -0.05f, -0.5f), glm::vec3(0.0f, 180.0f, 0.0f));
			midBeamRight.Move(glm::vec3(0.25f, 0.125f, 0.0f) * config.scale);
			roof.Join(midBeamRight);
		}

		for (int d = 0; d <= 2; d += 2)
		{
			if (roofData.Extend((D)(d + roofData.direction)))
			{
				Shape wallLeft = GeneratePart(PartType::slopedLowWall);
				wallLeft.Scale(glm::vec3(0.5f, 0.3f, 1.0f));
				wallLeft.Move(glm::vec3(-0.25f, 0.0f, 0.0f) * config.scale);
				wallLeft.Rotate90(d);

				Shape wallRight = GeneratePart(PartType::slopedLowWall, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 180.0f, 0.0f));
				wallRight.Scale(glm::vec3(0.5f, 0.3f, 1.0f));
				wallRight.Move(glm::vec3(0.25f, 0.0f, 0.0f) * config.scale);
				wallRight.Rotate90(d);

				if (!config.lod)
				{
					Shape beamLeft = GeneratePart(PartType::slightSlopedBeam);
					beamLeft.Move(glm::vec3(-0.25f, 0.125f, 0.0f) * config.scale);
					beamLeft.Rotate90(d);
					roof.Join(beamLeft);

					Shape beamRight = GeneratePart(PartType::slightSlopedBeam, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 180.0f, 0.0f));
					beamRight.Move(glm::vec3(0.25f, 0.125f, 0.0f) * config.scale);
					beamRight.Rotate90(d);
					roof.Join(beamRight);

					beamLeft = GeneratePart(PartType::slightSlopedBeam, glm::vec3(0.75f, 1.35f, 0.75f), glm::vec3(-0.1f, -0.05f, 0.075f), glm::vec3(0.0f));
					beamLeft.Move(glm::vec3(-0.25f, 0.125f, 0.0f) * config.scale);
					beamLeft.Rotate90(d);
					roof.Join(beamLeft);

					beamRight = GeneratePart(PartType::slightSlopedBeam, glm::vec3(0.75f, 1.35f, 0.75f), glm::vec3(0.1f, -0.05f, 0.075f), glm::vec3(0.0f, 180.0f, 0.0f));
					beamRight.Move(glm::vec3(0.25f, 0.125f, 0.0f) * config.scale);
					beamRight.Rotate90(d);
					roof.Join(beamRight);
				}

				roof.Join(wallLeft);
				roof.Join(wallRight);
			}

			if (roofData.LocalMerge(N))
			{
				Shape extension = GeneratePart(PartType::coneRoofExtension);
				extension.Move(glm::vec3(0.0f, 0.0f, 0.5f) * config.scale);

				roof.Join(extension);
			}
			if (roofData.LocalMerge(S))
			{
				Shape extension = GeneratePart(PartType::coneRoofExtension);
				extension.Move(glm::vec3(0.0f, 0.0f, 0.5f) * config.scale);
				extension.Rotate90(2);

				roof.Join(extension);
			}
		}
	}

	if (direction == S) roof.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == E) roof.Rotate(90.0f * (type == RoofType::cone ? 1.0f : -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == W) roof.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	roof.Move(glm::vec3(x, i + 1, y) * config.scale);

	shape.Join(roof);
}