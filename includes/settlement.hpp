#pragma once

#include "buildings.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define CHUNK_RADIUS 2
#define CHUNK_LENGTH (CHUNK_RADIUS * 2 + 1)
#define CELL_SIZE (8.0f * 3.0f)

struct SettlementCell
{
    int settlementID;
    glm::ivec2 chunkIndex;
    glm::ivec2 index;
    glm::vec3 localPosition;
    Building *building;
};

struct SettlementChunk
{
    bool initialized = false;
    int settlementID;
    glm::ivec2 index;
    glm::vec3 localPosition;
    SettlementCell cells[CHUNK_LENGTH][CHUNK_LENGTH];

    void Setup(int x, int y, int id);

    SettlementCell *GetCell(int x, int y);
    SettlementCell *GetCell(glm::vec3 target, bool local = false);
};

class Settlement
{
    private:

    public:
        Settlement();
        ~Settlement();

        int id;
        glm::vec3 position;
        int radius;
        int length;
        std::vector<SettlementChunk> chunks;

        void Start(int id);
        void Destroy();

        void IncreaseRadius(int amount);

        SettlementChunk *GetChunk(int x, int y);
        SettlementChunk *GetChunk(glm::vec3 target, bool local = false);
        SettlementCell *GetCell(glm::vec3 target, bool local = false);

        //void AddRadius();

        void GenerateBuilding(SettlementCell *cell);

        std::vector<SettlementCell *> GetRenderCells();
};