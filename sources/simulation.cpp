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
    newSettlement->position = glm::vec3(3250.0f, 0.0f, 4000.0f);
    Data::RequestData(newSettlement->position, &newSettlement->position.y);
    newSettlement->Start(settlements.size() - 1);
    //newSettlement->GenerateBuilding(newSettlement->GetCell(glm::vec3(0.0f), true));
    //newSettlement->GenerateBuilding(newSettlement->GetCell(glm::vec3(CELL_SIZE, 0.0f, 0.0f), true));
    //newSettlement->GenerateBuilding(newSettlement->GetCell(glm::vec3(-CELL_SIZE, 0.0f, 0.0f), true));
    //newSettlement->IncreaseRadius(1);

    //SettlementCell *cell = newSettlement->GetCell(glm::vec3(CELL_SIZE * 5.0f, 0.0f, 0.0f), true);
    //if (!cell)
    //    std::cout << "no cell" << std::endl;
    //else 
    //    std::cout << "cell index: " << cell->index.x << " " << cell->index.y << std::endl;
}

void Simulation::Frame()
{
    if (Input::GetKey(GLFW_KEY_I).pressed)
    {
        settlements[0]->IncreaseRadius(1);
        //settlements[0]->GenerateBuilding(settlements[0]->GetCell(glm::vec3(CELL_SIZE * (settlements[0]->radius), 0.0f, 0.0f), true));
    }
}

bool Simulation::started = false;
std::vector<Settlement *> Simulation::settlements;