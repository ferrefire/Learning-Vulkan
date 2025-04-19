#include "settlement.hpp"

#include "manager.hpp"
#include "terrain.hpp"
#include "utilities.hpp"
#include "data.hpp"
#include "simulation.hpp"

#include <iostream>

Settlement::Settlement()
{
    
}

Settlement::~Settlement()
{
    Destroy();
}

void Settlement::Start(int id, glm::vec3 position)
{
    this->id = id;
	this->position = position;
	Data::RequestData(position, &this->position.y);
}

void Settlement::Destroy()
{
    for (SettlementChunk &chunk : chunks)
    {
        for (int x = 0; x < CHUNK_LENGTH; x++)
        {
            for (int y = 0; y < CHUNK_LENGTH; y++)
            {
                SettlementCell &cell = chunk.cells[x][y];
                if (cell.building)
                {
                    Buildings::DestroyBuilding(cell.building->id);
                }
            }
        }
    }
}

void Settlement::AddChunk(glm::ivec2 coordinates)
{
	if (GetChunk(coordinates.x, coordinates.y) != nullptr) return;

	chunks.resize(chunks.size() + 1);
	chunks[chunks.size() - 1].Setup(id, chunks.size() - 1, coordinates);
}

/*void Settlement::IncreaseRadius(int amount)
{
    radius += amount;
    length = radius * 2 + 1;
    std::vector<SettlementChunk> originalChunks = chunks;
    chunks.clear();
    chunks.resize(length * length);

    int i = 0;
    for (SettlementChunk &chunk : chunks)
    {
        int x = i / length;
        int y = i % length;
        chunk.Setup(x, y, id);
        i++;
    }

    for (SettlementChunk chunk : originalChunks)
    {
        int x = chunk.index.x + amount;
        int y = chunk.index.y + amount;
        i = y * length + x;

        for (int x = 0; x < CHUNK_LENGTH; x++)
        {
            for (int y = 0; y < CHUNK_LENGTH; y++)
            {
                chunks[i].cells[x][y].building = chunk.cells[x][y].building;
            }
        }
    }

    std::cout << "increased" << std::endl;
}*/

/*void SettlementChunk::Setup(int x, int y, int id)
{
    settlementID = id;
    index = glm::ivec2(x, y);
    int settlementRadius = Simulation::settlements[id]->radius;
    localPosition = glm::vec3(x - settlementRadius, 0, y - settlementRadius) * (CELL_SIZE * CHUNK_LENGTH);
    Data::RequestData(localPosition + Simulation::settlements[settlementID]->position, &localPosition.y);

    for (int cx = 0; cx < CHUNK_LENGTH; cx++)
    {
        for (int cy = 0; cy < CHUNK_LENGTH; cy++)
        {
            cells[cx][cy].settlementID = id;
            cells[cx][cy].chunkIndex = index;
            cells[cx][cy].index = glm::ivec2(cx, cy);
            cells[cx][cy].localPosition.x = (cx - CHUNK_RADIUS) * CELL_SIZE;
            cells[cx][cy].localPosition.y = 0;
            cells[cx][cy].localPosition.z = (cy - CHUNK_RADIUS) * CELL_SIZE;
            //Data::RequestData(localPosition + Simulation::settlements[settlementID]->position, &localPosition[1]);
        }
    }

    initialized = true;
}*/

void SettlementChunk::Setup(int settlementID, int id, glm::ivec2 coordinates)
{
	this->settlementID = settlementID;
	this->id = id;
	this->coordinates = coordinates;
	this->localPosition = glm::vec3(coordinates.x, 0.0f, coordinates.y) * (CHUNK_LENGTH * CELL_SIZE);
	Data::RequestData(localPosition + Simulation::settlements[settlementID]->position, &localPosition.y);

	for (int cx = 0; cx < CHUNK_LENGTH; cx++)
	{
		for (int cy = 0; cy < CHUNK_LENGTH; cy++)
		{
			cells[cx][cy].settlementID = settlementID;
			cells[cx][cy].chunkID = id;
			cells[cx][cy].index = glm::ivec2(cx, cy);
			cells[cx][cy].coordinates = glm::ivec2(cx - CHUNK_RADIUS, cy - CHUNK_RADIUS);
			cells[cx][cy].localPosition.x = cells[cx][cy].coordinates.x * CELL_SIZE;
			cells[cx][cy].localPosition.y = 0;
			cells[cx][cy].localPosition.z = cells[cx][cy].coordinates.y * CELL_SIZE;
			cells[cx][cy].building = nullptr;
			// Data::RequestData(localPosition + Simulation::settlements[settlementID]->position, &localPosition[1]);
		}
	}

	initialized = true;
}

SettlementCell *SettlementChunk::GetCell(int x, int y)
{
    if (x < 0 || x >= CHUNK_LENGTH || y < 0 || y >= CHUNK_LENGTH) return (nullptr);

	std::cout << "cell coordinates " << x << " " << y << std::endl;

    return (&cells[x][y]);
}

SettlementCell *SettlementChunk::GetCell(glm::vec3 target, bool local)
{
    if (!local) target -= localPosition;

    int x = glm::floor((target.x + (CELL_SIZE * 0.5f)) / CELL_SIZE) + CHUNK_RADIUS;
	int y = glm::floor((target.z + (CELL_SIZE * 0.5f)) / CELL_SIZE) + CHUNK_RADIUS;

	return (GetCell(x, y));
}

SettlementChunk *Settlement::GetChunk(int x, int y)
{
	glm::ivec2 targetCoordinates = glm::ivec2(x, y);

    for (int i = 0; i < chunks.size(); i++)
	{
		if (chunks[i].coordinates == targetCoordinates)
		{
			return (&chunks[i]);
		}
	}

	return (nullptr);
}

SettlementChunk *Settlement::GetChunk(glm::vec3 target, bool local)
{
    if (!local) target -= position;

    int x = glm::floor((target.x + (CELL_SIZE * CHUNK_LENGTH * 0.5f)) / (CELL_SIZE * CHUNK_LENGTH));
    int y = glm::floor((target.z + (CELL_SIZE * CHUNK_LENGTH * 0.5f)) / (CELL_SIZE * CHUNK_LENGTH));

    return (GetChunk(x, y));
}

SettlementCell *Settlement::GetCell(glm::vec3 target, bool local)
{
    SettlementChunk *chunk = GetChunk(target, local);

    if (!chunk) return (nullptr);

    std::cout << "chunk coordinates " << chunk->coordinates.x << " " << chunk->coordinates.y << std::endl;

    return (chunk->GetCell(target));
}

void Settlement::GenerateBuilding(SettlementCell *cell)
{
    cell->building = Buildings::CreateBuilding();
    cell->building->position = cell->localPosition;
    cell->building->position += chunks[cell->chunkID].localPosition;
    cell->building->position += position;
    cell->building->position.y = 0.0f;
    cell->building->Update();

    Data::RequestData(cell->building->position, &cell->building->position.y, Buildings::UpdateBuilding, cell->building->id);
}

std::vector<SettlementCell *> Settlement::GetRenderCells()
{
    std::vector<SettlementCell *> renderCells;

    glm::vec3 worldSpace = position - Terrain::terrainOffset;

	float dis = Utilities::DistanceSqrd(worldSpace, Manager::camera.Position());
	if (dis > 6000.0f * 6000.0f) return (renderCells);

	for (SettlementChunk &chunk : chunks)
	{
		worldSpace = position + chunk.localPosition;
        worldSpace.y = chunk.localPosition.y;
        worldSpace -= Terrain::terrainOffset;

		if (Manager::camera.AreaInView(worldSpace, (CELL_SIZE * 0.5f) + (CELL_SIZE * CHUNK_RADIUS)))
        {
            dis = Utilities::DistanceSqrd(worldSpace, Manager::camera.Position());
            bool lod = true;
            if (dis < 1000.0f * 1000.0f) lod = false;

            for (int x = 0; x < CHUNK_LENGTH; x++)
            {
                for (int y = 0; y < CHUNK_LENGTH; y++)
                {
                    if (chunk.cells[x][y].building)
                    {
                        if ((lod && !chunk.cells[x][y].building->lodMesh.created) || (!lod && !chunk.cells[x][y].building->mesh.created))
                            Buildings::CreateBuildingMesh(chunk.cells[x][y].building, lod);

                        chunk.cells[x][y].building->lod = lod;
                        renderCells.push_back(&chunk.cells[x][y]);
                    }
                }
            }
        }
	}

    return (renderCells);
}