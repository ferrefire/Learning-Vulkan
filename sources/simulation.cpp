#include "simulation.hpp"

#include "data.hpp"
#include "input.hpp"
#include "time.hpp"
#include "manager.hpp"
#include "terrain.hpp"
#include "utilities.hpp"

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

	//Settlement *newSettlement2 = new Settlement;
    //settlements.push_back(newSettlement2);
	//newSettlement2->Start(settlements.size() - 1, glm::vec3(8125.0f, 0.0f, 5075.0f));

	//Settlement *newSettlement3 = new Settlement;
    //settlements.push_back(newSettlement3);
	//newSettlement3->Start(settlements.size() - 1, glm::vec3(5650.0f, 0.0f, 3030.0f));
}

void Simulation::Frame()
{
	if (!started) return;

	if (Time::newFrameTick)
	{
		generating = true;

		//for (Settlement *settlement : settlements)
		//{
		//	settlement->generating = true;
		//}
	}

	int chunkRange = 1;

	static int chunkCountX = -chunkRange;
	static int chunkCountY = -chunkRange;

	if (Time::newSubTick)
	{
		if (chunkCountX <= chunkRange)
		{
			if (chunkCountY <= chunkRange)
			{
				for (int i = 0; i < settlements.size(); i++)
				{
					settlements[i]->AddChunk(glm::ivec2(chunkCountX, chunkCountY));
					settlements[i]->FillChunk(glm::ivec2(chunkCountX, chunkCountY));
				}
				chunkCountY++;
			}

			if (chunkCountY > chunkRange)
			{
				chunkCountY = -chunkRange;
				chunkCountX++;
			}
		}
	}

	//if (Input::GetKey(GLFW_KEY_I).pressed)
	//{
	//	settlements[0]->chunks[0].GenerateChunkMesh();
	//}
}

std::vector<ProximityData> Simulation::GetSettlementProximity(glm::vec3 target)
{
	std::vector<ProximityData> proximityResults;

	target += Terrain::terrainOffset;

	for (Settlement *settlement : settlements)
	{
		ProximityData settlementProximity;
		settlementProximity.id = settlement->id;
		settlementProximity.distanceSquared = Utilities::DistanceSqrd(settlement->position, target);

		int l = 0;
		for (int k = 0; k < proximityResults.size(); k++)
		{
			if (settlementProximity.distanceSquared < proximityResults[k].distanceSquared) break;
			l++;
		}

		if (proximityResults.size() == 0)
		{
			proximityResults.resize(1);
			proximityResults[0] = settlementProximity;
		}
		else
		{
			proximityResults.insert(proximityResults.begin() + l, settlementProximity);
		}
	}

	return (proximityResults);
}

bool Simulation::started = false;
bool Simulation::generating = false;
std::vector<Settlement *> Simulation::settlements;