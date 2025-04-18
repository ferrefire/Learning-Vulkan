#pragma once

#include "buildings.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define CHUNK_RADIUS 2
#define CHUNK_LENGTH CHUNK_RADIUS * 2 + 1
#define CELL_SIZE 7.5f

struct SettlementCell
{
    glm::ivec3 index;
    glm::vec3 localPosition;
    Building *building;
};

struct SettlementChunk
{
    glm::ivec3 index;
    glm::vec3 localPosition;
    SettlementCell cells[CHUNK_LENGTH][CHUNK_LENGTH];

    void Setup(int x, int y);

    SettlementCell *GetCell(int x, int y);
    SettlementCell *GetCell(glm::vec3 target, bool local = false);
};

class Settlement
{
    private:

    public:
        Settlement();
        ~Settlement();

        glm::vec3 position;
        int radius;
        int length;
        std::vector<SettlementChunk> chunks;

        SettlementChunk *GetChunk(int x, int y);
        SettlementChunk *GetChunk(glm::vec3 target, bool local = false);
        SettlementCell *GetCell(glm::vec3 target, bool local = false);

        void GenerateBuilding(SettlementCell *cell);

        std::vector<SettlementCell *> GetRenderCells();

        void Destroy();
};