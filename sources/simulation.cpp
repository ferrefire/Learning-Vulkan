#include "simulation.hpp"

#include "data.hpp"
#include "input.hpp"
#include "time.hpp"
#include "manager.hpp"

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
}

void Simulation::Frame()
{
	//if (Time::newTick)
	if (Time::newFrameTick)
	{
		for (Settlement *settlement : settlements)
		{
			settlement->generating = false;
		}
	}

	static int chunkCountX = 0;
	static int chunkCountY = 0;

	int chunkRange = 4;

	if (Time::newSubTick && settlements.size() > 0)
	{
		if (chunkCountX < chunkRange)
		{
			if (chunkCountY < chunkRange)
			{
				settlements[0]->AddChunk(glm::ivec2(chunkCountX, chunkCountY));
				settlements[0]->FillChunk(glm::ivec2(chunkCountX, chunkCountY));
				chunkCountY++;
			}

			if (chunkCountY >= chunkRange)
			{
				chunkCountY = 0;
				chunkCountX++;
			}
		}
	}

	if (Input::GetKey(GLFW_KEY_I).pressed)
	{
		
	}
}

bool Simulation::started = false;
std::vector<Settlement *> Simulation::settlements;