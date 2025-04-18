#pragma once

#include "settlement.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Simulation
{
    private:

    public:
        static std::vector<Settlement *> settlements;

        static void Create();

        static void Destroy();
        static void DestroySettlements();

        static void Start();
};