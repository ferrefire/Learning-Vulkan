#pragma once

#include "mesh.hpp"
#include "buffer.hpp"
#include "pipeline.hpp"
#include "descriptor.hpp"
#include "random.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum D {N, S, E, W};
enum class FloorType {empty, flat, stairs, beams};
enum class WallType {empty, flat, window, door, balcony, beams};
enum class RoofType {empty, flat, flatUp, slope, cone};

struct Floor
{
    FloorType type = FloorType::empty;

    bool Empty()
    {
        return (type == FloorType::empty);
    }
};

struct Walls
{
    WallType N_Type = WallType::empty;
    WallType S_Type = WallType::empty;
    WallType E_Type = WallType::empty;
    WallType W_Type = WallType::empty;

    bool Empty()
    {
        return (N_Type == WallType::empty && S_Type == WallType::empty && E_Type == WallType::empty && W_Type == WallType::empty);
    }
};

struct Roof
{
    RoofType type = RoofType::empty;
    D direction;

    bool Empty()
    {
        return (type == RoofType::empty);
    }
};

struct BuildingCell
{
    Floor floor;
    Walls walls;
    Roof roof;

    bool Empty()
    {
        return (floor.Empty() && walls.Empty() && roof.Empty());
    }
};

struct Building
{
	Mesh mesh;
    std::vector<std::vector<std::vector<BuildingCell>>> cells;
    glm::ivec3 size = glm::ivec3(0);
};

struct GenerationConfig
{
    int seed = 1;
    glm::ivec3 minSize = glm::ivec3(2, 1, 2);
    glm::ivec3 maxSize = glm::ivec3(3, 3, 3);
    int expansionFactor = 5;
    int levelFactor = 3;
	int scaffoldingReduction = 1;
	bool random = false;
};

class Buildings
{
    private:

    public:
        static Mesh mesh;

        static std::vector<Texture> beamTextures;

        static Pipeline graphicsPipeline;

        static Descriptor graphicsDescriptor;

        static GenerationConfig generationConfig;

		static Building building;

		static Random random;

        static void Create();
        static void CreateMeshes();
        static void CreateTextures();
        static void CreatePipelines();
        static void CreateDescriptors();

        static void Destroy();
        static void DestroyMeshes();
        static void DestroyTextures();
        static void DestroyPipelines();
        static void DestroyDescriptors();

		static void Start();

        static void RecordGraphicsCommands(VkCommandBuffer commandBuffer);
        static void RenderBuildings(VkCommandBuffer commandBuffer);

        static void GenerateBuilding();
		static void GenerateCells();
		static void ExpandLevel(int level);
		static void FillLevel(int level);
		static void ExpandCell(int i, int x, int y, int factor);
        static bool ExpansionValid(int i, int x, int y, int factor, int increase);
		static void SetWalls();
		static void GenerateMesh();
		static void GenerateFloors(int level);
		static Shape GenerateFloor(FloorType type);
		static void GenerateWalls(int level);
		static Shape GenerateWall(WallType type, D direction);
		static bool CellValid(int i, int x, int y);
		static bool CellEmpty(int i, int x, int y);
		static bool FloorEmpty(int i, int x, int y);
};