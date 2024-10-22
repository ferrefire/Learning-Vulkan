#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define QUAD 1
#define CUBE 2
#define PLANE 3

#define INDEX_TYPE VK_INDEX_TYPE_UINT16

typedef uint16_t indexType;

class Shape
{
    private:
        

    public:
        Shape();
        Shape(int type);
        ~Shape();

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> coordinates;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> colors;
        std::vector<indexType> indices;

		bool positionsOnly = false;

        void SetShape(int type, int resolution = 1);

        void AddPosition(glm::vec3 pos);
		void AddCoordinate(glm::vec2 uv);
		void AddIndice(indexType index);

		void Join(Shape &joinShape);

        void Move(glm::vec3 movement);
        void Rotate(float degrees, glm::vec3 axis);
};
