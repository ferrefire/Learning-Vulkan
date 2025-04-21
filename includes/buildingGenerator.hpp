#pragma once

#include "buildings.hpp"
#include "random.hpp"
#include "shape.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>

struct NeighbourData
{
    bool N_Empty;
    bool E_Empty;
    bool S_Empty;
    bool W_Empty;
};

class BuildingGenerator
{
    private:
        GenerationConfig config;
        Random random;

        bool cellsGenerated = false;
        std::vector<std::vector<std::vector<BuildingCell>>> cells;

        bool shapeGenerated = false;
        Shape shape;

        void ExpandLevel(int level);
        void FillLevel(int level);

        void SetWalls();
        void SetBeams();
        void SetRoof();

        void ExpandCell(int i, int x, int y, int factor);
        bool ExpansionValid(int i, int x, int y, int factor, int increase);

        bool CellValid(int i, int x, int y);
		bool CellEmpty(int i, int x, int y, bool countBeams = true);
		bool FloorEmpty(int i, int x, int y);
        bool IsRoof(int i, int x, int y);

        bool BeamPass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool NE_Empty, bool NW_Empty, bool SE_Empty, bool SW_Empty);
        void RoofTypePass(int i, int x, int y);
		void RoofConePass(int i, int x, int y);
		void RoofDirectionPass(int i, int x, int y);
		void RoofMergePass(int i, int x, int y);
		void RoofExtendPass(int i, int x, int y);
		bool MergePass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone, bool S_Cone, bool E_Cone, bool W_Cone);

        D GetRoofDirection(RoofType type, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone, bool S_Cone, bool E_Cone, bool W_Cone, bool E_UpEmpty, bool W_UpEmpty);

        //NeighbourData GetNeighbourData(int i, int x, int y, bool countBeams = true, void(*func)(int, int, int, bool) = nullptr);

        Shape GeneratePart(PartType type, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 offset = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f));
		void GenerateFloors(int level);
		void GenerateFloor(int i, int x, int y, FloorType type);
		void GenerateWalls(int level);
		void GenerateWall(glm::vec3 offset, WallType type, D direction, int variant);
		void GenerateBeams(int i, int x, int y);
		void GenerateRoofs(int level);
		void GenerateRoof(int i, int x, int y, RoofType type, D direction);
        
    public:
        BuildingGenerator();
        BuildingGenerator(GenerationConfig config);
        BuildingGenerator(GenerationConfig config, std::vector<std::vector<std::vector<BuildingCell>>> cells);
        ~BuildingGenerator();

        void SetConfig(GenerationConfig config);
        void SetCells(std::vector<std::vector<std::vector<BuildingCell>>> cells);

        void GenerateCells();
        std::vector<std::vector<std::vector<BuildingCell>>> GetCells();

        void GenerateShape();
        Shape GetShape();
};