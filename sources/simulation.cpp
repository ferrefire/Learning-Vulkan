#include "simulation.hpp"

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
    Settlement *newSettlement = new Settlement;
    newSettlement->position = glm::vec3(0.0f, 2500.0f, 0.0f);
    newSettlement->GenerateBuilding(newSettlement->GetCell(glm::vec3(0.0f), true));
    settlements.push_back(newSettlement);
}

std::vector<Settlement *> Simulation::settlements;