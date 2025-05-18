#include "settlement.hpp"

#include "manager.hpp"
#include "terrain.hpp"
#include "utilities.hpp"
#include "data.hpp"
#include "simulation.hpp"
#include "random.hpp"
#include "time.hpp"
#include "buildingGenerator.hpp"

#include <iostream>
#include <thread>

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
	this->settlementBuilding = new Building;
	this->settlementBuilding->lod = true;
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

		if (chunk.chunkBuilding)
		{
			chunk.chunkBuilding->mesh.Destroy();
			chunk.chunkBuilding->lodMesh.Destroy();
			delete(chunk.chunkBuilding);
		}
    }

	settlementBuilding->mesh.Destroy();
	settlementBuilding->lodMesh.Destroy();
	delete(settlementBuilding);
}

void Settlement::AddChunk(glm::ivec2 coordinates)
{
	if (GetChunk(coordinates.x, coordinates.y) != nullptr) return;

	chunks.resize(chunks.size() + 1);
	chunks[chunks.size() - 1].Setup(id, chunks.size() - 1, coordinates);
}

void Do(Building *building)
{
	if (building->lod)
	{
		building->lodMesh.coordinate = true;
		building->lodMesh.normal = true;
		building->lodMesh.color = true;
	}
	else
	{
		building->mesh.coordinate = true;
		building->mesh.normal = true;
		building->mesh.color = true;
	}

	GenerationConfig config = Buildings::generationConfig;
	config.lod = building->lod;
	BuildingGenerator bg(config, building->cells);

	if (building->lod)
	{
		building->lodMesh.shape = bg.GetShape();
		building->lodMesh.RecalculateVertices();
	}
	else
	{
		building->mesh.shape = bg.GetShape();
		building->mesh.RecalculateVertices();
	}
	//currentMesh->Create();
}

void Settlement::FillChunk(glm::ivec2 coordinates)
{
	SettlementChunk *chunk = GetChunk(coordinates.x, coordinates.y);

	if (chunk == nullptr) return;

	for (int x = -CHUNK_RADIUS; x <= CHUNK_RADIUS; x++)
	{
		for (int y = -CHUNK_RADIUS; y <= CHUNK_RADIUS; y++)
		{
			//if (!(x == 0 && y == 0) && ((x == 0 && abs(y) == CHUNK_RADIUS) || (y == 0 && abs(x) == CHUNK_RADIUS) || (abs(x) <= 1 && abs(y) <= 1)))
			//	continue;
			if ((x == 0 || y == 0) || (abs(x) == CHUNK_RADIUS && abs(y) == CHUNK_RADIUS))
				continue;

			GenerateBuilding(chunk->localPosition + glm::vec3(CELL_SIZE * x, 0.0f, CELL_SIZE * y), true);
		}
	}
}

void SettlementChunk::Setup(int settlementID, int id, glm::ivec2 coordinates)
{
	this->settlementID = settlementID;
	this->id = id;
	this->coordinates = coordinates;
	this->localPosition = glm::vec3(coordinates.x, 0.0f, coordinates.y) * (CHUNK_LENGTH * CELL_SIZE);
	this->chunkBuilding = new Building;
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

void SettlementChunk::GenerateChunkMesh()
{
	Mesh *currentMesh = chunkBuilding->GetMesh();

	currentMesh->shape.coordinate = true;
	currentMesh->shape.normal = true;
	currentMesh->shape.color = true;
	currentMesh->coordinate = true;
	currentMesh->normal = true;
	currentMesh->color = true;

	for (int cx = 0; cx < CHUNK_LENGTH; cx++)
	{
		for (int cy = 0; cy < CHUNK_LENGTH; cy++)
		{
			if (cells[cx][cy].building && cells[cx][cy].building->GetMesh(chunkBuilding->lod)->created)
			{
				Shape temp = cells[cx][cy].building->GetMesh(chunkBuilding->lod)->shape;
				temp.Rotate(cells[cx][cy].building->rotation.y, glm::vec3(0, 1, 0));
				temp.Move(cells[cx][cy].building->position);
				currentMesh->shape.Join(temp);
			}
		}
	}

	currentMesh->RecalculateVertices();
	currentMesh->Create();

	//std::cout << "chunk mesh generated" << std::endl;
}

bool SettlementChunk::CanGenerateChunkMesh()
{
	for (int cx = 0; cx < CHUNK_LENGTH; cx++)
	{
		for (int cy = 0; cy < CHUNK_LENGTH; cy++)
		{
			if (cells[cx][cy].building && !cells[cx][cy].building->GetMesh(chunkBuilding->lod)->created)
			{
				return (false);
			}
		}
	}

	return (true);
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
	cell->building->rotation.y += Random::Int(cell->seed, -10, 10);
	//std::cout << "seed: " << cell->seed << " result: " << Random::Float(cell->seed, -45.0f, 45.0f) << std::endl;
    cell->building->Update();

    Data::RequestData(cell->building->position, &cell->building->position.y, Buildings::UpdateBuilding, cell->building->id);
}

void Settlement::GenerateBuilding(glm::vec3 target, bool local)
{
	GenerateBuilding(GetCell(target, local));
}

bool Settlement::CanGenerateSettlementMesh()
{
	if (chunks.size() <= 0) return (false);

	for (SettlementChunk &chunk : chunks)
	{
		if (!chunk.chunkBuilding->lodMesh.created)
		{
			return (false);
		}
	}

	return (true);
}

void Settlement::GenerateSettlementMesh()
{
	Mesh *currentMesh = &settlementBuilding->lodMesh;

	currentMesh->shape.coordinate = true;
	currentMesh->shape.normal = true;
	currentMesh->shape.color = true;
	currentMesh->coordinate = true;
	currentMesh->normal = true;
	currentMesh->color = true;

	for (SettlementChunk &chunk : chunks)
	{
		if (chunk.chunkBuilding->lodMesh.created)
		{
			Shape temp = chunk.chunkBuilding->lodMesh.shape;
			temp.Rotate(chunk.chunkBuilding->rotation.y, glm::vec3(0, 1, 0));
			temp.Move(chunk.chunkBuilding->position);
			currentMesh->shape.Join(temp);
		}
	}

	currentMesh->RecalculateVertices();
	currentMesh->Create();
}

SettlementRenderData Settlement::GetRenderData()
{
	SettlementRenderData renderData;
    //std::vector<SettlementCell *> renderCells;

    glm::vec3 worldSpace = position - Terrain::terrainOffset;

	float dis = Utilities::DistanceSqrd(worldSpace, Manager::camera.Position());

	//if (dis > 10000.0f * 10000.0f) return (renderData);

	//if ((dis > 4000.0f * 4000.0f) && (Manager::camera.AreaInView(worldSpace, (CELL_SIZE * (CHUNK_RADIUS + 1)) * (MaxCoordinate() + 1))));
	//if ((dis > 7500.0f * 7500.0f) && (Manager::camera.AreaInView(worldSpace, (CELL_SIZE * (CHUNK_RADIUS + 1)) * (MaxCoordinate() + 1))));
	//{
	//	settlementBuilding->lod = true;
	//	if (settlementBuilding->lodMesh.created)
	//	{
	//		renderData.settlementBuilding = settlementBuilding;
	//		return (renderData);
	//	}
	//	else if (CanGenerateSettlementMesh())
	//	{
	//		GenerateSettlementMesh();
	//	}
	//}

	int inChunk = GetChunkID(Manager::camera.Position() + Terrain::terrainOffset);

	std::vector<ProximityData> chunkProximities = GetChunkProximity(Manager::camera.Position());

	if (chunkProximities.size() > 0 && chunkProximities[0].distanceSquared > 4000.0f * 4000.0f)
	{
		if (chunkProximities[0].distanceSquared > 15000.0f * 15000.0f || !Manager::camera.AreaInView(worldSpace, (CELL_SIZE * (CHUNK_RADIUS + 1)) * (MaxCoordinate() + 1))) return (renderData);

		settlementBuilding->lod = true;
		if (settlementBuilding->lodMesh.created)
		{
			renderData.settlementBuilding = settlementBuilding;
			return (renderData);
		}
		else if (CanGenerateSettlementMesh())
		{
			GenerateSettlementMesh();
		}
	}

	for (ProximityData &chunkProximity : chunkProximities)
	{
		SettlementChunk &chunk = chunks[chunkProximity.id];

		worldSpace = position + chunk.localPosition;
        worldSpace.y = chunk.localPosition.y;
        worldSpace -= Terrain::terrainOffset;

		dis = chunkProximity.distanceSquared;
		if (dis > 5000.0f * 5000.0f) continue;

		if (chunk.id == inChunk || Manager::camera.AreaInView(worldSpace, (CELL_SIZE * (CHUNK_RADIUS + 1))))
        {
			//buildingsToGenerate.clear();

            bool lod = true;
            if (dis < 2000.0f * 2000.0f) lod = false;

			chunk.chunkBuilding->lod = lod;

			if (dis > 250.0f * 250.0f)
			{
				if (chunk.chunkBuilding->GetMesh()->created)
				{
					renderData.renderChunks.push_back(&chunk);
					continue;
				}
				else if (chunk.CanGenerateChunkMesh())
				{
					chunk.GenerateChunkMesh();
				}
			}

            for (int x = 0; x < CHUNK_LENGTH; x++)
            {
                for (int y = 0; y < CHUNK_LENGTH; y++)
                {
                    if (chunk.cells[x][y].building)
                    {
						bool needsMesh = (lod && !chunk.cells[x][y].building->lodMesh.created) || (!lod && !chunk.cells[x][y].building->mesh.created);
                        if (needsMesh && Simulation::generating)
                        {
							//buildingsToGenerate.push_back(chunk.cells[x][y].building);
							Simulation::generating = false;
							Buildings::CreateBuildingMesh(chunk.cells[x][y].building, lod);
							needsMesh = false;
						}

                        if (!needsMesh)
						{
							chunk.cells[x][y].building->lod = lod;
							renderData.renderCells.push_back(&chunk.cells[x][y]);
						}
						else if (chunk.cells[x][y].building->lodMesh.created)
						{
							chunk.cells[x][y].building->lod = true;
							renderData.renderCells.push_back(&chunk.cells[x][y]);
						}
                    }
                }
            }

			//if (Simulation::generating) GenerateBuildings();
        }
	}

    return (renderData);
}

std::vector<ProximityData> Settlement::GetChunkProximity(glm::vec3 target)
{
	std::vector<ProximityData> proximityResults;

	target += Terrain::terrainOffset;

	for (SettlementChunk &chunk : chunks)
	{
		ProximityData chunkProximity;
		chunkProximity.id = chunk.id;
		chunkProximity.distanceSquared = Utilities::DistanceSqrd(chunk.GetGlobalPosition(), target);

		int l = 0;
		for (int k = 0; k < proximityResults.size(); k++)
		{
			if (chunkProximity.distanceSquared < proximityResults[k].distanceSquared) break;
			l++;
		}

		if (proximityResults.size() == 0)
		{
			proximityResults.resize(1);
			proximityResults[0] = chunkProximity;
		}
		else
		{
			proximityResults.insert(proximityResults.begin() + l, chunkProximity);
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

void Settlement::GenerateBuildings()
{
	if (buildingsToGenerate.size() == 0) return;

	//double start = Time::GetCurrentTime();

	Simulation::generating = false;

	std::vector<std::thread> threads(buildingsToGenerate.size());

	for (int i = 0; i < buildingsToGenerate.size(); i++)
	{
		threads[i] = std::thread(Do, buildingsToGenerate[i]);
	}

	for (int i = 0; i < buildingsToGenerate.size(); i++)
	{
		threads[i].join();
	}

	VkCommandBuffer commandBuffer = Manager::currentDevice.BeginGraphicsCommand();

	for (int i = 0; i < buildingsToGenerate.size(); i++)
	{
		if (buildingsToGenerate[i]->lod)
		{
			buildingsToGenerate[i]->lodMesh.Create(commandBuffer);
		}
		else
		{
			buildingsToGenerate[i]->mesh.Create(commandBuffer);
		}
	}

	Manager::currentDevice.EndGraphicsCommand(commandBuffer);

	Manager::currentDevice.DestroyStagingBuffers();

	buildingsToGenerate.clear();

	//std::cout << "duration: " << (Time::GetCurrentTime() - start) << std::endl;
}

int Settlement::MaxCoordinate()
{
	int maxCoordinate = 0;

	for (SettlementChunk &chunk : chunks)
	{
		if (abs(chunk.coordinates.x) > maxCoordinate) maxCoordinate = abs(chunk.coordinates.x);
		if (abs(chunk.coordinates.y) > maxCoordinate) maxCoordinate = abs(chunk.coordinates.y);
	}

	return (maxCoordinate);
}