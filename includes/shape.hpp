#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define NO_SHAPE 0
#define QUAD 1
#define CUBE 2
#define PLANE 3
#define BLADE 4
#define CYLINDER 5
#define LEAF 6
#define CROSS 7
#define TRIANGLE 8
#define PRISM 9

#define INDEX_TYPE VK_INDEX_TYPE_UINT16
//#define INDEX_TYPE VK_INDEX_TYPE_UINT32

typedef uint16_t indexType;
//typedef uint32_t indexType;

class Shape
{
    private:
        

    public:
        Shape();
        Shape(int type);
        Shape(int type, bool coordinate, bool normal, bool color);
        Shape(int type, int resolution);
        ~Shape();

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> coordinates;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> colors;
        std::vector<indexType> indices;

        std::vector<unsigned int> mergeTopPoints;
		std::vector<unsigned int> mergeBottomPoints;
		std::vector<unsigned int> pointBlended;
		std::vector<glm::ivec2> pointMerged;

        int centerMergePoint = -1;
        int createResolution = -1;

		bool coordinate = false;
		bool normal = false;
		bool color = false;

		void SetShape(int type, int resolution = 1);
		void SetCube(bool front, bool back, bool left, bool right, bool top, bool bottom);

        void AddPosition(glm::vec3 pos);
		void AddCoordinate(glm::vec2 uv);
		void AddNormal(glm::vec3 norm);
		void AddColor(glm::vec3 col);
		void AddIndice(indexType index);

        void Join(Shape &joinShape);
        void Merge(Shape &joinShape, int mainBlendRange, int joinBlendRange);

        void Move(glm::vec3 movement);
		void Rotate(float degrees, glm::vec3 axis, bool rotateNormal = true);
		void Rotate90(int times = 1, bool rotateNormal = true);
		void Scale(glm::vec3 scale, bool scaleUV = false);

        void RecalculateNormals(bool x = true, bool y = false, bool z = true);
        void RecalculateNormal(unsigned int index);
        void RecalculateCoordinates();

		void SetCoordinates(glm::vec2 uv);
		void SetXCoordinates(float uv);
		void SetYCoordinates(float uv);
		void ScaleCoordinates(glm::vec2 scale);
		void SwapCoordinates();
		void InverseCoordinates(bool inverseX = true, bool inverseY = true);

		void SetColors(glm::vec3 value);

		int ClosestMergeIndex(glm::vec3 position, bool closest, bool top);
        glm::vec3 BottomMergePointsCenter();
		glm::vec3 TopMergePointsCenter();
        int GetPositionIndex(int x, int y);
        glm::ivec2 GetPositionCoordinates(int i);
        void CloseUnusedPoints();
};
