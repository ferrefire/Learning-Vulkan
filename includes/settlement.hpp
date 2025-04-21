#pragma once

#include "buildings.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <cstdint>

#define CHUNK_RADIUS 2
#define CHUNK_LENGTH (CHUNK_RADIUS * 2 + 1)
#define CELL_SIZE (8.0f * 3.0f)

enum class ChunkDirection { North, Eeast, South, West };

struct SettlementCell
{
    int settlementID;
    int chunkID;
	int seed;
	glm::ivec2 index;
	glm::ivec2 coordinates;
	glm::vec3 localPosition;
    Building *building;

	glm::vec3 GetGlobalPosition();
};

struct SettlementChunk
{
    bool initialized = false;
    int settlementID;
	int id;
    glm::ivec2 coordinates;
    glm::vec3 localPosition;
    SettlementCell cells[CHUNK_LENGTH][CHUNK_LENGTH];

    void Setup(int settlementID, int id, glm::ivec2 coordinates);

    SettlementCell *GetCell(int x, int y);
    SettlementCell *GetCell(glm::vec3 target, bool local = false);

	glm::vec3 GetGlobalPosition();
};

struct ProximityData
{
	int id = -1;
	float distanceSquared = FLT_MAX;
};

class Settlement
{
    private:
        std::vector<Building *> buildingsToGenerate;
        void GenerateBuildings();

    public:
        Settlement();
        ~Settlement();

        int id;
        glm::vec3 position;
        std::vector<SettlementChunk> chunks;
		bool generating;

        void Start(int id, glm::vec3 position);
        void Destroy();

		void AddChunk(glm::ivec2 coordinates);

		void FillChunk(glm::ivec2 coordinates);

		SettlementChunk *GetChunk(int x, int y);
		SettlementChunk *GetChunk(glm::vec3 target, bool local = false);
		int GetChunkID(glm::vec3 target, bool local = false);
		SettlementCell *GetCell(glm::vec3 target, bool local = false);

		void GenerateBuilding(SettlementCell *cell);
		void GenerateBuilding(glm::vec3 target, bool local = false);

		std::vector<ProximityData> GetChunkProximity(glm::vec3 target);

		std::vector<SettlementCell *> GetRenderCells();

        int MaxCoordinate();
};