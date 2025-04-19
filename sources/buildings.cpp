#include "buildings.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "ui.hpp"
#include "time.hpp"
#include "data.hpp"
#include "terrain.hpp"
#include "simulation.hpp"
#include "settlement.hpp"

#include <iostream>
#include <cstdlib>

void Buildings::Create()
{
    CreateMeshes();
    CreateTextures();
    CreatePipelines();
	CreateBuffers();
    CreateDescriptors();
}

void Buildings::CreateMeshes()
{
	//generationConfig.seed = 1472;
	//generationConfig.seed = 15049;
	//generationConfig.seed = 1347;
	//generationConfig.seed = 17301;
	//generationConfig.seed = 1845;
	//generationConfig.scaffoldingReduction = 1;
	//generationConfig.minSize = glm::ivec3(3);
	//generationConfig.maxSize = glm::ivec3(6);

	//generationConfig.seed = 657970;
	/*generationConfig.minSize = glm::ivec3(2);
	generationConfig.maxSize = glm::ivec3(2, 3, 2);
	generationConfig.random = true;

	int index = 0;
	int range = 1;

	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			building = &buildings[index];
			GenerateBuilding();
			building->position += glm::vec3(15.0f * x, 0.0f, 15.0f * y);
			//building->position += glm::vec3(3250.0f, 0.0f, 4000.0f);
			building->Update();
			Data::RequestData(buildings[index].position, &buildings[index].position[1], UpdateBuilding, index);
			index++;
		}
	}*/

	//building = &buildings[0];
	//GenerateBuilding();
	//building->rotation += glm::vec3(45.0f, 0.0f, 0.0f);
	//building->Update();

	/*building = &buildings[1];
	GenerateBuilding();
	building->position += glm::vec3(15.0f, 0.0f, 0.0f);
	building->rotation = glm::vec3(0.0f, 90.0f * random.Next(0, 4), 0.0f);
	building->Update();

	building = &buildings[2];
	GenerateBuilding();
	building->position += glm::vec3(-15.0f, 0.0f, 0.0f);
	building->rotation = glm::vec3(0.0f, 90.0f * random.Next(0, 4), 0.0f);
	building->Update();

	building = &buildings[3];
	GenerateBuilding();
	building->position += glm::vec3(0.0f, 0.0f, 15.0f);
	building->rotation = glm::vec3(0.0f, 90.0f * random.Next(0, 4), 0.0f);
	building->Update();

	building = &buildings[4];
	GenerateBuilding();
	building->position += glm::vec3(15.0f, 0.0f, 15.0f);
	building->rotation = glm::vec3(0.0f, 90.0f * random.Next(0, 4), 0.0f);
	building->Update();

	building = &buildings[5];
	GenerateBuilding();
	building->position += glm::vec3(-15.0f, 0.0f, 15.0f);
	building->rotation = glm::vec3(0.0f, 90.0f * random.Next(0, 4), 0.0f);
	building->Update();

	building = &buildings[6];
	GenerateBuilding();
	building->position += glm::vec3(0.0f, 0.0f, -15.0f);
	building->rotation = glm::vec3(0.0f, 90.0f * random.Next(0, 4), 0.0f);
	building->Update();

	building = &buildings[7];
	GenerateBuilding();
	building->position += glm::vec3(15.0f, 0.0f, -15.0f);
	building->rotation = glm::vec3(0.0f, 90.0f * random.Next(0, 4), 0.0f);
	building->Update();

	building = &buildings[8];
	GenerateBuilding();
	building->position += glm::vec3(-15.0f, 0.0f, -15.0f);
	building->rotation = glm::vec3(0.0f, 90.0f * random.Next(0, 4), 0.0f);
	building->Update();*/

	//building->mesh.coordinate = true;
	//building.mesh.normal = true;
	//building.mesh.color = true;
	//building.mesh.shape.coordinate = true;
	//building.mesh.shape.normal = true;
	//building.mesh.shape.color = true;
	//building.mesh.shape.SetShape(PRISM);
	//building.mesh.RecalculateVertices();
	//building.mesh.Create();
}

void Buildings::CreateTextures()
{
    SamplerConfiguration beamSamplerConfig;
	beamSamplerConfig.repeatMode = REPEAT;
	beamSamplerConfig.anisotrophic = VK_FALSE;
	beamSamplerConfig.anisotrophicSampleCount = 0;

    beamTextures.resize(3);
	beamTextures[0].CreateTexture("beam_diff.jpg", beamSamplerConfig);
	beamSamplerConfig.anisotrophic = VK_TRUE;
	beamSamplerConfig.anisotrophicSampleCount = 2;
	beamTextures[1].CreateTexture("beam_norm.jpg", beamSamplerConfig);
	beamSamplerConfig.anisotrophic = VK_TRUE;
	beamSamplerConfig.anisotrophicSampleCount = 2;
	beamTextures[2].CreateTexture("beam_ao.jpg", beamSamplerConfig);

	SamplerConfiguration plasteredSamplerConfig;
	plasteredSamplerConfig.repeatMode = REPEAT;
	plasteredSamplerConfig.anisotrophic = VK_FALSE;
	plasteredSamplerConfig.anisotrophicSampleCount = 0;

	plasteredTextures.resize(3);
	plasteredTextures[0].CreateTexture("plastered_diff.jpg", plasteredSamplerConfig);
	plasteredSamplerConfig.anisotrophic = VK_TRUE;
	plasteredSamplerConfig.anisotrophicSampleCount = 2;
	plasteredTextures[1].CreateTexture("plastered_norm.jpg", plasteredSamplerConfig);
	plasteredSamplerConfig.anisotrophic = VK_TRUE;
	plasteredSamplerConfig.anisotrophicSampleCount = 2;
	plasteredTextures[2].CreateTexture("plastered_ao.jpg", plasteredSamplerConfig);

	SamplerConfiguration reedSamplerConfig;
	reedSamplerConfig.repeatMode = REPEAT;
	reedSamplerConfig.anisotrophic = VK_FALSE;
	reedSamplerConfig.anisotrophicSampleCount = 0;

	reedTextures.resize(3);
	reedTextures[0].CreateTexture("reed_diff.jpg", reedSamplerConfig);
	reedSamplerConfig.anisotrophic = VK_TRUE;
	reedSamplerConfig.anisotrophicSampleCount = 4;
	reedTextures[1].CreateTexture("reed_norm.jpg", reedSamplerConfig);
	reedSamplerConfig.anisotrophic = VK_TRUE;
	reedSamplerConfig.anisotrophicSampleCount = 4;
	reedTextures[2].CreateTexture("reed_ao.jpg", reedSamplerConfig);

	SamplerConfiguration brickSamplerConfig;
	brickSamplerConfig.repeatMode = REPEAT;
	brickSamplerConfig.anisotrophic = VK_FALSE;
	brickSamplerConfig.anisotrophicSampleCount = 0;

	brickTextures.resize(3);
	brickTextures[0].CreateTexture("brick_diff.jpg", brickSamplerConfig);
	brickSamplerConfig.anisotrophic = VK_TRUE;
	brickSamplerConfig.anisotrophicSampleCount = 4;
	brickTextures[1].CreateTexture("brick_norm.jpg", brickSamplerConfig);
	brickSamplerConfig.anisotrophic = VK_TRUE;
	brickSamplerConfig.anisotrophicSampleCount = 4;
	brickTextures[2].CreateTexture("brick_ao.jpg", brickSamplerConfig);
}

void Buildings::CreatePipelines()
{
    std::vector<DescriptorLayoutConfiguration> graphicsDescriptorLayoutConfig(5);
	int i = 0;
	graphicsDescriptorLayoutConfig[i].type = UNIFORM_BUFFER;
	graphicsDescriptorLayoutConfig[i++].stages = VERTEX_STAGE;
	graphicsDescriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	graphicsDescriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	graphicsDescriptorLayoutConfig[i++].count = beamTextures.size();
	graphicsDescriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	graphicsDescriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	graphicsDescriptorLayoutConfig[i++].count = plasteredTextures.size();
	graphicsDescriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	graphicsDescriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	graphicsDescriptorLayoutConfig[i++].count = reedTextures.size();
	graphicsDescriptorLayoutConfig[i].type = IMAGE_SAMPLER;
	graphicsDescriptorLayoutConfig[i].stages = FRAGMENT_STAGE;
	graphicsDescriptorLayoutConfig[i++].count = brickTextures.size();
	PipelineConfiguration graphicsPipelineConfiguration = Pipeline::DefaultConfiguration();
	//VertexInfo vertexInfo = building->mesh.MeshVertexInfo();
	VertexInfo vertexInfo = Mesh::GetVertexInfo(true, true, true, true);
	graphicsPipeline.CreateGraphicsPipeline("building", graphicsDescriptorLayoutConfig, graphicsPipelineConfiguration, vertexInfo);

	std::vector<DescriptorLayoutConfiguration> shadowDescriptorLayoutConfig(1);
	i = 0;
	shadowDescriptorLayoutConfig[i].type = UNIFORM_BUFFER;
	shadowDescriptorLayoutConfig[i++].stages = VERTEX_STAGE;
	PipelineConfiguration shadowPipelineConfiguration = Pipeline::DefaultConfiguration();
	shadowPipelineConfiguration.shadow = true;
	shadowPipelineConfiguration.pushConstantCount = 1;
	shadowPipelineConfiguration.pushConstantStage = VERTEX_STAGE;
	shadowPipelineConfiguration.pushConstantSize = sizeof(uint32_t);
    shadowPipeline.CreateGraphicsPipeline("buildingShadow", shadowDescriptorLayoutConfig, shadowPipelineConfiguration, vertexInfo);
}

void Buildings::CreateBuffers()
{
	buildingBuffers.resize(maxRenderBuildings);
	uniformBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration bufferConfiguration;
	bufferConfiguration.size = sizeof(BuildingBuffer) * buildingBuffers.size();
	bufferConfiguration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	bufferConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferConfiguration.mapped = true;

	for (Buffer &buffer : uniformBuffers)
	{
		buffer.Create(bufferConfiguration);
	}

	//uniformBuffer.Create(bufferConfiguration);

	buildingShadowBuffers.resize(maxRenderBuildings);
	uniformShadowBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration shadowBufferConfiguration;
	shadowBufferConfiguration.size = sizeof(BuildingBuffer) * buildingShadowBuffers.size();
	shadowBufferConfiguration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	shadowBufferConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	shadowBufferConfiguration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	shadowBufferConfiguration.mapped = true;

	for (Buffer &buffer : uniformShadowBuffers)
	{
		buffer.Create(bufferConfiguration);
	}

	//uniformShadowBuffer.Create(shadowBufferConfiguration);
}

void Buildings::CreateDescriptors()
{
    std::vector<DescriptorConfiguration> graphicsDescriptorConfig(5);

	int index = 0;

	graphicsDescriptorConfig[index].type = UNIFORM_BUFFER;
	graphicsDescriptorConfig[index].stages = VERTEX_STAGE;
	graphicsDescriptorConfig[index].buffersInfo.resize(uniformBuffers.size());
	for (int i = 0; i < uniformBuffers.size(); i++)
	{
		graphicsDescriptorConfig[index].buffersInfo[i].buffer = uniformBuffers[i].buffer;
		graphicsDescriptorConfig[index].buffersInfo[i].range = sizeof(BuildingBuffer) * buildingBuffers.size();
	}
	index++;
	//graphicsDescriptorConfig[index].buffersInfo[0].buffer = uniformBuffer.buffer;
	//graphicsDescriptorConfig[index++].buffersInfo[0].range = sizeof(BuildingBuffer) * buildingBuffers.size();

	graphicsDescriptorConfig[index].type = IMAGE_SAMPLER;
	graphicsDescriptorConfig[index].stages = FRAGMENT_STAGE;
	graphicsDescriptorConfig[index].count = beamTextures.size();
	graphicsDescriptorConfig[index].imageInfos.resize(beamTextures.size());
	for (int i = 0; i < beamTextures.size(); i++)
	{
		graphicsDescriptorConfig[index].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		graphicsDescriptorConfig[index].imageInfos[i].imageView = beamTextures[i].imageView;
		graphicsDescriptorConfig[index].imageInfos[i].sampler = beamTextures[i].sampler;
	}
	index++;

	graphicsDescriptorConfig[index].type = IMAGE_SAMPLER;
	graphicsDescriptorConfig[index].stages = FRAGMENT_STAGE;
	graphicsDescriptorConfig[index].count = plasteredTextures.size();
	graphicsDescriptorConfig[index].imageInfos.resize(plasteredTextures.size());
	for (int i = 0; i < plasteredTextures.size(); i++)
	{
		graphicsDescriptorConfig[index].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		graphicsDescriptorConfig[index].imageInfos[i].imageView = plasteredTextures[i].imageView;
		graphicsDescriptorConfig[index].imageInfos[i].sampler = plasteredTextures[i].sampler;
	}
	index++;

	graphicsDescriptorConfig[index].type = IMAGE_SAMPLER;
	graphicsDescriptorConfig[index].stages = FRAGMENT_STAGE;
	graphicsDescriptorConfig[index].count = reedTextures.size();
	graphicsDescriptorConfig[index].imageInfos.resize(reedTextures.size());
	for (int i = 0; i < reedTextures.size(); i++)
	{
		graphicsDescriptorConfig[index].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		graphicsDescriptorConfig[index].imageInfos[i].imageView = reedTextures[i].imageView;
		graphicsDescriptorConfig[index].imageInfos[i].sampler = reedTextures[i].sampler;
	}
	index++;

	graphicsDescriptorConfig[index].type = IMAGE_SAMPLER;
	graphicsDescriptorConfig[index].stages = FRAGMENT_STAGE;
	graphicsDescriptorConfig[index].count = brickTextures.size();
	graphicsDescriptorConfig[index].imageInfos.resize(brickTextures.size());
	for (int i = 0; i < brickTextures.size(); i++)
	{
		graphicsDescriptorConfig[index].imageInfos[i].imageLayout = LAYOUT_READ_ONLY;
		graphicsDescriptorConfig[index].imageInfos[i].imageView = brickTextures[i].imageView;
		graphicsDescriptorConfig[index].imageInfos[i].sampler = brickTextures[i].sampler;
	}
	index++;

	//graphicsDescriptor.perFrame = false;
    graphicsDescriptor.Create(graphicsDescriptorConfig, graphicsPipeline.objectDescriptorSetLayout);

	std::vector<DescriptorConfiguration> shadowDescriptorConfig(1);

	index = 0;

	shadowDescriptorConfig[index].type = UNIFORM_BUFFER;
	shadowDescriptorConfig[index].stages = VERTEX_STAGE;
	shadowDescriptorConfig[index].buffersInfo.resize(uniformShadowBuffers.size());
	for (int i = 0; i < uniformShadowBuffers.size(); i++)
	{
		shadowDescriptorConfig[index].buffersInfo[i].buffer = uniformShadowBuffers[i].buffer;
		shadowDescriptorConfig[index].buffersInfo[i].range = sizeof(BuildingBuffer) * buildingShadowBuffers.size();
	}
	index++;
	//shadowDescriptorConfig[index].buffersInfo[0].buffer = uniformShadowBuffer.buffer;
	//shadowDescriptorConfig[index++].buffersInfo[0].range = sizeof(BuildingBuffer) * buildingShadowBuffers.size();

	//shadowDescriptor.perFrame = false;
	shadowDescriptor.Create(shadowDescriptorConfig, shadowPipeline.objectDescriptorSetLayout);
}

void Buildings::Destroy()
{
    DestroyMeshes();
    DestroyTextures();
	DestroyPipelines();
	DestroyBuffers();
	DestroyDescriptors();
	DestroyBuildings();
}

void Buildings::DestroyMeshes()
{
	for (int i = 0; i < buildings.size(); i++)
	{
		if (buildings[i]->active)
		{
			buildings[i]->mesh.Destroy();
			buildings[i]->lodMesh.Destroy();
		}
	}
}

void Buildings::DestroyTextures()
{
    for (Texture &texture : beamTextures) texture.Destroy();
    beamTextures.clear();

	for (Texture &texture : plasteredTextures) texture.Destroy();
    plasteredTextures.clear();

	for (Texture &texture : reedTextures) texture.Destroy();
    reedTextures.clear();

	for (Texture &texture : brickTextures) texture.Destroy();
    brickTextures.clear();
}

void Buildings::DestroyPipelines()
{
    graphicsPipeline.Destroy();
	shadowPipeline.Destroy();
}

void Buildings::DestroyBuffers()
{
	for (Buffer &buffer : uniformBuffers)
	{
		buffer.Destroy();
	}
	uniformBuffers.clear();

	for (Buffer &buffer : uniformShadowBuffers)
	{
		buffer.Destroy();
	}
	uniformShadowBuffers.clear();

	//uniformBuffer.Destroy();
	//uniformShadowBuffer.Destroy();
}

void Buildings::DestroyDescriptors()
{
    graphicsDescriptor.Destroy();
	shadowDescriptor.Destroy();
}

void Buildings::DestroyBuildings()
{
	for (int i = 0; i < buildings.size(); i++)
	{
		if (!buildings[i]) continue;
		delete(buildings[i]);
	}
	buildings.clear();
}

void Buildings::Start()
{
	Menu &menu = UI::NewMenu("buildings");
	menu.AddNode("generation config", true);
	menu.AddNode("bounds", true);
	menu.AddRange("width", generationConfig.minSize.x, generationConfig.maxSize.x, 1, 10);
	menu.AddRange("length", generationConfig.minSize.z, generationConfig.maxSize.z, 1, 10);
	menu.AddRange("height", generationConfig.minSize.y, generationConfig.maxSize.y, 1, 10);
	menu.AddNode("bounds", false);
	menu.AddNode("factors", true);
	menu.AddSlider("expansion", generationConfig.expansionFactor, 1, 10);
	menu.AddSlider("level", generationConfig.levelFactor, 1, 10);
	menu.AddSlider("decorated", generationConfig.decoratedFactor, 0, 10);
	menu.AddSlider("scaffolding", generationConfig.scaffoldingReduction, 1, 10);
	menu.AddNode("factors", false);
	menu.AddNode("part properties", true);
	std::vector<void *> parts = {&floorConfig, &foundationConfig, &flatWallConfig, &windowedWallConfig, 
		&slopedWallConfig, &slopedLowWallConfig, &flatRoofConfig, &flatLowRoofConfig, &slopedRoofConfig, &slopedLowRoofConfig, 
		&coneRoofConfig, &coneLowRoofConfig,
		&coneRoofExtensionConfig, &beamConfig, &slopedBeamConfig, &slightSlopedBeamConfig, &collumnConfig};
	for (void *part : parts)
	{
		PartConfig *p = (PartConfig *)part;
		menu.AddNode(p->name.c_str(), true);
		std::string s = "scale##" + p->name;
		menu.AddDrag(s.c_str(), p->scale, 0.005f);
		std::string r = "rotation##" + p->name;
		menu.AddDrag(r.c_str(), p->rotation, 0.1f);
		std::string o = "offset##" + p->name;
		menu.AddDrag(o.c_str(), p->offset, 0.005f);
		menu.AddNode(p->name.c_str(), false);
	}
	menu.AddNode("part properties", false);
	menu.AddInput("seed", generationConfig.seed);
	menu.AddCheck("lod", generationConfig.lod);
	menu.AddCheck("random", generationConfig.random);
	menu.AddButton("generate", GenerateBuilding);
	menu.AddNode("generation config", false);

	generationConfig.minSize = glm::ivec3(3);
	generationConfig.maxSize = glm::ivec3(3);
	//generationConfig.seed = 224388;
	//generationConfig.seed = 1347167;
	//generationConfig.seed = 1372670;
	//generationConfig.scaffoldingReduction = 1;
	generationConfig.random = true;

	//building = &buildings[0];
	//GenerateBuilding();
	//building->position.y = 2500.0f;
	//building->Update();

	//building = new Building;
	//building->id = currentActiveBuildings;
	//GenerateBuilding();
	//building->position += glm::vec3(0.0f, 2500.0f, 0.0f);
	//building->Update();
	//currentActiveBuildings++;
	//buildings.push_back(building);
}

void Buildings::Frame()
{
	return;

	if (!Terrain::HeightMapsGenerated()) return;

	int villageRange = 8;
	static int buildingIndex = 0;
	static int buildingX = -villageRange;
	static int buildingY = -villageRange;
	static int villageIndex = 0;

	if (Time::newFrameTick)
	{
		if (buildingX <= villageRange)
		{
			if (buildingY <= villageRange)
			{
				if (random.Next(0, villageRange) >= glm::max(abs(buildingX), abs(buildingY)))
				{
					//building = &buildings[buildingIndex];
					building = new Building;
					building->id = currentActiveBuildings;
					GenerateBuilding();
					building->position += glm::vec3(5.0f * generationConfig.scale * buildingX, 0.0f, 5.0f * generationConfig.scale * buildingY);
					if (villageIndex == 0) building->position += glm::vec3(3250.0f, 0.0f * generationConfig.scale, 4000.0f);
					else if (villageIndex == 1) building->position += glm::vec3(8125.0f, 0.0f * generationConfig.scale, 5075.0f);
					else if (villageIndex == 2) building->position += glm::vec3(5650.0f, 0.0f * generationConfig.scale, 3030.0f);
					building->Update();
					Data::RequestData(building->position, &building->position.y, UpdateBuilding, buildingIndex);
					buildingIndex++;
					currentActiveBuildings++;
					buildings.push_back(building);
				}

				buildingY++;
			}

			if (buildingY > villageRange)
			{
				buildingX++;
				buildingY = -villageRange;
			}
		}

		//if (buildingX > villageRange && villageIndex < 2)
		if (buildingX > villageRange && villageIndex < 0)
		{
			buildingX = -villageRange;
			villageIndex++;
		}
	}
}

Building *Buildings::CreateBuilding()
{
	building = new Building;
	currentActiveBuildings++;
	building->id = currentActiveBuildings;
	buildings.push_back(building);
	GenerateCells();
	return (building);
}

void Buildings::DestroyBuilding(int id)
{
	for (int i = 0; i < buildings.size(); i++)
	{
		if (buildings[i]->id == id)
		{
			buildings[i]->mesh.Destroy();
			buildings[i]->lodMesh.Destroy();
			delete(buildings[i]);
			buildings.erase(buildings.begin() + i);
			break;
		}
	}
}

bool Buildings::CreateBuildingMesh(Building *targetBuilding, bool lod)
{
	if (generating) return (false);

	generating = true;

	building = targetBuilding;
	generationConfig.lod = lod;
	GenerateMesh();
	
	generating = false;

	return (true);
}

void Buildings::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
    graphicsPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	graphicsDescriptor.Bind(commandBuffer, graphicsPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderBuildings(commandBuffer);
}

void Buildings::RecordShadowCommands(VkCommandBuffer commandBuffer, int cascade)
{
	if (cascade > 2) return;

	shadowPipeline.BindGraphics(commandBuffer);

	Manager::globalDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 0);
	shadowDescriptor.Bind(commandBuffer, shadowPipeline.graphicsPipelineLayout, GRAPHICS_BIND_POINT, 1);

	RenderShadows(commandBuffer, cascade);
}

void Buildings::RenderBuildings(VkCommandBuffer commandBuffer)
{
	SetRenderBuildings();

	for (int i = 0; i < renderBuildings.size(); i++)
	{
		if (renderBuildings[i]->lod)
		{
			renderBuildings[i]->lodMesh.Bind(commandBuffer);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(renderBuildings[i]->lodMesh.indices.size()), 1, 0, 0, i);
		}
		else
		{
			renderBuildings[i]->mesh.Bind(commandBuffer);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(renderBuildings[i]->mesh.indices.size()), 1, 0, 0, i);
		}
	}
}

void Buildings::RenderShadows(VkCommandBuffer commandBuffer, int cascade)
{
	uint32_t cascadeIndex = cascade;

	if (cascade == 0) SetRenderBuildingsShadow();

	vkCmdPushConstants(commandBuffer, shadowPipeline.graphicsPipelineLayout, VERTEX_STAGE, 0, sizeof(cascadeIndex), &cascadeIndex);

	for (int i = 0; i < renderBuildingsShadow.size(); i++)
	{
		if (renderBuildingsShadow[i]->lod)
		{
			renderBuildingsShadow[i]->lodMesh.Bind(commandBuffer);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(renderBuildingsShadow[i]->lodMesh.indices.size()), 1, 0, 0, i);
		}
		else
		{
			renderBuildingsShadow[i]->mesh.Bind(commandBuffer);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(renderBuildingsShadow[i]->mesh.indices.size()), 1, 0, 0, i);
		}
	}
}

void Buildings::SetRenderBuildings()
{
	renderBuildings.clear();

	/*for (int i = 0; i < Simulation::settlements.size(); i++)
	{
		if (renderBuildings.size() >= maxRenderBuildings) break;

		std::vector<SettlementCell *> renderCells = Simulation::settlements[i]->GetRenderCells();

		for (int j = 0; j < renderCells.size(); j++)
		{
			renderBuildings.push_back(renderCells[j]->building);
			buildingBuffers[renderBuildings.size() - 1].translation = renderCells[j]->building->translation;
			buildingBuffers[renderBuildings.size() - 1].orientation = renderCells[j]->building->orientation;

			if (renderBuildings.size() >= maxRenderBuildings) break;
		}
	}*/

	for (int i = 0; i < buildings.size(); i++)
	{
		if (buildings[i]->active && BuildingInView(buildings[i]))
		{
			renderBuildings.push_back(buildings[i]);
			buildingBuffers[renderBuildings.size() - 1].translation = buildings[i]->translation;
			buildingBuffers[renderBuildings.size() - 1].orientation = buildings[i]->orientation;

			if (renderBuildings.size() >= maxRenderBuildings) break;
		}
	}

	memcpy(uniformBuffers[Manager::currentFrame].mappedBuffer, buildingBuffers.data(), sizeof(BuildingBuffer) * renderBuildings.size());
}

void Buildings::SetRenderBuildingsShadow()
{
	renderBuildingsShadow.clear();

	/*for (int i = 0; i < Simulation::settlements.size(); i++)
	{
		if (renderBuildingsShadow.size() >= maxRenderBuildings) break;

		std::vector<SettlementCell *> renderCells = Simulation::settlements[i]->GetRenderCells();

		for (int j = 0; j < renderCells.size(); j++)
		{
			renderBuildingsShadow.push_back(renderCells[j]->building);
			buildingShadowBuffers[renderBuildingsShadow.size() - 1].translation = renderCells[j]->building->translation;
			buildingShadowBuffers[renderBuildingsShadow.size() - 1].orientation = renderCells[j]->building->orientation;

			if (renderBuildingsShadow.size() >= maxRenderBuildings) break;
		}
	}*/

	for (int i = 0; i < buildings.size(); i++)
	{
		if (buildings[i]->active && BuildingInView(buildings[i]))
		{
			renderBuildingsShadow.push_back(buildings[i]);
			buildingShadowBuffers[renderBuildingsShadow.size() - 1].translation = buildings[i]->translation;
			buildingShadowBuffers[renderBuildingsShadow.size() - 1].orientation = buildings[i]->orientation;

			if (renderBuildingsShadow.size() >= maxRenderBuildings) break;
		}
	}

	memcpy(uniformShadowBuffers[Manager::currentFrame].mappedBuffer, buildingShadowBuffers.data(), sizeof(BuildingBuffer) * renderBuildingsShadow.size());
}

void Buildings::GenerateBuilding()
{
	GenerateCells();
	generationConfig.lod = false;
	GenerateMesh();
	generationConfig.lod = true;
	GenerateMesh();
	building->active = true;

	building->rotation = glm::vec3(0.0f, random.Next(0, 360), 0.0f);
	building->Update();

	//std::cout << std::endl;
	//std::cout << "vertex count: " << building->mesh.vertices.size() << std::endl;
	//std::cout << "indice count: " << building->mesh.indices.size() << std::endl;
}

void Buildings::GenerateCells()
{
	building->cells.clear();
	building->size = generationConfig.maxSize;
	generationConfig.currentSize = glm::ivec3(0, 0, 0);
	if (generationConfig.random) generationConfig.seed = int(uint32_t(Time::GetCurrentTime() * 10000) % RAND_MAX);
	random.SetSeed(generationConfig.seed);

	building->cells.resize(generationConfig.maxSize.y);
    for (int i = 0; i < generationConfig.maxSize.y; i++)
    {
        building->cells[i].resize(generationConfig.maxSize.x);
        for (int x = 0; x < generationConfig.maxSize.x; x++)
        {
            building->cells[i][x].resize(generationConfig.maxSize.z);
            for (int y = 0; y < generationConfig.maxSize.z; y++)
            {
                
            }
        }
    }

	for (int i = 0; i < generationConfig.maxSize.y; i++)
	{
		if (i < generationConfig.minSize.y || random.Next(0, 9) < generationConfig.levelFactor)
		{
			ExpandLevel(i);
			FillLevel(i);
			generationConfig.currentSize = glm::ivec3(0, generationConfig.currentSize.y + 1, 0);
		}
		else break;
	}

	SetWalls();
	SetBeams();
	SetRoof();
}

void Buildings::ExpandLevel(int level)
{
    glm::ivec2 start;

    start.x = int(round(float(generationConfig.maxSize.x - 1) * 0.5));
    start.y = int(round(float(generationConfig.maxSize.z - 1) * 0.5));

	ExpandCell(level, start.x, start.y, glm::clamp(generationConfig.expansionFactor - level, 1, generationConfig.expansionFactor));
}

void Buildings::FillLevel(int level)
{
	for (int x = 0; x < building->cells[level].size(); x++)
	{
		for (int y = 0; y < building->cells[level][x].size(); y++)
		{
			if (CellEmpty(level, x, y))
			{
				bool N_Empty = CellEmpty(level, x, y + 1, false);
				bool S_Empty = CellEmpty(level, x, y - 1, false);
				bool E_Empty = CellEmpty(level, x + 1, y, false);
				bool W_Empty = CellEmpty(level, x - 1, y, false);

				if (!N_Empty && !S_Empty && !E_Empty && !W_Empty)
				{
					building->cells[level][x][y].floor.type = FloorType::flat;
				}
			}
		}
	}
}

void Buildings::ExpandCell(int i, int x, int y, int factor)
{
    building->cells[i][x][y].floor.type = FloorType::flat;

	int currentFactor = factor - ((CellEmpty(i - 1, x + 1, y) && i != 0) ? generationConfig.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x + 1, y, currentFactor, 1)) ExpandCell(i, x + 1, y, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x - 1, y) && i != 0) ? generationConfig.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x - 1, y, currentFactor, 1)) ExpandCell(i, x - 1, y, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x, y + 1) && i != 0) ? generationConfig.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x, y + 1, currentFactor, -1)) ExpandCell(i, x, y + 1, currentFactor);

	currentFactor = factor - ((CellEmpty(i - 1, x, y - 1) && i != 0) ? generationConfig.scaffoldingReduction - 1 : 0);
	if (ExpansionValid(i, x, y - 1, currentFactor, -1)) ExpandCell(i, x, y - 1, currentFactor);
}

bool Buildings::ExpansionValid(int i, int x, int y, int factor, int increase)
{
    if ((increase == 1 && generationConfig.currentSize.x < generationConfig.minSize.x - i) || (increase == -1 && generationConfig.currentSize.z < generationConfig.minSize.z - i))
        factor = 10;
    
    bool valid = CellValid(i, x, y) && building->cells[i][x][y].Empty() && random.Next(0, 9) < factor;

    if (valid && increase == 1) generationConfig.currentSize.x++;
    else if (valid && increase == -1) generationConfig.currentSize.z++;

    return (valid);
}

void Buildings::SetWalls()
{
	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				if (!building->cells[i][x][y].floor.Empty())
				{
					bool N_Empty = FloorEmpty(i, x, y + 1);
					bool S_Empty = FloorEmpty(i, x, y - 1);
					bool E_Empty = FloorEmpty(i, x + 1, y);
					bool W_Empty = FloorEmpty(i, x - 1, y);
					bool N_DownEmpty = FloorEmpty(i - 1, x, y + 1);
					bool S_DownEmpty = FloorEmpty(i - 1, x, y - 1);
					bool E_DownEmpty = FloorEmpty(i - 1, x + 1, y);
					bool W_DownEmpty = FloorEmpty(i - 1, x - 1, y);
					bool NE_DownEmpty = FloorEmpty(i - 1, x + 1, y + 1) || !IsRoof(i - 1, x + 1, y + 1);
                    bool NW_DownEmpty = FloorEmpty(i - 1, x - 1, y + 1) || !IsRoof(i - 1, x - 1, y + 1);
                    bool SE_DownEmpty = FloorEmpty(i - 1, x + 1, y - 1) || !IsRoof(i - 1, x + 1, y - 1);
                    bool SW_DownEmpty = FloorEmpty(i - 1, x - 1, y - 1) || !IsRoof(i - 1, x - 1, y - 1);
					bool S_Balcony = i != 0 && SE_DownEmpty && SW_DownEmpty;
                    bool N_Balcony = i != 0 && NE_DownEmpty && NW_DownEmpty;
                    bool E_Balcony = i != 0 && NE_DownEmpty && SE_DownEmpty;
                    bool W_Balcony = i != 0 && NW_DownEmpty && SW_DownEmpty;

					if (N_Empty)
					{
						building->cells[i][x][y].walls.N_Type = WallType::flat;
						building->cells[i][x][y].walls.N_Variant = random.Next(0, 21 - generationConfig.decoratedFactor);
						if (N_DownEmpty && (x + i + generationConfig.seed) % 2 == 0)
						{
							if (N_Balcony && random.Next(0, 9) < generationConfig.balconyFactor)
							{
								building->cells[i][x][y].walls.N_Type = WallType::balcony;
							}
							else
							{
								building->cells[i][x][y].walls.N_Type = WallType::window;
							}
						}
					}

					if (S_Empty)
					{
						building->cells[i][x][y].walls.S_Type = WallType::flat;
						building->cells[i][x][y].walls.S_Variant = random.Next(0, 21 - generationConfig.decoratedFactor);
						if (S_DownEmpty && (x + 1 + i + generationConfig.seed) % 2 == 0)
						{
							if (S_Balcony && random.Next(0, 9) < generationConfig.balconyFactor)
							{
								building->cells[i][x][y].walls.S_Type = WallType::balcony;
							}
							else
							{
								building->cells[i][x][y].walls.S_Type = WallType::window;
							}
						}
					}

					if (E_Empty)
					{
						building->cells[i][x][y].walls.E_Type = WallType::flat;
						building->cells[i][x][y].walls.E_Variant = random.Next(0, 21 - generationConfig.decoratedFactor);
						if (E_DownEmpty && (y + i + generationConfig.seed) % 2 == 0)
						{
							if (E_Balcony && random.Next(0, 9) < generationConfig.balconyFactor)
							{
								building->cells[i][x][y].walls.E_Type = WallType::balcony;
							}
							else
							{
								building->cells[i][x][y].walls.E_Type = WallType::window;
							}
						}
					}

					if (W_Empty)
					{
						building->cells[i][x][y].walls.W_Type = WallType::flat;
						building->cells[i][x][y].walls.W_Variant = random.Next(0, 21 - generationConfig.decoratedFactor);
						if (W_DownEmpty && (y + 1 + i + generationConfig.seed) % 2 == 0)
						{
							if (W_Balcony && random.Next(0, 9) < generationConfig.balconyFactor)
							{
								building->cells[i][x][y].walls.W_Type = WallType::balcony;
							}
							else
							{
								building->cells[i][x][y].walls.W_Type = WallType::window;
							}
						}
					}
				}
			}
		}
	}
}

void Buildings::SetBeams()
{
	for (int i = building->cells.size() - 1; i >= 0; i--)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				if (building->cells[i][x][y].walls.Empty() && !CellEmpty(i + 1, x, y))
				{
					bool N_Empty = CellEmpty(i, x, y + 1);
                    bool S_Empty = CellEmpty(i, x, y - 1);
                    bool E_Empty = CellEmpty(i, x + 1, y);
                    bool W_Empty = CellEmpty(i, x - 1, y);
                    bool NE_Empty = CellEmpty(i, x + 1, y + 1);
                    bool NW_Empty = CellEmpty(i, x - 1, y + 1);
                    bool SE_Empty = CellEmpty(i, x + 1, y - 1);
                    bool SW_Empty = CellEmpty(i, x - 1, y - 1);

					bool N_Beam = BeamPass(i, x, y, N, N_Empty, S_Empty, E_Empty, W_Empty, NE_Empty, NW_Empty, SE_Empty, SW_Empty);
					bool S_Beam = BeamPass(i, x, y, S, N_Empty, S_Empty, E_Empty, W_Empty, NE_Empty, NW_Empty, SE_Empty, SW_Empty);
					bool E_Beam = BeamPass(i, x, y, E, N_Empty, S_Empty, E_Empty, W_Empty, NE_Empty, NW_Empty, SE_Empty, SW_Empty);
					bool W_Beam = BeamPass(i, x, y, W, N_Empty, S_Empty, E_Empty, W_Empty, NE_Empty, NW_Empty, SE_Empty, SW_Empty);

					building->cells[i][x][y].walls.N_Type = N_Beam ? WallType::beams : WallType::empty;
                    building->cells[i][x][y].walls.S_Type = S_Beam ? WallType::beams : WallType::empty;
                    building->cells[i][x][y].walls.E_Type = E_Beam ? WallType::beams : WallType::empty;
                    building->cells[i][x][y].walls.W_Type = W_Beam ? WallType::beams : WallType::empty;
				}
			}
		}
	}
}

bool Buildings::BeamPass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool NE_Empty, bool NW_Empty, bool SE_Empty, bool SW_Empty)
{
	if (direction == N)
	{
		bool N = N_Empty || building->cells[i][x][y + 1].walls.E_Type == WallType::empty;
        bool E = E_Empty || building->cells[i][x + 1][y].walls.W_Type == WallType::empty;
        bool NE = NE_Empty || building->cells[i][x + 1][y + 1].walls.S_Type == WallType::empty;
        return (N && E && NE);
	}
	else if (direction == S)
	{
		bool S = S_Empty || building->cells[i][x][y - 1].walls.W_Type == WallType::empty;
        bool W = W_Empty || building->cells[i][x - 1][y].walls.E_Type == WallType::empty;
        bool SW = SW_Empty || building->cells[i][x - 1][y - 1].walls.N_Type == WallType::empty;
        return (S && W && SW);
	}
	else if (direction == E)
	{
		bool S = S_Empty || building->cells[i][x][y - 1].walls.N_Type == WallType::empty;
        bool E = E_Empty || building->cells[i][x + 1][y].walls.S_Type == WallType::empty;
        bool SE = SE_Empty || building->cells[i][x + 1][y - 1].walls.W_Type == WallType::empty;
        return (S && E && SE);
	}
	else if (direction == W)
	{
		bool N = N_Empty || building->cells[i][x][y + 1].walls.S_Type == WallType::empty;
        bool W = W_Empty || building->cells[i][x - 1][y].walls.N_Type == WallType::empty;
        bool NW = NW_Empty || building->cells[i][x - 1][y + 1].walls.E_Type == WallType::empty;
        return (N && W && NW);
	}
	return (false);
}

void Buildings::SetRoof()
{
	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofTypePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofTypePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofConePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofTypePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofDirectionPass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofMergePass(i, x, y);
			}
		}
	}

	for (int i = 0; i < building->cells.size(); i++)
	{
		for (int x = 0; x < building->cells[i].size(); x++)
		{
			for (int y = 0; y < building->cells[i][x].size(); y++)
			{
				RoofExtendPass(i, x, y);
			}
		}
	}
}

void Buildings::RoofTypePass(int i, int x, int y)
{
	if (IsRoof(i, x, y))
	{
		bool N_Empty = CellEmpty(i, x, y + 1);
		bool S_Empty = CellEmpty(i, x, y - 1);
		bool E_Empty = CellEmpty(i, x + 1, y);
		bool W_Empty = CellEmpty(i, x - 1, y);
		bool N_UpEmpty = CellEmpty(i + 1, x, y + 1);
		bool S_UpEmpty = CellEmpty(i + 1, x, y - 1);
		bool E_UpEmpty = CellEmpty(i + 1, x + 1, y);
		bool W_UpEmpty = CellEmpty(i + 1, x - 1, y);
		int emptyAmount = ((N_Empty && N_UpEmpty) ? 1 : 0) + ((S_Empty && S_UpEmpty) ? 1 : 0) +
			((E_Empty && E_UpEmpty) ? 1 : 0) + ((W_Empty && W_UpEmpty) ? 1 : 0);
		bool N_Cone = !N_Empty && building->cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && building->cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && building->cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && building->cells[i][x - 1][y].roof.type == RoofType::cone;
		int coneAmount = (N_Cone ? 1 : 0) + (S_Cone ? 1 : 0) + (E_Cone ? 1 : 0) + (W_Cone ? 1 : 0);
		int EW_Empty = ((E_Empty || E_Cone) ? 1 : 0) + ((W_Empty || W_Cone) ? 1 : 0);

		if (building->cells[i][x][y].roof.type == RoofType::cone || emptyAmount >= 3)
		{
			building->cells[i][x][y].roof.type = RoofType::cone;
		}
		else if (emptyAmount + coneAmount >= 1 && EW_Empty != 0)
		{
			//building->cells[i][x][y].roof.type = RoofType::slopeLow;
			building->cells[i][x][y].roof.type = RoofType::slope;
		}
		else 
		{
			building->cells[i][x][y].roof.type = RoofType::flatUp;
		}
	}
}

void Buildings::RoofConePass(int i, int x, int y)
{
	if (!building->cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1);
		bool S_Empty = CellEmpty(i, x, y - 1);
		bool E_Empty = CellEmpty(i, x + 1, y);
		bool W_Empty = CellEmpty(i, x - 1, y);
		int emptyAmount = (N_Empty ? 1 : 0) + (S_Empty ? 1 : 0) + (E_Empty ? 1 : 0) + (W_Empty ? 1 : 0);
		bool N_Cone = !N_Empty && building->cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && building->cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && building->cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && building->cells[i][x - 1][y].roof.type == RoofType::cone;
		int coneAmount = (N_Cone ? 1 : 0) + (S_Cone ? 1 : 0) + (E_Cone ? 1 : 0) + (W_Cone ? 1 : 0);
		bool N_Wall = !N_Empty && !CellEmpty(i + 1, x, y + 1);
		bool S_Wall = !N_Empty && !CellEmpty(i + 1, x, y - 1);
		bool E_Wall = !N_Empty && !CellEmpty(i + 1, x + 1, y);
		bool W_Wall = !N_Empty && !CellEmpty(i + 1, x - 1, y);
		int wallAmount = (N_Wall ? 1 : 0) + (S_Wall ? 1 : 0) + (E_Wall ? 1 : 0) + (W_Wall ? 1 : 0);

		if (building->cells[i][x][y].roof.type != RoofType::cone && ((emptyAmount == 2 && coneAmount == 2) ||
            (emptyAmount == 1 && coneAmount == 3) || (emptyAmount == 2 && coneAmount == 1) || 
            (emptyAmount == 1 && ((N_Cone && S_Cone) || (E_Cone && W_Cone))) ||
            (emptyAmount == 1 && coneAmount == 2 && wallAmount == 1))) 
            building->cells[i][x][y].roof.type = RoofType::cone;
	}
}

void Buildings::RoofDirectionPass(int i, int x, int y)
{
	if (!building->cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1, false);
		bool S_Empty = CellEmpty(i, x, y - 1, false);
		bool E_Empty = CellEmpty(i, x + 1, y, false);
		bool W_Empty = CellEmpty(i, x - 1, y, false);
		bool N_UpEmpty = CellEmpty(i + 1, x, y + 1, false);
		bool S_UpEmpty = CellEmpty(i + 1, x, y - 1, false);
		bool E_UpEmpty = CellEmpty(i + 1, x + 1, y, false);
		bool W_UpEmpty = CellEmpty(i + 1, x - 1, y, false);
		bool N_Cone = !N_Empty && building->cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && building->cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && building->cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && building->cells[i][x - 1][y].roof.type == RoofType::cone;

		building->cells[i][x][y].roof.direction = GetRoofDirection(building->cells[i][x][y].roof.type, N_Empty, S_Empty, E_Empty,
			W_Empty, N_Cone, S_Cone, E_Cone, W_Cone, E_UpEmpty, W_UpEmpty);
	}
}

void Buildings::RoofMergePass(int i, int x, int y)
{
	if (!building->cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1);
		bool S_Empty = CellEmpty(i, x, y - 1);
		bool E_Empty = CellEmpty(i, x + 1, y);
		bool W_Empty = CellEmpty(i, x - 1, y);
		bool N_Cone = !N_Empty && building->cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && building->cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && building->cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && building->cells[i][x - 1][y].roof.type == RoofType::cone;

		if (building->cells[i][x][y].roof.type == RoofType::cone)
		{
			if (building->cells[i][x][y].roof.direction == N)
			{
				building->cells[i][x][y].roof.N_Merge = MergePass(i, x, y, N, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
				building->cells[i][x][y].roof.S_Merge = MergePass(i, x, y, S, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
			}
			else if (building->cells[i][x][y].roof.direction == E)
			{
				building->cells[i][x][y].roof.E_Merge = MergePass(i, x, y, E, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
				building->cells[i][x][y].roof.W_Merge = MergePass(i, x, y, W, N_Empty, S_Empty, E_Empty, W_Empty, N_Cone, S_Cone, E_Cone, W_Cone);
			}
		}
	}
}

void Buildings::RoofExtendPass(int i, int x, int y)
{
	if (!building->cells[i][x][y].roof.Empty())
	{
		bool N_Empty = CellEmpty(i, x, y + 1, false);
		bool S_Empty = CellEmpty(i, x, y - 1, false);
		bool E_Empty = CellEmpty(i, x + 1, y, false);
		bool W_Empty = CellEmpty(i, x - 1, y, false);
		bool N_UpEmpty = CellEmpty(i + 1, x, y + 1, false);
		bool S_UpEmpty = CellEmpty(i + 1, x, y - 1, false);
		bool E_UpEmpty = CellEmpty(i + 1, x + 1, y, false);
		bool W_UpEmpty = CellEmpty(i + 1, x - 1, y, false);
		bool N_Cone = !N_Empty && building->cells[i][x][y + 1].roof.type == RoofType::cone;
		bool S_Cone = !S_Empty && building->cells[i][x][y - 1].roof.type == RoofType::cone;
		bool E_Cone = !E_Empty && building->cells[i][x + 1][y].roof.type == RoofType::cone;
		bool W_Cone = !W_Empty && building->cells[i][x - 1][y].roof.type == RoofType::cone;
		bool NE_UpObstruction = !CellEmpty(i + 1, x + 1, y + 1) && (building->cells[i + 1][x + 1][y + 1].walls.W_Type == WallType::window || building->cells[i + 1][x + 1][y + 1].walls.W_Type == WallType::balcony);
		bool NW_UpObstruction = !CellEmpty(i + 1, x - 1, y + 1) && (building->cells[i + 1][x - 1][y + 1].walls.W_Type == WallType::window || building->cells[i + 1][x - 1][y + 1].walls.W_Type == WallType::balcony);
		bool SW_UpObstruction = !CellEmpty(i + 1, x - 1, y - 1) && (building->cells[i + 1][x - 1][y - 1].walls.W_Type == WallType::window || building->cells[i + 1][x - 1][y - 1].walls.W_Type == WallType::balcony);
		bool SE_UpObstruction = !CellEmpty(i + 1, x + 1, y - 1) && (building->cells[i + 1][x + 1][y - 1].walls.W_Type == WallType::window || building->cells[i + 1][x + 1][y - 1].walls.W_Type == WallType::balcony);

		if (building->cells[i][x][y].roof.type == RoofType::cone)
		{
			building->cells[i][x][y].roof.N_Extend = N_Empty;
			building->cells[i][x][y].roof.S_Extend = S_Empty;
			building->cells[i][x][y].roof.E_Extend = E_Empty;
			building->cells[i][x][y].roof.W_Extend = W_Empty;
		}
		else
		{
			building->cells[i][x][y].roof.N_Extend = (N_Empty && N_UpEmpty) || N_Cone;
			building->cells[i][x][y].roof.S_Extend = (S_Empty && S_UpEmpty) || S_Cone;
			building->cells[i][x][y].roof.E_Extend = (E_Empty && E_UpEmpty) || E_Cone;
			building->cells[i][x][y].roof.W_Extend = (W_Empty && W_UpEmpty) || W_Cone;
		}

		if (building->cells[i][x][y].roof.type == RoofType::flatUp)
		{
			building->cells[i][x][y].roof.N_Extend = building->cells[i][x][y].roof.N_Extend || (N_Empty && N_UpEmpty) ||
				(building->cells[i][x][y + 1].roof.type == RoofType::slope && building->cells[i][x][y + 1].roof.direction != S);
			building->cells[i][x][y].roof.S_Extend = building->cells[i][x][y].roof.S_Extend || (S_Empty && S_UpEmpty) ||
				(building->cells[i][x][y - 1].roof.type == RoofType::slope && building->cells[i][x][y - 1].roof.direction != N);
			building->cells[i][x][y].roof.E_Extend = building->cells[i][x][y].roof.E_Extend || (E_Empty && E_UpEmpty) ||
				(building->cells[i][x + 1][y].roof.type == RoofType::slope && building->cells[i][x + 1][y].roof.direction != W);
			building->cells[i][x][y].roof.W_Extend = building->cells[i][x][y].roof.W_Extend || (W_Empty && W_UpEmpty) ||
				(building->cells[i][x - 1][y].roof.type == RoofType::slope && building->cells[i][x - 1][y].roof.direction != E);
		}
	}
}

D Buildings::GetRoofDirection(RoofType type, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone,
	bool S_Cone, bool E_Cone, bool W_Cone, bool E_UpEmpty, bool W_UpEmpty)
{
	int NS = (N_Empty ? 0 : 1) + (S_Empty ? 0 : 1);
	int EW = (E_Empty ? 0 : 1) + (W_Empty ? 0 : 1);

	if (type == RoofType::cone)
	{
		if (NS == EW)
		{
			if (!N_Cone && !N_Empty) return (N);
			else if (!E_Cone && !E_Empty) return (E);
			else if (!S_Cone && !S_Empty) return (N);
			else if (!W_Cone && !W_Empty) return (E);
			else if (N_Cone) return (N);
			else if (E_Cone) return (E);
			else if (S_Cone) return (N);
			else if (W_Cone) return (E);
		}
		else
		{
			//NS = (N_Empty ? 0 : (N_Cone ? 1 : 3)) + (S_Empty ? 0 : (S_Cone ? 1 : 3));
			//EW = (E_Empty ? 0 : (E_Cone ? 1 : 3)) + (W_Empty ? 0 : (W_Cone ? 1 : 3));
			return NS > EW ? N : E;
		}
	}
	else if (type == RoofType::slope)
	{
		EW = ((E_Empty || E_Cone) ? 1 : 0) + ((W_Empty || W_Cone) ? 1 : 0);
		if (E_Cone) return (W);
		else if (W_Cone) return (E);
		else if (!E_Empty) return (E);
		else if (!W_Empty) return (W);
		else
		{
			if (E_Empty && E_UpEmpty) return (W);
			else return (E);
		}
	}

	return (N);
}

bool Buildings::MergePass(int i, int x, int y, D direction, bool N_Empty, bool S_Empty, bool E_Empty, bool W_Empty, bool N_Cone, bool S_Cone, bool E_Cone, bool W_Cone)
{
	bool merge = false;

	if (direction == N)
	{
		if (N_Cone && building->cells[i][x][y + 1].roof.direction == E) 
			merge = true;
		else if (!N_Empty && building->cells[i][x][y + 1].roof.type == RoofType::slope && building->cells[i][x][y + 1].roof.direction == N) 
			merge = true;
	}
	else if (direction == S)
	{
		if (S_Cone && building->cells[i][x][y - 1].roof.direction == E)
			merge = true;
		else if (!S_Empty && building->cells[i][x][y - 1].roof.type == RoofType::slope && building->cells[i][x][y - 1].roof.direction == S)
			merge = true;
	}
	else if (direction == E)
	{
		if (E_Cone && building->cells[i][x + 1][y].roof.direction == N)
			merge = true;
		else if (!E_Empty && building->cells[i][x + 1][y].roof.type == RoofType::slope && building->cells[i][x + 1][y].roof.direction == E)
			merge = true;
	}
	else if (direction == W)
	{
		if (W_Cone && building->cells[i][x - 1][y].roof.direction == N)
			merge = true;
		else if (!W_Empty && building->cells[i][x - 1][y].roof.type == RoofType::slope && building->cells[i][x - 1][y].roof.direction == W)
			merge = true;
	}

	return (merge);
}

bool Buildings::IsRoof(int i, int x, int y)
{
	return (CellValid(i, x, y) && !building->cells[i][x][y].Empty(false) && (!CellValid(i + 1, x, y) || 
		building->cells[i + 1][x][y].Empty(false)));
}

Shape Buildings::GeneratePart(PartType type, glm::vec3 scale, glm::vec3 offset, glm::vec3 rotation)
{
	Shape part;
	part.coordinate = true;
	part.normal = true;
	part.color = true;

	if (type == PartType::floor)
	{
		//Shape floor = Shape(CUBE, true, true, true);
		Shape floor = Shape(NO_SHAPE, true, true, true);
		floor.SetCube(false, false, false, false, true, true);
		floor.SetColors(glm::vec3(0));
		floor.Scale(floorConfig.scale * generationConfig.scale);
		floor.Scale(scale);
		floor.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		floor.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		floor.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		floor.Move(offset * generationConfig.scale);

		part.Join(floor);
	}
	else if (type == PartType::foundation)
	{
		Shape floor = Shape(CUBE, true, true, true);
		floor.SetColors(glm::vec3(3, 0, 0));
		floor.Scale(foundationConfig.scale * generationConfig.scale);
		floor.Scale(scale);
		floor.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		floor.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		floor.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		floor.Move(foundationConfig.offset * generationConfig.scale);
		floor.Move(offset * generationConfig.scale);

		part.Join(floor);
	}
	else if (type == PartType::flatWall)
	{
		//Shape wall = Shape(CUBE, true, true, true);
		Shape wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(flatWallConfig.scale * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move(flatWallConfig.offset * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);

		part.Join(wall);
	}
	else if (type == PartType::windowedWall)
	{
		Shape wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(windowedWallConfig.scale * glm::vec3(1.0f, 0.25f, 1.0f) * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((windowedWallConfig.offset + glm::vec3(0.0f, -0.375f, 0.0f)) * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(windowedWallConfig.scale * glm::vec3(1.0f, 0.25f, 1.0f) * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((windowedWallConfig.offset + glm::vec3(0.0f, 0.375f, 0.0f)) * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(windowedWallConfig.scale * glm::vec3(0.25f, 0.5f, 1.0f) * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((windowedWallConfig.offset + glm::vec3(0.375f, 0.0f, 0.0f)) * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(windowedWallConfig.scale * glm::vec3(0.25f, 0.5f, 1.0f) * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((windowedWallConfig.offset + glm::vec3(-0.375f, 0.0f, 0.0f)) * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);
		part.Join(wall);
	}
	else if (type == PartType::dooredWall)
	{
		Shape wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(dooredWallConfig.scale * glm::vec3(0.5f, 0.25f, 1.0f) * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((dooredWallConfig.offset + glm::vec3(0.0f, 0.375f, 0.0f)) * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(dooredWallConfig.scale * glm::vec3(0.25f, 1.0f, 1.0f) * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((dooredWallConfig.offset + glm::vec3(0.375f, 0.0f, 0.0f)) * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);
		part.Join(wall);

		wall = Shape(NO_SHAPE, true, true, true);
		wall.SetCube(true, true, false, false, false, false);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(dooredWallConfig.scale * glm::vec3(0.25f, 1.0f, 1.0f) * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move((dooredWallConfig.offset + glm::vec3(-0.375f, 0.0f, 0.0f)) * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);
		part.Join(wall);
	}
	else if (type == PartType::slopedWall)
	{
		Shape wall = Shape(PRISM, true, true, true);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(slopedWallConfig.scale * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move(slopedWallConfig.offset * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);

		part.Join(wall);
	}
	else if (type == PartType::slopedLowWall)
	{
		Shape wall = Shape(PRISM, true, true, true);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(slopedLowWallConfig.scale * generationConfig.scale);
		wall.Scale(scale);
		wall.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		wall.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		wall.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		wall.Move(slopedLowWallConfig.offset * generationConfig.scale);
		wall.Move(offset * generationConfig.scale);

		part.Join(wall);
	}
	else if (type == PartType::flatRoof)
	{
		Shape roof = Shape(CUBE, true, true, true);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.SwapCoordinates();
		roof.Scale(flatRoofConfig.scale * generationConfig.scale);
		roof.Scale(scale);
		roof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		roof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		roof.Move(flatRoofConfig.offset * generationConfig.scale);
		roof.Move(offset * generationConfig.scale);

		part.Join(roof);
	}
	else if (type == PartType::slopedRoof)
	{
		Shape roof = Shape(CUBE, true, true, true);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.Scale(slopedRoofConfig.scale * generationConfig.scale);
		roof.Scale(scale);
		roof.Rotate(slopedRoofConfig.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		roof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		roof.Move(slopedRoofConfig.offset * generationConfig.scale);
		roof.Move(offset * generationConfig.scale);

		part.Join(roof);
	}
	else if (type == PartType::slopedLowRoof)
	{
		Shape roof = Shape(CUBE, true, true, true);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.Scale(slopedLowRoofConfig.scale * generationConfig.scale);
		roof.Scale(scale);
		roof.Rotate(slopedLowRoofConfig.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		roof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		roof.Move(slopedLowRoofConfig.offset * generationConfig.scale);
		roof.Move(offset * generationConfig.scale);

		part.Join(roof);
	}
	else if (type == PartType::coneRoof)
	{
		Shape leftRoof = Shape(CUBE, true, true, true);
		leftRoof.InverseCoordinates(false, true);
		leftRoof.SwapCoordinates();
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(coneRoofConfig.scale * generationConfig.scale);
		leftRoof.Scale(scale);
		leftRoof.Rotate(coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		leftRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		leftRoof.Move(coneRoofConfig.offset * generationConfig.scale);
		leftRoof.Move(offset * generationConfig.scale);

		Shape rightRoof = Shape(CUBE, true, true, true);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.SwapCoordinates();
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(coneRoofConfig.scale * generationConfig.scale);
		rightRoof.Scale(scale);
		rightRoof.Rotate(-coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rightRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rightRoof.Move(coneRoofConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * generationConfig.scale);
		rightRoof.Move(offset * generationConfig.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::coneLowRoof)
	{
		Shape leftRoof = Shape(CUBE, true, true, true);
		leftRoof.InverseCoordinates(false, true);
		leftRoof.SwapCoordinates();
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(coneLowRoofConfig.scale * generationConfig.scale);
		leftRoof.Scale(scale);
		leftRoof.Rotate(coneLowRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		leftRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		leftRoof.Move(coneLowRoofConfig.offset * generationConfig.scale);
		leftRoof.Move(offset * generationConfig.scale);

		Shape rightRoof = Shape(CUBE, true, true, true);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.SwapCoordinates();
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(coneLowRoofConfig.scale * generationConfig.scale);
		rightRoof.Scale(scale);
		rightRoof.Rotate(-coneLowRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rightRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rightRoof.Move(coneLowRoofConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * generationConfig.scale);
		rightRoof.Move(offset * generationConfig.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::coneRoofExtension)
	{
		Shape leftRoof = Shape(PRISM, true, true, true);
		leftRoof.InverseCoordinates(false, false);
		leftRoof.SwapCoordinates();
		leftRoof.ScaleCoordinates(glm::vec2(0.5f, 1.0f));
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(coneRoofExtensionConfig.scale * generationConfig.scale);
		leftRoof.Scale(scale);
		leftRoof.Rotate90();
		leftRoof.Rotate(-coneRoofExtensionConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		leftRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		leftRoof.Move(coneRoofExtensionConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * generationConfig.scale);
		leftRoof.Move(offset * generationConfig.scale);

		Shape rightRoof = Shape(PRISM, true, true, true);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.ScaleCoordinates(glm::vec2(0.5f, 1.0f));
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(coneRoofExtensionConfig.scale * generationConfig.scale);
		rightRoof.Scale(scale);
		rightRoof.Rotate90();
		rightRoof.Rotate(coneRoofExtensionConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rightRoof.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rightRoof.Move(coneRoofExtensionConfig.offset * generationConfig.scale);
		rightRoof.Move(offset * generationConfig.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::beam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(4, 0, 0));
		beam.Scale(beamConfig.scale * generationConfig.scale);
		beam.Scale(scale);
		beam.Rotate(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		beam.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		beam.Move(beamConfig.offset * generationConfig.scale);
		beam.Move(offset * generationConfig.scale);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));

		part.Join(beam);
	}
	else if (type == PartType::slopedBeam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(4, 0, 0));
		beam.Scale(slopedBeamConfig.scale * generationConfig.scale);
		beam.Scale(scale);
		beam.Rotate(slopedBeamConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		beam.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		beam.Move(slopedBeamConfig.offset * generationConfig.scale);
		beam.Move(offset * generationConfig.scale);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));

		part.Join(beam);
	}
	else if (type == PartType::slightSlopedBeam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(4, 0, 0));
		beam.Scale(slightSlopedBeamConfig.scale * generationConfig.scale);
		beam.Scale(scale);
		beam.Rotate(slightSlopedBeamConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		beam.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		beam.Move(slightSlopedBeamConfig.offset * generationConfig.scale);
		beam.Move(offset * generationConfig.scale);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));

		part.Join(beam);
	}
	else if (type == PartType::collumn)
	{
		//Shape collumn = Shape(CUBE, true, true, true);
		Shape collumn = Shape(NO_SHAPE, true, true, true);
		collumn.SetCube(true, true, true, true, false, false);
		collumn.SetColors(glm::vec3(0, 0, 0));
		collumn.Scale(collumnConfig.scale * generationConfig.scale);
		collumn.Scale(scale);
		collumn.Rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		collumn.Rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		collumn.Rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		collumn.Move(collumnConfig.offset * generationConfig.scale);
		collumn.Move(offset * generationConfig.scale);
		collumn.ScaleCoordinates(glm::vec2(0.14f, 1.0f));

		part.Join(collumn);
	}
	
	return (part);
}

Shape Buildings::GeneratePart(PartType type, int rotate)
{
	return (GeneratePart(type, glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 90.0f * rotate, 0.0f)));

	/*Shape part;
	part.coordinate = true;
	part.normal = true;
	part.color = true;

	if (type == PartType::floor)
	{
		//Shape floor = Shape(CUBE, true, true, true);
		Shape floor = Shape(NO_SHAPE, true, true, true);
		floor.SetCube(false, false, false, false, true, true);
		floor.SetColors(glm::vec3(0, 0, 0));
		floor.Scale(floorConfig.scale * generationConfig.scale);

		part.Join(floor);
	}
	else if (type == PartType::foundation)
	{
		Shape floor = Shape(CUBE, true, true, true);
		floor.SetColors(glm::vec3(3, 0, 0));
		floor.Scale(foundationConfig.scale * generationConfig.scale);
		floor.Move(foundationConfig.offset * generationConfig.scale);

		part.Join(floor);
	}
	else if (type == PartType::flatWall)
	{
		Shape wall = Shape(CUBE, true, true, true);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(flatWallConfig.scale * generationConfig.scale);
		wall.Move(flatWallConfig.offset * generationConfig.scale);

		part.Join(wall);
	}
	else if (type == PartType::windowedWall)
	{
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				if ((y == 1 || y == 2) && (x == 1 || x == 2)) continue;

				Shape wall = Shape(CUBE, true, true, true);
				wall.SetColors(glm::vec3(1, 0, 0));
				wall.Scale(windowedWallConfig.scale * 0.25f * generationConfig.scale);
				wall.Move((windowedWallConfig.offset + glm::vec3(-0.375f + 0.25f * x, -0.375f + 0.25f * y, 0.0f)) * generationConfig.scale);
				part.Join(wall);
			}
		}
	}
	else if (type == PartType::slopedWall)
	{
		Shape wall = Shape(PRISM, true, true, true);
		wall.SetColors(glm::vec3(1, 0, 0));
		wall.Scale(slopedWallConfig.scale * generationConfig.scale);
		wall.Rotate90(rotate);
		wall.Move(slopedWallConfig.offset * generationConfig.scale);

		part.Join(wall);
	}
	else if (type == PartType::flatRoof)
	{
		Shape roof = Shape(CUBE, true, true, true);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.SwapCoordinates();
		roof.Scale(flatRoofConfig.scale * generationConfig.scale);
		roof.Move(flatRoofConfig.offset * generationConfig.scale);

		part.Join(roof);
	}
	else if (type == PartType::slopedRoof)
	{
		Shape roof = Shape(CUBE, true, true, true);
		roof.SetColors(glm::vec3(2, 0, 0));
		roof.Scale(slopedRoofConfig.scale * generationConfig.scale);
		roof.Rotate(slopedRoofConfig.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		roof.Move(slopedRoofConfig.offset * generationConfig.scale);

		part.Join(roof);
	}
	else if (type == PartType::coneRoof)
	{
		Shape leftRoof = Shape(CUBE, true, true, true);
		leftRoof.InverseCoordinates(false, true);
		leftRoof.SwapCoordinates();
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(coneRoofConfig.scale * generationConfig.scale);
		leftRoof.Rotate(coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Move(coneRoofConfig.offset * generationConfig.scale);

		Shape rightRoof = Shape(CUBE, true, true, true);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.SwapCoordinates();
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(coneRoofConfig.scale * generationConfig.scale);
		rightRoof.Rotate(-coneRoofConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Move(coneRoofConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * generationConfig.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::coneRoofExtension)
	{
		Shape leftRoof = Shape(PRISM, true, true, true);
		leftRoof.Rotate90(rotate);
		leftRoof.InverseCoordinates(false, false);
		leftRoof.SwapCoordinates();
		leftRoof.ScaleCoordinates(glm::vec2(0.5f, 1.0f));
		leftRoof.SetColors(glm::vec3(2, 0, 0));
		leftRoof.Scale(coneRoofExtensionConfig.scale * generationConfig.scale);
		leftRoof.Rotate90();
		leftRoof.Rotate(-coneRoofExtensionConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		leftRoof.Move(coneRoofExtensionConfig.offset * glm::vec3(-1.0f, 1.0f, 1.0f) * generationConfig.scale);

		Shape rightRoof = Shape(PRISM, true, true, true);
		rightRoof.Rotate90(rotate);
		rightRoof.InverseCoordinates(true, true);
		rightRoof.ScaleCoordinates(glm::vec2(0.5f, 1.0f));
		rightRoof.SetColors(glm::vec3(2, 0, 0));
		rightRoof.Scale(coneRoofExtensionConfig.scale * generationConfig.scale);
		rightRoof.Rotate90();
		rightRoof.Rotate(coneRoofExtensionConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		rightRoof.Move(coneRoofExtensionConfig.offset * generationConfig.scale);

		part.Join(leftRoof);
		part.Join(rightRoof);
	}
	else if (type == PartType::beam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(4, 0, 0));
		beam.Scale(beamConfig.scale * generationConfig.scale);
		beam.Rotate(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));
		beam.Move(beamConfig.offset * generationConfig.scale);

		part.Join(beam);
	}
	else if (type == PartType::slopedBeam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(4, 0, 0));
		beam.Scale(slopedBeamConfig.scale * generationConfig.scale);
		beam.Rotate(slopedBeamConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate90(rotate);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));
		beam.Move(slopedBeamConfig.offset * generationConfig.scale);

		part.Join(beam);
	}
	else if (type == PartType::slightSlopedBeam)
	{
		Shape beam = Shape(CUBE, true, true, true);
		beam.SetColors(glm::vec3(4, 0, 0));
		beam.Scale(slightSlopedBeamConfig.scale * generationConfig.scale);
		beam.Rotate(slightSlopedBeamConfig.rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
		beam.Rotate90(rotate);
		beam.ScaleCoordinates(glm::vec2(0.14f, 1.0f));
		beam.Move(slightSlopedBeamConfig.offset * generationConfig.scale);

		part.Join(beam);
	}
	else if (type == PartType::collumn)
	{
		Shape collumn = Shape(CUBE, true, true, true);
		collumn.SetColors(glm::vec3(0, 0, 0));
		collumn.Scale(collumnConfig.scale * generationConfig.scale);
		collumn.ScaleCoordinates(glm::vec2(0.14f, 1.0f));
		collumn.Move(collumnConfig.offset * generationConfig.scale);

		part.Join(collumn);
	}
	
	return (part);*/
}

void Buildings::GenerateMesh()
{
	Mesh *currentMesh = &building->mesh;
	if (generationConfig.lod) currentMesh = &building->lodMesh;

	if (currentMesh->created)
		currentMesh->DestroyAtRuntime();

	currentMesh->coordinate = true;
	currentMesh->normal = true;
	currentMesh->color = true;
	currentMesh->shape.coordinate = true;
	currentMesh->shape.normal = true;
	currentMesh->shape.color = true;

	for (int i = 0; i < building->cells.size(); i++)
	{
		GenerateFloors(i);
		GenerateWalls(i);
		GenerateRoofs(i);
	}

	float xOffset = float(generationConfig.maxSize.x - 1) * 0.5;
	float zOffset = float(generationConfig.maxSize.z - 1) * 0.5;
	currentMesh->shape.Move(glm::vec3(-generationConfig.scale * xOffset, 0.375f * generationConfig.scale, -generationConfig.scale * zOffset));
	currentMesh->RecalculateVertices();
	currentMesh->Create();
}

void Buildings::GenerateFloors(int level)
{
	for (int x = 0; x < building->cells[level].size(); x++)
	{
		for (int y = 0; y < building->cells[level][x].size(); y++)
		{
			if (!building->cells[level][x][y].Empty())
			{
				GenerateFloor(level, x , y, building->cells [level][x][y].floor.type);
			}
		}
	}
}

void Buildings::GenerateFloor(int i, int x, int y, FloorType type)
{
	Shape floor;
	floor.normal = true;
	floor.coordinate = true;
	floor.color = true;

	if (type == FloorType::flat && !generationConfig.lod)
	{
		Shape flatFloor = GeneratePart(PartType::floor);
		floor.Join(flatFloor);
	}

	if (i == 0 && !building->cells[i][x][y].walls.Empty())
	{
		if (!building->cells[i][x][y].walls.Empty(false))
		{
			Shape foundation = GeneratePart(PartType::foundation);
			floor.Join(foundation);
		}

		if (!generationConfig.lod)
		{
			bool N_Empty = CellEmpty(i, x, y + 1, false);
			bool S_Empty = CellEmpty(i, x, y - 1, false);
			bool E_Empty = CellEmpty(i, x + 1, y, false);
			bool W_Empty = CellEmpty(i, x - 1, y, false);
			bool empties[] = {N_Empty, E_Empty, S_Empty, W_Empty};

			for (int d = 0; d < 4; d++)
			{
				if (empties[d] && empties[(d + 1) % 4])
				{
					Shape collumn = GeneratePart(PartType::collumn);
					collumn.Rotate90(d);
					collumn.Move(glm::vec3(0.0f, -1.0f, 0.0f) * generationConfig.scale);
					floor.Join(collumn);
					//break;
				}
			}
		}
	}

	floor.Move(glm::vec3(x, i, y) * generationConfig.scale);

	if (generationConfig.lod) building->lodMesh.shape.Join(floor);
	else building->mesh.shape.Join(floor);
}

void Buildings::GenerateWalls(int level)
{
	for (int x = 0; x < building->cells[level].size(); x++)
	{
		for (int y = 0; y < building->cells[level][x].size(); y++)
		{
			Walls walls = building->cells[level][x][y].walls;

			if (walls.N_Type != WallType::empty && walls.N_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * generationConfig.scale, walls.N_Type, N, walls.N_Variant);
			}
			if (walls.S_Type != WallType::empty && walls.S_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * generationConfig.scale, walls.S_Type, S, walls.S_Variant);
			}
			if (walls.E_Type != WallType::empty && walls.E_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * generationConfig.scale, walls.E_Type, E, walls.E_Variant);
			}
			if (walls.W_Type != WallType::empty && walls.W_Type != WallType::beams)
			{
				GenerateWall(glm::vec3(x, level, y) * generationConfig.scale, walls.W_Type, W, walls.W_Variant);
			}

			if (walls.Beams() && !generationConfig.lod)
			{
				GenerateBeams(level, x, y);
			}
		}
	}
}

void Buildings::GenerateWall(glm::vec3 offset, WallType type, D direction, int variant)
{
	PartType partType = PartType::flatWall;
	if (type == WallType::window && !generationConfig.lod) partType = PartType::windowedWall;
	else if (type == WallType::balcony && !generationConfig.lod) partType = PartType::dooredWall;
	Shape wall = GeneratePart(partType);

	if (!generationConfig.lod)
	{
		Shape beamLow = GeneratePart(PartType::beam);
		wall.Join(beamLow);

		Shape beamHigh = GeneratePart(PartType::beam);
		beamHigh.Move(glm::vec3(0.0f, 1.0f, 0.0f) * generationConfig.scale);
		wall.Join(beamHigh);

		Shape collumn = GeneratePart(PartType::collumn);
		wall.Join(collumn);
	}

	if (type == WallType::flat && variant > 0 && !generationConfig.lod)
	{
		if (variant <= 6)
		{
			float angle = 22.5f;
			float height = 1.0f - 0.225f;
			if (variant % 2 == 0)
			{
				height = 0.225f;
				angle = -22.5f;
			}
			Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.95f, 0.75f), glm::vec3(0.0f, height, 0.0f), glm::vec3(0.0f, 0.0f, angle));
			wall.Join(slopeBeam);
		}
		if (variant <= 4 || variant == 7 || variant == 8)
		{
			float angle = -22.5f;
			float height = 0.225f;
			if (variant % 2 == 0 && variant != 8)
			{
				height = 1.0f - 0.225f;
				angle = 22.5f;
			}
			Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.95f, 0.75f), glm::vec3(0.0f, height, 0.0f), glm::vec3(0.0f, 0.0f, angle));
			wall.Join(slopeBeam);
		}
		if (variant >= 3 && variant <= 9)
		{
			Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.95f, 0.75f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			wall.Join(slopeBeam);
		}
		if (variant == 10 || variant == 11)
		{
			Shape slopeBeam = GeneratePart(PartType::collumn, glm::vec3(0.75f, 0.95f, 0.75f), glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			wall.Join(slopeBeam);
		}
		if (variant >= 10 && variant % 2 == 0)
		{
			Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.375f, 0.75f), glm::vec3(-0.375f, 0.85f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f));
			wall.Join(slopeBeam);
			slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.375f, 0.75f), glm::vec3(0.375f, 0.85f, 0.0f), glm::vec3(0.0f, 0.0f, -45.0f));
			wall.Join(slopeBeam);
		}
	}

	if ((type == WallType::window || type == WallType::balcony) && !generationConfig.lod)
	{
		Shape frame = GeneratePart(PartType::beam, glm::vec3(0.7f, 0.5f, 0.7f), glm::vec3(0.0f, 0.725f, 0.0f), glm::vec3(0.0f));
		wall.Join(frame);
		frame = GeneratePart(PartType::collumn, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(-0.75f, 0.0f, 0.0f), glm::vec3(0.0f));
		wall.Join(frame);
		frame = GeneratePart(PartType::collumn, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(-0.25f, 0.0f, 0.0f), glm::vec3(0.0f));
		wall.Join(frame);

		if (type == WallType::window)
		{
			frame = GeneratePart(PartType::beam, glm::vec3(0.7f, 0.5f, 0.7f), glm::vec3(0.0f, 0.275f, 0.0f), glm::vec3(0.0f));
			wall.Join(frame);
		}
	}

	if (type == WallType::balcony && !generationConfig.lod)
	{
		Shape balcony = GeneratePart(PartType::floor, glm::vec3(1.0f, 1.0f, 0.5f), glm::vec3(0.0f, 0.0f, 0.75f), glm::vec3(0.0f));

		Shape beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.5f, 0.75f), glm::vec3(0.5f, 0.0f, 0.25f), glm::vec3(0.0f, 90.0f, 0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.5f, 0.75f), glm::vec3(-0.5f, 0.0f, 0.25f), glm::vec3(0.0f, -90.0f, 0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(0.0f, 0.25f, 0.5f), glm::vec3(0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.5f, 0.75f), glm::vec3(0.5f, 0.25f, 0.25f), glm::vec3(0.0f, 90.0f, 0.0f));
		balcony.Join(beam);
		beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.5f, 0.75f), glm::vec3(-0.5f, 0.25f, 0.25f), glm::vec3(0.0f, -90.0f, 0.0f));
		balcony.Join(beam);

		Shape tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(0.0f, -0.375f, 0.25f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-1.0f, -0.375f, 0.25f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(0.0f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-1.0f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-0.25f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-0.75f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);
		tinyCollumn = GeneratePart(PartType::collumn, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(-0.5f, -0.375f, 0.5f), glm::vec3(0.0f));
		balcony.Join(tinyCollumn);

		Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.55f, 0.75f), glm::vec3(0.375f, -0.15f, 0.0f), glm::vec3(0.0f, 0.0f, -30.0f));
		slopeBeam.Rotate90(1);
		slopeBeam.Move(glm::vec3(0.0f, 0.0f, 1.125f) * generationConfig.scale);
		balcony.Join(slopeBeam);
		slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.55f, 0.75f), glm::vec3(0.375f, -0.15f, 0.0f), glm::vec3(0.0f, 0.0f, -30.0f));
		slopeBeam.Rotate90(1);
		slopeBeam.Move(glm::vec3(-1.0f, 0.0f, 1.125f) * generationConfig.scale);
		balcony.Join(slopeBeam);

		wall.Join(balcony);
	}

	if (direction == S) wall.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == E) wall.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == W) wall.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	wall.Move(offset);

	if (generationConfig.lod) building->lodMesh.shape.Join(wall);
	else building->mesh.shape.Join(wall);
}

void Buildings::GenerateBeams(int i, int x, int y)
{
	Shape beams;
	beams.normal = true;
	beams.coordinate = true;
	beams.color = true;

	Walls walls = building->cells[i][x][y].walls;

	bool N_Empty_B = CellEmpty(i, x, y + 1, false);
    bool N_Empty = CellEmpty(i, x, y + 1);
    bool S_Empty = CellEmpty(i, x, y - 1);
    bool E_Empty_B = CellEmpty(i, x + 1, y, false);
    bool E_Empty = CellEmpty(i, x + 1, y);
    bool W_Empty = CellEmpty(i, x - 1, y);

	bool empties[] = {N_Empty, E_Empty, S_Empty, W_Empty};
	WallType types[] = {walls.N_Type, walls.E_Type, walls.S_Type, walls.W_Type};

	for (int d = 0; d < 4; d++)
	{
		if (types[d] == WallType::beams)
		{
			Shape collumn = GeneratePart(PartType::collumn);
			if (empties[d] && empties[(d + 1) % 4])
			{
				
				Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.375f, 0.75f), glm::vec3(0.375f, 0.85f, 0.0f), glm::vec3(0.0f, 0.0f, -45.0f));
				collumn.Join(slopeBeam);
				slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.375f, 0.75f), glm::vec3(-0.375f, 0.85f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f));
				slopeBeam.Rotate90(1);
				collumn.Join(slopeBeam);
			}
			collumn.Rotate90(d);
			beams.Join(collumn);
		}
	}

	if (i != 0)
    {
        if (N_Empty_B)
		{
			Shape beam = GeneratePart(PartType::beam);

			beams.Join(beam);
		}
		if (E_Empty_B)
		{
			Shape beam = GeneratePart(PartType::beam);

			beam.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			beams.Join(beam);
		}
		if (S_Empty)
		{
			Shape beam = GeneratePart(PartType::beam);

			beam.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			beams.Join(beam);
		}
		if (W_Empty)
		{
			Shape beam = GeneratePart(PartType::beam);

			beam.Rotate(270.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			beams.Join(beam);
		}
    }

	/*if (walls.N_Type == WallType::beams)
	{
		Shape collumn = GeneratePart(PartType::collumn);
		beams.Join(collumn);

		//if (N_Empty && E_Empty)
		//{
		//	Shape slopeBeam = GeneratePart(PartType::beam, glm::vec3(0.75f, 0.375f, 0.75f), glm::vec3(-0.375f, 0.85f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f));
		//	beams.Join(slopeBeam);
		//}
	}
	if (walls.E_Type == WallType::beams)
	{
		Shape collumn = GeneratePart(PartType::collumn);
		collumn.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		beams.Join(collumn);
	}
	if (walls.S_Type == WallType::beams)
	{
		Shape collumn = GeneratePart(PartType::collumn);
		collumn.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		beams.Join(collumn);
	}
	if (walls.W_Type == WallType::beams)
	{
		Shape collumn = GeneratePart(PartType::collumn);
		collumn.Rotate(270.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		beams.Join(collumn);
	}*/

	beams.Move(glm::vec3(x, i, y) * generationConfig.scale);
	building->mesh.shape.Join(beams);
}

void Buildings::GenerateRoofs(int level)
{
	for (int x = 0; x < building->cells[level].size(); x++)
	{
		for (int y = 0; y < building->cells[level][x].size(); y++)
		{
			if (!building->cells[level][x][y].roof.Empty())
			{
				GenerateRoof(level, x, y, building->cells[level][x][y].roof.type,
					building->cells[level][x][y].roof.direction);
			}
		}
	}
}

void Buildings::GenerateRoof(int i, int x, int y, RoofType type, D direction)
{
	Roof roofData = building->cells[i][x][y].roof;

	Shape roof;
	roof.coordinate = true;
	roof.normal = true;
	roof.color = true;

	if (type == RoofType::flatUp)
	{
		roof = GeneratePart(PartType::flatRoof);
		float extendScale = 1.0f;
		float extendOffset = 0.0f;
		if (roofData.LocalExtend(N))
		{
			extendScale += 0.2f;
			extendOffset += 0.1f;
		}
		if (roofData.LocalExtend(S))
		{
			extendScale += 0.2f;
			extendOffset -= 0.1f;
		}
		roof.Scale(glm::vec3(1.0f, 1.0f, extendScale));
		roof.Move(glm::vec3(0.0f, 0.0f, extendOffset) * generationConfig.scale);

		if (!generationConfig.lod)
		{
			Shape sideBeam = GeneratePart(PartType::beam);
			sideBeam.Rotate90(-1);
			sideBeam.Scale(glm::vec3(1.0f, 1.0f, extendScale));
			sideBeam.Move(glm::vec3(0.0f, 0.0f, extendOffset) * generationConfig.scale);
			sideBeam.Move(flatRoofConfig.offset * generationConfig.scale);
			roof.Join(sideBeam);

			if (!CellValid(i, x + 1, y) || (building->cells[i][x + 1][y].roof.type != RoofType::flatUp &&
				building->cells[i][x + 1][y].roof.type != RoofType::slope))
			{
				sideBeam = GeneratePart(PartType::beam);
				sideBeam.Rotate90();
				sideBeam.Scale(glm::vec3(1.0f, 1.0f, extendScale));
				sideBeam.Move(glm::vec3(0.0f, 0.0f, extendOffset) * generationConfig.scale);
				sideBeam.Move(flatRoofConfig.offset * generationConfig.scale);
				roof.Join(sideBeam);
			}
		}

		for (int d = 0; d < 4; d++)
		{
			if (roofData.Extend((D)d))
			{
				Shape wall = GeneratePart(PartType::flatWall);
				//wall.Scale(glm::vec3(1.0f, 0.6f, 1.0f));

				if (!generationConfig.lod)
				{
					Shape beam = GeneratePart(PartType::beam);
					//beam.Move(glm::vec3(0.0f, 0.6f, 0.0f) * generationConfig.scale);
					beam.Move(glm::vec3(0.0f, 0.95f, 0.0f) * generationConfig.scale);
					wall.Join(beam);

					Shape collumn = GeneratePart(PartType::collumn);
					//collumn.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
					wall.Join(collumn);

					beam = GeneratePart(PartType::beam, glm::vec3(0.75f, 1.0f, 0.75f), glm::vec3(0.0f, 0.0f, 0.075f), glm::vec3(0.0f));
					//beam.Move(glm::vec3(0.0f, 0.6f, 0.0f) * generationConfig.scale);
					beam.Move(glm::vec3(0.0f, 0.95f, 0.0f) * generationConfig.scale);
					wall.Join(beam);
				}

				wall.Rotate90(d);
				roof.Join(wall);
			}
		}
	}
	else if (type == RoofType::slope || type == RoofType::slopeLow)
	{
		PartType slopedRoof = type == RoofType::slope ? PartType::slopedRoof : PartType::slopedLowRoof;
		PartType slopedWall = type == RoofType::slope ? PartType::slopedWall : PartType::slopedLowWall;

		roof = GeneratePart(slopedRoof);
		float extendScale = 1.0f;
		float extendOffset = 0.0f;
		if (roofData.LocalExtend(E))
		{
			extendScale += 0.2f;
			extendOffset -= 0.1f;
		}
		if (roofData.LocalExtend(W))
		{
			extendScale += 0.2f;
			extendOffset += 0.1f;
		}
		roof.Scale(glm::vec3(extendScale, 1.0f, 1.0f));
		roof.Move(glm::vec3(extendOffset, 0.0f, 0.0f) * generationConfig.scale);

		if (!generationConfig.lod)
		{
			if (!CellValid(i, x + 1, y) || (building->cells[i][x + 1][y].roof.type != RoofType::flatUp &&
				building->cells[i][x + 1][y].roof.type != RoofType::slope))
			{
				Shape sideBeam = GeneratePart(PartType::beam);
				sideBeam.Scale(glm::vec3(extendScale, 1.0f, 1.0f));
				sideBeam.Move(glm::vec3(extendOffset, 0.0f, 0.0f) * generationConfig.scale);
				sideBeam.Move(flatRoofConfig.offset * generationConfig.scale);
				sideBeam.Move(glm::vec3(0.0f, 0.0f, beamConfig.offset.z) * -2.0f * generationConfig.scale);
				roof.Join(sideBeam);
			}

			if (roofData.Extend(E))
			{
				Shape sideBeam = GeneratePart(PartType::beam);
				sideBeam.Scale(glm::vec3(extendScale, 1.0f, 1.0f));
				sideBeam.Move(glm::vec3(extendOffset, 0.0f, 0.0f) * generationConfig.scale);
				sideBeam.Move(flatRoofConfig.offset * generationConfig.scale);
				sideBeam.Move(glm::vec3(0.0f, 0.0f, beamConfig.offset.z) * -2.0f * generationConfig.scale);
				roof.Join(sideBeam);
			}

			Shape midBeam = GeneratePart(PartType::slopedBeam, glm::vec3(0.75f, 1.3f, 0.75f), glm::vec3(-0.05f, -0.035f, -0.5f), glm::vec3(0.0f));
			midBeam.Rotate90();
			midBeam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * generationConfig.scale);
			roof.Join(midBeam);
		}

		if (roofData.LocalExtend(W))
		{
			Shape wall = GeneratePart(slopedWall);
			wall.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
			wall.Rotate90();
			roof.Join(wall);

			if (!generationConfig.lod)
			{
				Shape collumn = GeneratePart(PartType::collumn);
				//collumn.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
				collumn.Move(glm::vec3(0.0f, 0.0f, -collumnConfig.offset.z * 2.0f) * generationConfig.scale);
				roof.Join(collumn);

				Shape beam = GeneratePart(PartType::slopedBeam);
				beam.Rotate90();
				beam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * generationConfig.scale);
				roof.Join(beam);

				beam = GeneratePart(PartType::slopedBeam, glm::vec3(0.75f, 1.3f, 0.75f), glm::vec3(-0.05f, -0.035f, 0.075f), glm::vec3(0.0f));
				beam.Rotate90();
				beam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * generationConfig.scale);
				roof.Join(beam);
			}
		}

		if (roofData.LocalExtend(E))
		{
			Shape wall = GeneratePart(slopedWall);
			wall.Scale(glm::vec3(1.0f, 0.6f, 1.0f));
			wall.Rotate90();
			wall.Move(glm::vec3(-slopedWallConfig.offset.z * 2.0f, 0.0f, 0.0f) * generationConfig.scale);

			if (!generationConfig.lod)
			{
				Shape beam = GeneratePart(PartType::slopedBeam, 2);
				beam.Rotate90(-1);
				beam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * generationConfig.scale);
				roof.Join(beam);

				beam = GeneratePart(PartType::slopedBeam, glm::vec3(0.75f, 1.3f, 0.75f), glm::vec3(0.05f, -0.035f, 0.075f), glm::vec3(0.0f, 180.0f, 0.0f));
				beam.Rotate90(-1);
				beam.Move(glm::vec3(0.0f, 0.275f, 0.0f) * generationConfig.scale);
				roof.Join(beam);
			}

			roof.Join(wall);
		}
	}
	else if (type == RoofType::cone)
	{
		roof = GeneratePart(PartType::coneRoof);
		float extendScale = 1.0f;
		float extendOffset = 0.0f;
		if (roofData.LocalExtend(N))
		{
			extendScale += 0.2f;
			extendOffset += 0.1f;
		}
		if (roofData.LocalExtend(S))
		{
			extendScale += 0.2f;
			extendOffset -= 0.1f;
		}
		roof.Scale(glm::vec3(1.0f, 1.0f, extendScale));
		roof.Move(glm::vec3(0.0f, 0.0f, extendOffset) * generationConfig.scale);

		if (roofData.LocalMerge(N))
		{
			extendScale += 0.4f;
			extendOffset += 0.2f;
		}
		if (roofData.LocalMerge(S))
		{
			extendScale += 0.4f;
			extendOffset -= 0.2f;
		}

		if (!generationConfig.lod)
		{
			Shape sideBeam = GeneratePart(PartType::beam, glm::vec3(1.0f, extendScale, 0.75f), glm::vec3(0.0f, 0.3125f, -beamConfig.offset.z + extendOffset), glm::vec3(0.0f, -90.0f, 0.0f));
			sideBeam.Scale(glm::vec3(1.0f, 1.5f, 1.0f));
			roof.Join(sideBeam);

			Shape midBeamLeft = GeneratePart(PartType::slightSlopedBeam, glm::vec3(0.75f, 1.35f, 0.75f), glm::vec3(-0.1f, -0.05f, -0.5f), glm::vec3(0.0f));
			midBeamLeft.Move(glm::vec3(-0.25f, 0.125f, 0.0f) * generationConfig.scale);
			roof.Join(midBeamLeft);

			Shape midBeamRight = GeneratePart(PartType::slightSlopedBeam, glm::vec3(0.75f, 1.35f, 0.75f), glm::vec3(0.1f, -0.05f, -0.5f), glm::vec3(0.0f, 180.0f, 0.0f));
			midBeamRight.Move(glm::vec3(0.25f, 0.125f, 0.0f) * generationConfig.scale);
			roof.Join(midBeamRight);
		}

		for (int d = 0; d <= 2; d += 2)
		{
			if (roofData.Extend((D)(d + roofData.direction)))
			{
				Shape wallLeft = GeneratePart(PartType::slopedLowWall);
				wallLeft.Scale(glm::vec3(0.5f, 0.3f, 1.0f));
				wallLeft.Move(glm::vec3(-0.25f, 0.0f, 0.0f) * generationConfig.scale);
				wallLeft.Rotate90(d);

				Shape wallRight = GeneratePart(PartType::slopedLowWall, 2);
				wallRight.Scale(glm::vec3(0.5f, 0.3f, 1.0f));
				wallRight.Move(glm::vec3(0.25f, 0.0f, 0.0f) * generationConfig.scale);
				wallRight.Rotate90(d);

				if (!generationConfig.lod)
				{
					Shape beamLeft = GeneratePart(PartType::slightSlopedBeam);
					beamLeft.Move(glm::vec3(-0.25f, 0.125f, 0.0f) * generationConfig.scale);
					beamLeft.Rotate90(d);
					roof.Join(beamLeft);

					Shape beamRight = GeneratePart(PartType::slightSlopedBeam, 2);
					beamRight.Move(glm::vec3(0.25f, 0.125f, 0.0f) * generationConfig.scale);
					beamRight.Rotate90(d);
					roof.Join(beamRight);

					beamLeft = GeneratePart(PartType::slightSlopedBeam, glm::vec3(0.75f, 1.35f, 0.75f), glm::vec3(-0.1f, -0.05f, 0.075f), glm::vec3(0.0f));
					beamLeft.Move(glm::vec3(-0.25f, 0.125f, 0.0f) * generationConfig.scale);
					beamLeft.Rotate90(d);
					roof.Join(beamLeft);

					beamRight = GeneratePart(PartType::slightSlopedBeam, glm::vec3(0.75f, 1.35f, 0.75f), glm::vec3(0.1f, -0.05f, 0.075f), glm::vec3(0.0f, 180.0f, 0.0f));
					beamRight.Move(glm::vec3(0.25f, 0.125f, 0.0f) * generationConfig.scale);
					beamRight.Rotate90(d);
					roof.Join(beamRight);
				}

				roof.Join(wallLeft);
				roof.Join(wallRight);
			}

			if (roofData.LocalMerge(N))
			{
				Shape extension = GeneratePart(PartType::coneRoofExtension);
				extension.Move(glm::vec3(0.0f, 0.0f, 0.5f) * generationConfig.scale);

				roof.Join(extension);
			}
			if (roofData.LocalMerge(S))
			{
				Shape extension = GeneratePart(PartType::coneRoofExtension);
				extension.Move(glm::vec3(0.0f, 0.0f, 0.5f) * generationConfig.scale);
				extension.Rotate90(2);
				//extension.InverseCoordinates(true, false);

				roof.Join(extension);
			}
		}
	}

	if (direction == S) roof.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == E) roof.Rotate(90.0f * (type == RoofType::cone ? 1.0f : -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	else if (direction == W) roof.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	roof.Move(glm::vec3(x, i + 1, y) * generationConfig.scale);

	if (generationConfig.lod) building->lodMesh.shape.Join(roof);
	else building->mesh.shape.Join(roof);
}

bool Buildings::CellValid(int i, int x, int y)
{
    return (i >= 0 && i < generationConfig.maxSize.y && x >= 0 && x < generationConfig.maxSize.x && 
		y >= 0 && y < generationConfig.maxSize.z);
}

bool Buildings::CellEmpty(int i, int x, int y, bool countBeams)
{
	return (!CellValid(i, x, y) || building->cells[i][x][y].Empty(countBeams));
}

bool Buildings::FloorEmpty(int i, int x, int y)
{
	return (!CellValid(i, x, y) || building->cells[i][x][y].floor.Empty());
}

void Buildings::UpdateBuilding(int id)
{
	buildings[id]->Update();

	/*for (int i = 0; i < buildings.size(); i++)
	{
		if (buildings[i]->id == id)
		{
			buildings[i]->Update();
			//std::cout << "building " << i << " id " << id << " height " << buildings[i]->position.y << std::endl;
			return;
		}
	}*/

	//std::cout << "building id " << id << std::endl;
}

bool Buildings::BuildingInView(Building *b)
{
	glm::vec3 worldSpace = b->position - Terrain::terrainOffset + glm::vec3(0.0f, float(b->size.y) * 0.5f, 0.0f);

	float dis = Utilities::DistanceSqrd(worldSpace, Manager::camera.Position());
	if (dis > 5000.0f * 5000.0f) return (false);
	float tolerance = 1.0 - (glm::clamp(dis, 0.0f, 1000.0f * 1000.0f) / (1000.0f * 1000.0f));
	if (dis < 50.0f * 50.0f || Manager::camera.InView(worldSpace, 2.5f * tolerance))
	{
		if (dis < 1000.0f * 1000.0f) b->lod = false;
		else b->lod = true;
		return (true);
	}
	

	/*float xOffset = float(b->size.x) * 0.5;
	float yOffset = float(b->size.y) * 0.5;
	float zOffset = float(b->size.z) * 0.5;

	for (int y = 0; y <= 2; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			for (int z = -1; z <= 1; z++)
			{
				glm::vec3 sampleOffset = glm::vec3(x * xOffset, y * yOffset, z * zOffset) * generationConfig.scale * 1.25f;
				sampleOffset = Utilities::RotateVec(sampleOffset, b->rotation.y, glm::vec3(0, 1, 0));
				glm::vec3 samplePosition = worldSpace + sampleOffset;
				
				if (Manager::camera.InView(samplePosition))
				{
					return (true);
				}
			}
		}
	}*/

	return (false);
}

std::vector<Texture> Buildings::beamTextures;
std::vector<Texture> Buildings::plasteredTextures;
std::vector<Texture> Buildings::reedTextures;
std::vector<Texture> Buildings::brickTextures;

Pipeline Buildings::graphicsPipeline{Manager::currentDevice, Manager::camera};
Pipeline Buildings::shadowPipeline{Manager::currentDevice, Manager::camera};

std::vector<BuildingBuffer> Buildings::buildingBuffers;
std::vector<BuildingBuffer> Buildings::buildingShadowBuffers;
std::vector<Buffer> Buildings::uniformBuffers;
std::vector<Buffer> Buildings::uniformShadowBuffers;

Descriptor Buildings::graphicsDescriptor{Manager::currentDevice};
Descriptor Buildings::shadowDescriptor{Manager::currentDevice};

bool Buildings::generating = false;

GenerationConfig Buildings::generationConfig;

int Buildings::maxRenderBuildings = 250;
int Buildings::currentActiveBuildings = 0;

//std::vector<Building> Buildings::buildings = std::vector<Building>(2500);
std::vector<Building *> Buildings::buildings;
Building *Buildings::building = nullptr;
std::vector<Building *> Buildings::renderBuildings = std::vector<Building *>(0);
std::vector<Building *> Buildings::renderBuildingsShadow = std::vector<Building *>(0);

Random Buildings::random;

PartConfig Buildings::floorConfig{"floor", glm::vec3(1.0f, 0.05f, 1.0f)};
PartConfig Buildings::foundationConfig{"foundation", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, -0.50f, 0.0f)};
PartConfig Buildings::flatWallConfig{"flat wall", glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.0f), glm::vec3(0.0f, 0.5f, 0.5f)};
PartConfig Buildings::windowedWallConfig{"windowed wall", glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.0f), glm::vec3(0.0f, 0.5f, 0.5f)};
PartConfig Buildings::dooredWallConfig{"doored wall", glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.0f), glm::vec3(0.0f, 0.5f, 0.5f)};
PartConfig Buildings::slopedWallConfig{"sloped wall", glm::vec3(1.0f, 1.6f, 0.05f), glm::vec3(0.0f), glm::vec3(-0.05f, 0.8f, 0.5f)};
//PartConfig Buildings::slopedLowWallConfig{"sloped low wall", glm::vec3(1.0f, 1.0f, 0.05f), glm::vec3(0.0f), glm::vec3(0.0f, 0.5f, 0.5f)};
PartConfig Buildings::slopedLowWallConfig{"sloped low wall", glm::vec3(1.0f, 1.6f, 0.05f), glm::vec3(0.0f), glm::vec3(0.0f, 0.8f, 0.5f)};
PartConfig Buildings::flatRoofConfig{"flat roof", glm::vec3(1.0f, 0.05f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)};
PartConfig Buildings::flatLowRoofConfig{"flat low roof", glm::vec3(1.0f, 0.05f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.675f, 0.0f)};
//PartConfig Buildings::slopedRoofConfig{"sloped roof", glm::vec3(1.0f, 0.05f, 1.5f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.3f, 0.1f)};
//PartConfig Buildings::slopedRoofConfig{"sloped roof", glm::vec3(1.0f, 0.1f, 1.25f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.3f, 0.0f)};
PartConfig Buildings::slopedRoofConfig{"sloped roof", glm::vec3(1.0f, 0.05f, 1.5f), glm::vec3(43.2f, 0.0f, 0.0f), glm::vec3(0.0f, 0.475f, 0.07f)};
PartConfig Buildings::slopedLowRoofConfig{"sloped low roof", glm::vec3(1.0f, 0.05f, 1.375f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.31f, 0.1f)};
PartConfig Buildings::coneRoofConfig{"cone roof", glm::vec3(0.815f, 0.05f, 1.0f), glm::vec3(43.2f, 0.0f, 0.0f), glm::vec3(-0.32f, 0.24f, 0.0f)};
PartConfig Buildings::coneLowRoofConfig{"cone low roof", glm::vec3(0.785f, 0.05f, 1.0f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(-0.35f, 0.15f, 0.0f)};
//PartConfig Buildings::coneRoofConfig{"cone roof", glm::vec3(0.635f, 0.1f, 1.0f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(-0.26f, 0.135f, 0.0f)};
//PartConfig Buildings::coneRoofConfig{"cone roof", glm::vec3(0.850f, 0.05f, 1.0f), glm::vec3(32.5f, 0.0f, 0.0f), glm::vec3(-0.375f, 0.125f, 0.0f)};
//PartConfig Buildings::coneRoofExtensionConfig{"cone roof extension", glm::vec3(0.5f, 0.635f, 0.1f), glm::vec3(122.5f, 0.0f, 0.0f), glm::vec3(-0.26f, 0.135f, 0.250f)};
//PartConfig Buildings::coneRoofExtensionConfig{"cone roof extension", glm::vec3(0.5f, 0.785f, 0.05f), glm::vec3(122.5f, 0.0f, 0.0f), glm::vec3(-0.35f, 0.15f, 0.250f)};
PartConfig Buildings::coneRoofExtensionConfig{"cone roof extension", glm::vec3(0.5f, 0.785f, 0.05f), glm::vec3(133.3f, 0.0f, 0.0f), glm::vec3(-0.31f, 0.25f, 0.250f)};
//PartConfig Buildings::beamConfig{"beam", glm::vec3(1.1f, 0.15f, 0.15f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f)};
PartConfig Buildings::beamConfig{"beam", glm::vec3(0.1f, 1.1f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f)};
PartConfig Buildings::slopedBeamConfig{"sloped beam", glm::vec3(0.1f, 1.25f, 0.1f), glm::vec3(-46.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.2f, 0.5f)};
//PartConfig Buildings::slopedBeamConfig{"sloped beam", glm::vec3(0.1f, 1.15f, 0.1f), glm::vec3(-57.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.025f, 0.5f)};
//PartConfig Buildings::slightSlopedBeamConfig{"slight sloped beam", glm::vec3(0.1f, 0.6f, 0.1f), glm::vec3(-57.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.5f)};
PartConfig Buildings::slightSlopedBeamConfig{"slight sloped beam", glm::vec3(0.1f, 0.6f, 0.1f), glm::vec3(-46.8f, 0.0f, 0.0f), glm::vec3(0.0f, 0.07f, 0.5f)};
PartConfig Buildings::collumnConfig{"collumn", glm::vec3(0.1f, 1.0f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.499f, 0.5f, 0.499f)};