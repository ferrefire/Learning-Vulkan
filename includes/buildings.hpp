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

enum D {N = 0, S = 2, E = 1, W = 3};
enum class FloorType {empty, flat, stairs, beams};
enum class WallType {empty, flat, window, door, balcony, beams};
enum class RoofType {empty, flat, flatUp, slope, slopeLow, cone};
enum class PartType {floor, foundation, flatWall, windowedWall, dooredWall, slopedWall, slopedLowWall, flatRoof, flatLowRoof, slopedRoof, slopedLowRoof, coneRoof, coneLowRoof, coneRoofExtension, beam, slopedBeam, slightSlopedBeam, collumn};

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
	//WallType types[4] = {WallType::empty, WallType::empty, WallType::empty, WallType::empty};
	//int variants[4] = {0, 0, 0, 0};

    WallType N_Type = WallType::empty;
    WallType S_Type = WallType::empty;
    WallType E_Type = WallType::empty;
    WallType W_Type = WallType::empty;

	int N_Variant = 0;
    int S_Variant = 0;
    int E_Variant = 0;
    int W_Variant = 0;

    bool Empty(bool countBeams = true)
    {
        if (countBeams)
        {
            return (N_Type == WallType::empty && S_Type == WallType::empty && E_Type == WallType::empty && W_Type == WallType::empty);
        }
        else
        {
            return ((N_Type == WallType::empty || N_Type == WallType::beams) && (S_Type == WallType::empty || S_Type == WallType::beams) && 
                (E_Type == WallType::empty || E_Type == WallType::beams) && (W_Type == WallType::empty || W_Type == WallType::beams));
        }
    }

    bool Beams()
    {
        return (N_Type == WallType::beams || S_Type == WallType::beams || E_Type == WallType::beams || W_Type == WallType::beams);
    }
};

struct Roof
{
    RoofType type = RoofType::empty;
    D direction;

	bool N_Extend = false;
	bool S_Extend = false;
	bool E_Extend = false;
	bool W_Extend = false;

	bool N_Merge = false;
	bool S_Merge = false;
	bool E_Merge = false;
	bool W_Merge = false;

	bool Empty()
    {
        return (type == RoofType::empty);
    }

	bool Extend()
	{
		return (N_Extend || S_Extend || E_Extend || W_Extend);
	}

	bool Extend(D direction)
	{
		if (direction == N) return (N_Extend);
		else if (direction == S) return (S_Extend);
		else if (direction == E) return (E_Extend);
		else if (direction == W) return (W_Extend);
		else return (false);
	}

	bool LocalExtend(D direction)
	{
		return (Extend((D)((this->direction + direction) % 4)));
	}

	bool Merge(D direction)
	{
		if (direction == N) return (N_Merge);
		else if (direction == S) return (S_Merge);
		else if (direction == E) return (E_Merge);
		else if (direction == W) return (W_Merge);
		else return (false);
	}

	bool LocalMerge(D direction)
	{
		return (Merge((D)((this->direction + direction) % 4)));
	}

	int MergeCount(D direction)
	{
		if (direction == N) return ((N_Merge ? 1 : 0) + (S_Merge ? 1 : 0));
		else if (direction == E) return ((E_Merge ? 1 : 0) + (W_Merge ? 1 : 0));
		return (0);
	}
};

struct BuildingCell
{
    Floor floor;
    Walls walls;
    Roof roof;

    bool Empty(bool countBeams = true)
    {
        return (floor.Empty() && walls.Empty(countBeams) && roof.Empty());
    }
};

struct BuildingBuffer
{
	glm::mat4 translation = glm::mat4(1.0f);
	glm::mat4 orientation = glm::mat4(1.0f);
};

struct Building
{
	int id = -1;
	bool active = false;
	bool lod = true;

	Mesh mesh;
	Mesh lodMesh;

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::mat4 translation = glm::mat4(1.0f);
	glm::mat4 orientation = glm::mat4(1.0f);

	std::vector<std::vector<std::vector<BuildingCell>>> cells;
    glm::ivec3 size = glm::ivec3(0);

	void Update()
	{
		translation = glm::mat4(1.0f);
		translation = glm::translate(translation, position);

		orientation = glm::mat4(1.0f);
		orientation = glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		orientation = glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	}
};

struct GenerationConfig
{
    int seed = 1;
    glm::ivec3 minSize = glm::ivec3(2, 2, 2);
    glm::ivec3 maxSize = glm::ivec3(3, 3, 3);
	glm::ivec3 currentSize = glm::ivec3(0);
	int expansionFactor = 3;
    int levelFactor = 3;
	int decoratedFactor = 0;
	int scaffoldingReduction = 2;
	int balconyFactor = 5;
	bool random = false;
	float scale = 7.5f;
	bool lod = false;
};

struct PartConfig
{
	std::string name = "part";
	glm::vec3 scale = glm::vec3(1);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 offset = glm::vec3(0);
};

struct BuildingVariables
{
	glm::vec4 roofTint = glm::vec4(98.0f, 122.0f, 181.0f, 255.0f) / 255.0f;
	glm::vec4 wallTint = glm::vec4(255.0f, 255.0f, 255.0f, 255.0f) / 255.0f;
	glm::vec4 beamTint = glm::vec4(213.0f, 234.0f, 236.0f, 255.0f) / 255.0f;
	glm::vec4 brickTint = glm::vec4(255.0f, 255.0f, 255.0f, 255.0f) / 255.0f;
	alignas(8) glm::vec2 roofDistance = glm::vec2(150.0f, 100.0f);
	glm::vec2 wallDistance = glm::vec2(50.0f, 100.0f);
	glm::vec2 beamDistance = glm::vec2(50.0f, 50.0f);
	glm::vec2 brickDistance = glm::vec2(100.0f, 100.0f);
	float roofNormal = 4.0f;
	float wallNormal = 2.0f;
	float beamNormal = 1.0f;
	float brickNormal = 1.0f;
	float roofAmbient = 4.0f;
	float wallAmbient = 6.0f;
	float beamAmbient = 4.0f;
	float brickAmbient = 2.0f;
	float roofAmbientDefault = 0.6f;
	float wallAmbientDefault = 0.75f;
	float beamAmbientDefault = 1.0f;
	float brickAmbientDefault = 1.0f;
};

class Buildings
{
    private:
		static Building *building;

    public:
		static std::vector<Texture> beamTextures;
		static std::vector<Texture> plasteredTextures;
		static std::vector<Texture> reedTextures;
		static std::vector<Texture> brickTextures;

		static Pipeline graphicsPipeline;
		static Pipeline shadowPipeline;

		static std::vector<BuildingBuffer> buildingBuffers;
		static std::vector<BuildingBuffer> buildingShadowBuffers;
		static std::vector<Buffer> uniformBuffers;
		static std::vector<Buffer> uniformShadowBuffers;
		static Buffer variablesBuffer;

		static Descriptor graphicsDescriptor;
        static Descriptor shadowDescriptor;

		static BuildingVariables buildingVariables;

		static bool generating;

        static GenerationConfig generationConfig;

		static std::vector<Building *> buildings;
		static std::vector<Building *> renderBuildings;
		static std::vector<Building *> renderBuildingsShadow;

		static Random random;

		static PartConfig floorConfig;
		static PartConfig foundationConfig;
		static PartConfig flatWallConfig;
		static PartConfig windowedWallConfig;
		static PartConfig dooredWallConfig;
		static PartConfig slopedWallConfig;
		static PartConfig slopedLowWallConfig;
		static PartConfig flatRoofConfig;
		static PartConfig flatLowRoofConfig;
		static PartConfig slopedRoofConfig;
		static PartConfig slopedLowRoofConfig;
		static PartConfig coneRoofConfig;
		static PartConfig coneLowRoofConfig;
		static PartConfig coneRoofExtensionConfig;
		static PartConfig beamConfig;
		static PartConfig slopedBeamConfig;
		static PartConfig slightSlopedBeamConfig;
		static PartConfig collumnConfig;

		static int maxRenderBuildings;
		static int currentActiveBuildings;

		static int partCount;

		static void Create();
        static void CreateMeshes();
        static void CreateTextures();
		static void CreatePipelines();
		static void CreateBuffers();
		static void CreateDescriptors();

        static void Destroy();
        static void DestroyMeshes();
        static void DestroyTextures();
		static void DestroyPipelines();
		static void DestroyBuffers();
		static void DestroyDescriptors();
		static void DestroyBuildings();

		static void Start();
		static void Frame();

		static Building *CreateBuilding(int seed = 1);
		static void DestroyBuilding(int id);

		static bool CreateBuildingMesh(Building *targetBuilding, bool lod);

		static void RecordGraphicsCommands(VkCommandBuffer commandBuffer);
        static void RenderBuildings(VkCommandBuffer commandBuffer);

        static void RecordShadowCommands(VkCommandBuffer commandBuffer, int cascade);
        static void RenderShadows(VkCommandBuffer commandBuffer, int cascade);

		static void SetRenderBuildings();
		static void SetRenderBuildingsShadow();

		static void GenerateBuilding();
		//static void GenerateCells();
		//static void ExpandLevel(int level);
		//static void FillLevel(int level);
		//static void ExpandCell(int i, int x, int y, int factor);
        //static bool ExpansionValid(int i, int x, int y, int factor, int increase);
		//static void SetWalls();
		//static void SetBeams();
		//static void SetRoof();
        //static bool BeamPass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool NE_Empty, bool NW_Empty, bool SE_Empty, bool SW_Empty);
		//static void RoofTypePass(int i, int x, int y);
		//static void RoofConePass(int i, int x, int y);
		//static void RoofDirectionPass(int i, int x, int y);
		//static void RoofMergePass(int i, int x, int y);
		//static void RoofExtendPass(int i, int x, int y);
		//static D GetRoofDirection(RoofType type, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone,
		//	bool S_Cone, bool E_Cone, bool W_Cone, bool E_UpEmpty, bool W_UpEmpty);
		//static bool MergePass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone, bool S_Cone, bool E_Cone, bool W_Cone);
		//static Shape GeneratePart(PartType type, int rotate = 0);
		//static Shape GeneratePart(PartType type, glm::vec3 scale, glm::vec3 offset, glm::vec3 rotation);
		//static void GenerateMesh();
		//static void GenerateFloors(int level);
		//static void GenerateFloor(int i, int x, int y, FloorType type);
		//static void GenerateWalls(int level);
		//static void GenerateWall(glm::vec3 offset, WallType type, D direction, int variant);
		//static void GenerateBeams(int i, int x, int y);
		//static void GenerateRoofs(int level);
		//static void GenerateRoof(int i, int x, int y, RoofType type, D direction);
		//static bool IsRoof(int i, int x, int y);
		//static bool CellValid(int i, int x, int y);
		//static bool CellEmpty(int i, int x, int y, bool countBeams = true);
		//static bool FloorEmpty(int i, int x, int y);
		static void UpdateBuilding(int i);
		static bool BuildingInView(Building *b);
		static void UpdateBuildingVariables();
};