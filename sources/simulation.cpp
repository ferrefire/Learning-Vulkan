#include "simulation.hpp"

#include "data.hpp"
#include "input.hpp"

#include <iostream>

void Simulation::Create()
{

}

void Simulation::Destroy()
{
    DestroySettlements();
}

void Simulation::DestroySettlements()
{
    for (int i = 0; i < settlements.size(); i++)
    {
        if (!settlements[i]) continue;

        settlements[i]->Destroy();
        delete(settlements[i]);
    }
    settlements.clear();
}

void Simulation::Start()
{
    started = true;

    Settlement *newSettlement = new Settlement;
    settlements.push_back(newSettlement);
	newSettlement->Start(settlements.size() - 1, glm::vec3(3250.0f, 0.0f, 4000.0f));
	newSettlement->AddChunk(glm::ivec2(0, 0));
	newSettlement->GenerateBuilding(newSettlement->GetCell(glm::vec3(0.0f, 0.0f, 0.0f), true));
	newSettlement->GenerateBuilding(newSettlement->GetCell(glm::vec3(CELL_SIZE * 2.0f, 0.0f, 0.0f), true));

	//newSettlement->GetCell(glm::vec3(CELL_SIZE * 3.0f, 0.0f, 0.0f), true);
}

void Simulation::Frame()
{
    if (Input::GetKey(GLFW_KEY_I).pressed)
    {
        settlements[0]->AddChunk(glm::ivec2(1, 0));
		settlements[0]->GenerateBuilding(settlements[0]->GetCell(glm::vec3(CELL_SIZE * 4.0f, 0.0f, CELL_SIZE), true));
	}
}

bool Simulation::started = false;
std::vector<Settlement *> Simulation::settlements;