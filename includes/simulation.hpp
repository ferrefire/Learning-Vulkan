#pragma once

#include "settlement.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Simulation
{
    private:

    public:
        static bool started;

        static std::vector<Settlement *> settlements;

        static void Create();

        static void Destroy();
        static void DestroySettlements();

        static void Start();
        static void Frame();
};