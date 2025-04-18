#include "settlement.hpp"

#include "manager.hpp"
#include "terrain.hpp"
#include "utilities.hpp"
#include "data.hpp"

Settlement::Settlement()
{
    radius = 0;
    length = radius * 2 + 1;
    chunks.resize(length * length);
    chunks[0].Setup(0, 0);
    //Set chunk y position!!! and also settlement itself
}

Settlement::~Settlement()
{
    Destroy();
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

void SettlementChunk::Setup(int x, int y)
{
    index = glm::ivec3(x, y, 0);
    localPosition = glm::vec3(x, 0, y) * (CELL_SIZE * CHUNK_LENGTH);

    for (int cx = 0; cx < CHUNK_LENGTH; cx++)
    {
        for (int cy = 0; cy < CHUNK_LENGTH; cy++)
        {
            cells[cx][cy].index = glm::ivec3(cx, cy, 0);
            cells[cx][cy].localPosition.x = (cx - CHUNK_RADIUS) * CELL_SIZE;
            cells[cx][cy].localPosition.z = (cy - CHUNK_RADIUS) * CELL_SIZE;
        }
    }
}

SettlementCell *SettlementChunk::GetCell(int x, int y)
{
    if (x < 0 || x >= CHUNK_LENGTH || y < 0 || y >= CHUNK_LENGTH) return (nullptr);

    return (&cells[x][y]);
}

SettlementCell *SettlementChunk::GetCell(glm::vec3 target, bool local)
{
    if (!local) target -= localPosition;

    int x = glm::floor(target.x / CELL_SIZE) + CHUNK_RADIUS;
    int y = glm::floor(target.z / CELL_SIZE) + CHUNK_RADIUS;

    return (GetCell(x, y));
}

SettlementChunk *Settlement::GetChunk(int x, int y)
{
    int index = y * length + x;
    
    if (index < 0 || index >= chunks.size()) return (nullptr);

    return (&chunks[index]);
}

SettlementChunk *Settlement::GetChunk(glm::vec3 target, bool local)
{
    if (!local) target -= position;

    int x = glm::floor(target.x / (CELL_SIZE * CHUNK_LENGTH)) + radius;
    int y = glm::floor(target.z / (CELL_SIZE * CHUNK_LENGTH)) + radius;

    return (GetChunk(x, y));
}

SettlementCell *Settlement::GetCell(glm::vec3 target, bool local)
{
    SettlementChunk *chunk = GetChunk(target, local);

    if (!chunk) return (nullptr);

    return (chunk->GetCell(target));
}

void Settlement::GenerateBuilding(SettlementCell *cell)
{
    cell->building = Buildings::CreateBuilding();
    cell->building->position += cell->localPosition;
    cell->building->position += chunks[cell->index.z].localPosition;
    cell->building->position += position;
    cell->building->Update();

    Data::RequestData(cell->building->position, &cell->building->position[1], Buildings::UpdateBuilding, cell->building->id);
}

std::vector<SettlementCell *> Settlement::GetRenderCells()
{
    std::vector<SettlementCell *> renderCells;

    glm::vec3 worldSpace = position - Terrain::terrainOffset;

	float dis = Utilities::DistanceSqrd(worldSpace, Manager::camera.Position());
	if (dis > 6000.0f * 6000.0f) return (renderCells);

	for (SettlementChunk &chunk : chunks)
	{
		worldSpace = position + chunk.localPosition - Terrain::terrainOffset;

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