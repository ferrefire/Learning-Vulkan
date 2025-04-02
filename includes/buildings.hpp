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
	float scale = 5.0f;
};

struct PartConfig
{
	std::string name = "part";
	glm::vec3 scale = glm::vec3(1);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 offset = glm::vec3(0);
};

class Buildings
{
    private:

    public:
        static Mesh mesh;

		static std::vector<Texture> beamTextures;
		static std::vector<Texture> plasteredTextures;
		static std::vector<Texture> reedTextures;

		static Pipeline graphicsPipeline;

        static Descriptor graphicsDescriptor;

        static GenerationConfig generationConfig;

		static Building building;

		static Random random;

		static PartConfig floorConfig;
		static PartConfig flatWallConfig;
		static PartConfig flatRoofConfig;
		static PartConfig slopedRoofConfig;

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
		static void SetRoof();
		static void RoofTypePass(int i, int x, int y);
		static void RoofDirectionPass(int i, int x, int y);
		static D GetRoofDirection(RoofType type, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone,
			bool S_Cone, bool E_Cone, bool W_Cone, bool E_UpEmpty, bool W_UpEmpty);
		static bool IsRoof(int i, int x, int y);
		static void GenerateMesh();
		static void GenerateFloors(int level);
		static void GenerateFloor(glm::vec3 offset, FloorType type);
		static void GenerateWalls(int level);
		static void GenerateWall(glm::vec3 offset, WallType type, D direction);
		static void GenerateRoofs(int level);
		static void GenerateRoof(glm::vec3 offset, RoofType type, D direction);
		static bool CellValid(int i, int x, int y);
		static bool CellEmpty(int i, int x, int y);
		static bool FloorEmpty(int i, int x, int y);
};