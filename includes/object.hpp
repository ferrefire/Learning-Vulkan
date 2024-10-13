#pragma once

#include "pipeline.hpp"
#include "mesh.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

class Object
{
    private:
        glm::vec3 position = glm::vec3(0);
        glm::vec3 rotation = glm::vec3(0);
        glm::vec3 scale = glm::vec3(1);

        glm::mat4 translation = glm::mat4(1);

    public:
        Mesh *mesh = nullptr;
        Pipeline *pipeline = nullptr;

        Object();
        Object(Mesh *mesh, Pipeline *pipeline);
        ~Object();

        void Move(glm::vec3 amount);
        void Rotate(glm::vec3 amount);
        void Resize(glm::vec3 amount);

        glm::mat4 Translation();
};
