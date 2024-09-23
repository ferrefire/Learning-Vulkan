#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define QUAD 1
#define CUBE 2

class Shape
{
    private:
        

    public:
        Shape();
        Shape(int type);
        ~Shape();

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> coordinates;
        std::vector<uint16_t> indices;

        void SetShape(int type);

        void AddPosition(glm::vec3 pos);
		void AddCoordinate(glm::vec2 uv);
		void AddIndice(uint16_t index);

        void Join(Shape &joinShape);

        void Move(glm::vec3 movement);
        void Rotate(float degrees, glm::vec3 axis);
};
