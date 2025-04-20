#include "settlement.hpp"

#include "manager.hpp"
#include "terrain.hpp"
#include "utilities.hpp"
#include "data.hpp"
#include "simulation.hpp"
#include "random.hpp"

#include <iostream>

Settlement::Settlement()
{
    
}

Settlement::~Settlement()
{
    //Destroy();
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
                if (cell.building != nullptr)
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

void Settlement::FillChunk(glm::ivec2 coordinates)
{
	SettlementChunk *chunk = GetChunk(coordinates.x, coordinates.y);

	if (chunk == nullptr) return;

	//GenerateBuilding(chunk->localPosition + glm::vec3(0.0f, 0.0f, 0.0f), true);
	//GenerateBuilding(chunk->localPosition + glm::vec3(CELL_SIZE * 2.0f, 0.0f, 0.0f), true);
	//GenerateBuilding(chunk->localPosition + glm::vec3(CELL_SIZE * 2.0f, 0.0f, 0.0f), true);

	for (int x = -CHUNK_RADIUS; x <= CHUNK_RADIUS; x++)
	{
		for (int y = -CHUNK_RADIUS; y <= CHUNK_RADIUS; y++)
		{
			if (!(x == 0 && y == 0) && ((x == 0 && abs(y) == CHUNK_RADIUS) || (y == 0 && abs(x) == CHUNK_RADIUS) || (abs(x) <= 1 && abs(y) <= 1)))
				continue;

			GenerateBuilding(chunk->localPosition + glm::vec3(CELL_SIZE * x, 0.0f, CELL_SIZE * y), true);
		}
	}
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
	Data::RequestData(this->localPosition + Simulation::settlements[settlementID]->position, &this->localPosition.y);

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

			glm::vec3 globalPosition = cells[cx][cy].GetGlobalPosition();
			cells[cx][cy].seed = ((long int)Random::Int(int(globalPosition.x)) + (long int)Random::Int(int(globalPosition.z)) + (long int)Random::Int(id)) % RAND_MAX;
			// Data::RequestData(localPosition + Simulation::settlements[settlementID]->position, &localPosition[1]);
		}
	}

	initialized = true;
}

SettlementCell *SettlementChunk::GetCell(int x, int y)
{
    if (x < 0 || x >= CHUNK_LENGTH || y < 0 || y >= CHUNK_LENGTH) return (nullptr);

	//std::cout << "cell coordinates " << x << " " << y << std::endl;

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

int Settlement::GetChunkID(glm::vec3 target, bool local)
{
	if (!local) target -= position;

	int x = glm::floor((target.x + (CELL_SIZE * CHUNK_LENGTH * 0.5f)) / (CELL_SIZE * CHUNK_LENGTH));
	int y = glm::floor((target.z + (CELL_SIZE * CHUNK_LENGTH * 0.5f)) / (CELL_SIZE * CHUNK_LENGTH));

	SettlementChunk *chunk = GetChunk(x, y);

	return (chunk != nullptr ? chunk->id : -1);
}

SettlementCell *Settlement::GetCell(glm::vec3 target, bool local)
{
    SettlementChunk *chunk = GetChunk(target, local);

    if (!chunk) return (nullptr);

    //std::cout << "chunk coordinates " << chunk->coordinates.x << " " << chunk->coordinates.y << std::endl;

    return (chunk->GetCell(target));
}

void Settlement::GenerateBuilding(SettlementCell *cell)
{
	if (cell == nullptr || cell->building != nullptr) return;

    cell->building = Buildings::CreateBuilding(cell->seed);
    cell->building->position = cell->localPosition;
    cell->building->position += chunks[cell->chunkID].localPosition;
    cell->building->position += position;
    cell->building->position.y = 0.0f;
    cell->building->Update();

    Data::RequestData(cell->building->position, &cell->building->position.y, Buildings::UpdateBuilding, cell->building->id);
}

void Settlement::GenerateBuilding(glm::vec3 target, bool local)
{
	GenerateBuilding(GetCell(target, local));
}

std::vector<SettlementCell *> Settlement::GetRenderCells()
{
    std::vector<SettlementCell *> renderCells;

    glm::vec3 worldSpace = position - Terrain::terrainOffset;

	float dis = Utilities::DistanceSqrd(worldSpace, Manager::camera.Position());
	if (dis > 7500.0f * 7500.0f) return (renderCells);

	int inChunk = GetChunkID(Manager::camera.Position() + Terrain::terrainOffset);

	std::vector<ChunkProximity> chunkProximities = GetChunkProximity(Manager::camera.Position());

	//for (SettlementChunk &chunk : chunks)
	for (ChunkProximity &chunkProximity : chunkProximities)
	{
		SettlementChunk &chunk = chunks[chunkProximity.chunkID];

		worldSpace = position + chunk.localPosition;
        worldSpace.y = chunk.localPosition.y;
        worldSpace -= Terrain::terrainOffset;

		//dis = Utilities::DistanceSqrd(worldSpace, Manager::camera.Position());
		dis = chunkProximity.distanceSquared;
		if (dis > 5000.0f * 5000.0f) continue;

		if (chunk.id == inChunk || Manager::camera.AreaInView(worldSpace, (CELL_SIZE * 0.5f) + (CELL_SIZE * CHUNK_RADIUS)))
        {
            bool lod = true;
            if (dis < 1000.0f * 1000.0f) lod = false;

            for (int x = 0; x < CHUNK_LENGTH; x++)
            {
                for (int y = 0; y < CHUNK_LENGTH; y++)
                {
                    if (chunk.cells[x][y].building)
                    {
						bool needsMesh = (lod && !chunk.cells[x][y].building->lodMesh.created) || (!lod && !chunk.cells[x][y].building->mesh.created);
                        if (needsMesh && !generating)
                        {
							generating = true;
							Buildings::CreateBuildingMesh(chunk.cells[x][y].building, lod);
							needsMesh = false;

							//if (lod) std::cout << chunk.cells[x][y].building->lodMesh.vertices.size() << std::endl;
						}

                        if (!needsMesh)
						{
							chunk.cells[x][y].building->lod = lod;
							renderCells.push_back(&chunk.cells[x][y]);
						}
						else if (chunk.cells[x][y].building->lodMesh.created)
						{
							chunk.cells[x][y].building->lod = true;
							renderCells.push_back(&chunk.cells[x][y]);
						}
						
                    }
                }
            }
        }
	}

    return (renderCells);
}

std::vector<ChunkProximity> Settlement::GetChunkProximity(glm::vec3 target)
{
	std::vector<ChunkProximity> proximityData(chunks.size());
	std::vector<ChunkProximity> proximityResults;

	target += Terrain::terrainOffset;

	int i = 0;
	for (SettlementChunk &chunk : chunks)
	{
		proximityData[i].chunkID = chunk.id;
		proximityData[i].distanceSquared = Utilities::DistanceSqrd(chunk.GetGlobalPosition(), target);
		i++;
	}

	for (int j = 0; j < proximityData.size(); j++)
	{
		int l = 0;
		for (int k = 0; k < proximityResults.size(); k++)
		{
			if (proximityData[j].distanceSquared < proximityResults[k].distanceSquared) break;
			l++;
		}

		if (proximityResults.size() == 0)
		{
			proximityResults.resize(1);
			proximityResults[0] = proximityData[j];
		}
		else
		{
			proximityResults.insert(proximityResults.begin() + l, proximityData[j]);
		}
	}

	return (proximityResults);
}

glm::vec3 SettlementCell::GetGlobalPosition()
{
	glm::vec3 globalPosition = localPosition;
	globalPosition += Simulation::settlements[settlementID]->chunks[chunkID].localPosition;
	globalPosition += Simulation::settlements[settlementID]->position;

	return (globalPosition);
}

glm::vec3 SettlementChunk::GetGlobalPosition()
{
	glm::vec3 globalPosition = localPosition;
	globalPosition += Simulation::settlements[settlementID]->position;
	globalPosition.y = localPosition.y;

	return (globalPosition);
}