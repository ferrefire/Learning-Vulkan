#include "cinematic.hpp"
#include "manager.hpp"
#include "time.hpp"
#include "terrain.hpp"
#include "utilities.hpp"
#include "input.hpp"

#include <fstream>
#include <iostream>

Cinematic::Cinematic()
{

}

Cinematic::~Cinematic()
{

}

void Cinematic::Load(const char *path)
{
	std::string positionsString = "#key_positions";
	std::string rotationsString = "#key_rotations";
	std::string lightsString = "#key_lights";
	std::string endString = "#end";
	std::string fileString = Utilities::FileToString(path);

	size_t rotationsPosition = fileString.find(rotationsString);
	size_t lightsPosition = fileString.find(lightsString);
	size_t endPosition = fileString.find(endString);
	size_t filePosition = fileString.find(positionsString);
	size_t dividerPosition;

	if (lightsPosition == std::string::npos) lightsPosition = endPosition;

	filePosition = fileString.find("<");

	while (filePosition != std::string::npos && filePosition < rotationsPosition)
	{
		glm::vec3 newKeyPosition;
		float newKeyDuration;

		filePosition++;
		dividerPosition = fileString.find(",", filePosition);
		newKeyPosition.x = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = dividerPosition + 1;
		dividerPosition = fileString.find(",", filePosition);
		newKeyPosition.y = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = dividerPosition + 1;
		dividerPosition = fileString.find(",", filePosition);
		newKeyPosition.z = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = dividerPosition + 1;
		dividerPosition = fileString.find(">", filePosition);
		newKeyDuration = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = fileString.find("<", filePosition);

		// Utilities::PrintVec3(newKeyPosition);
		AddKeyPosition(newKeyPosition, newKeyDuration);
	}

	while (filePosition != std::string::npos && filePosition < lightsPosition)
	{
		glm::vec3 newKeyRotation;
		float newKeyDuration;

		filePosition++;
		dividerPosition = fileString.find(",", filePosition);
		newKeyRotation.x = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = dividerPosition + 1;
		dividerPosition = fileString.find(",", filePosition);
		newKeyRotation.y = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = dividerPosition + 1;
		dividerPosition = fileString.find(",", filePosition);
		newKeyRotation.z = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = dividerPosition + 1;
		dividerPosition = fileString.find(">", filePosition);
		newKeyDuration = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = fileString.find("<", filePosition);

		// Utilities::PrintVec3(newKeyRotation);
		AddKeyRotation(newKeyRotation, newKeyDuration);
	}

	while (filePosition != std::string::npos && filePosition < endPosition)
	{
		glm::vec3 newKeyLight;
		float newKeyDuration;

		filePosition++;
		dividerPosition = fileString.find(",", filePosition);
		newKeyLight.x = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = dividerPosition + 1;
		dividerPosition = fileString.find(",", filePosition);
		newKeyLight.y = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = dividerPosition + 1;
		dividerPosition = fileString.find(",", filePosition);
		newKeyLight.z = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = dividerPosition + 1;
		dividerPosition = fileString.find(">", filePosition);
		newKeyDuration = std::stof(fileString.substr(filePosition, dividerPosition));
		filePosition = fileString.find("<", filePosition);

		// Utilities::PrintVec3(newKeyLight);
		AddKeyLight(newKeyLight, newKeyDuration);
	}
}

void Cinematic::Create(const char *path)
{
	std::string ps;

	ps.append("#key_positions\n");
	for (CinematicKey &key : keyPositions)
	{
		ps.append("<" + std::to_string(key.value.x) + "," + std::to_string(key.value.y) + "," + std::to_string(key.value.z) + "," + std::to_string(key.duration) + ">" + "\n");
	}
	ps.append("#key_rotations\n");
	for (CinematicKey &key : keyRotations)
	{
		ps.append("<" + std::to_string(key.value.x) + "," + std::to_string(key.value.y) + "," + std::to_string(key.value.z) + "," + std::to_string(key.duration) + ">" + "\n");
	}
	if (keyLights.size() > 0)
	{
		ps.append("#key_lights\n");
		for (CinematicKey &key : keyLights)
		{
			ps.append("<" + std::to_string(key.value.x) + "," + std::to_string(key.value.y) + "," + std::to_string(key.value.z) + "," + std::to_string(key.duration) + ">" + "\n");
		}
	}
	ps.append("#end\n");

	std::ofstream newCinFile(path);
	newCinFile << ps;
	newCinFile.close();
}

void Cinematic::AddKeyPosition(glm::vec3 value, float duration)
{
	CinematicKey key;
	key.value = value;
	key.duration = duration;

	keyPositions.push_back(key);
}

void Cinematic::AddKeyRotation(glm::vec3 value, float duration)
{
	CinematicKey key;
	key.value = value;
	key.duration = duration;

	keyRotations.push_back(key);
}

void Cinematic::AddKeyLight(glm::vec3 value, float duration)
{
	CinematicKey key;
	key.value = value;
	key.duration = duration;

	keyLights.push_back(key);
}

void Cinematic::AddKey(glm::vec3 position, glm::vec3 rotation, float duration)
{
	float currentTime = Time::GetCurrentTime();
	if (duration == -1) duration = currentTime - lastKeyTime;
	lastKeyTime = currentTime;

	if (keyPositions.size() == 0 || keyRotations.size() == 0 || keyLights.size() == 0) duration = 0;

	AddKeyPosition(position + glm::vec3(Terrain::terrainOffset.x, 0, Terrain::terrainOffset.y), duration);
	AddKeyRotation(rotation, duration);
	AddKeyLight(Manager::lightAngles, duration);

	//if (Manager::lightAngles != lastLight || keyLights.size() == 0)
	//{
	//	lastLight = Manager::lightAngles;
	//	if (keyLights.size() == 0) duration = 0;
	//	AddKeyLight(Manager::lightAngles, duration);
	//}
}

void Cinematic::Start()
{
	if (!Valid()) return;

	Manager::camera.canMove = false;
	Manager::camera.canLook = false;
	//Manager::EnableVsync(false);
	running = true;
}

void Cinematic::Play()
{
	if (!Valid()) return;

	bool ended = true;

	if (positionIndex + 1 < keyPositions.size())
	{
		Manager::camera.SetPosition(glm::mix(keyPositions[positionIndex].value, keyPositions[positionIndex + 1].value,
			positionDuration / keyPositions[positionIndex + 1].duration) -
			glm::vec3(Terrain::terrainOffset.x, 0, Terrain::terrainOffset.y));

		positionDuration += Time::deltaTime * speed;

		if (positionDuration >= keyPositions[positionIndex + 1].duration)
		{
			positionIndex++;
			positionDuration = 0;
		}

		ended = false;
	}

	if (rotationIndex + 1 < keyRotations.size())
	{
		Manager::camera.SetRotation(glm::mix(keyRotations[rotationIndex].value, keyRotations[rotationIndex + 1].value,
			rotationDuration / keyRotations[rotationIndex + 1].duration));

		rotationDuration += Time::deltaTime * speed;

		if (rotationDuration >= keyRotations[rotationIndex + 1].duration)
		{
			rotationIndex++;
			rotationDuration = 0;
		}

		ended = false;
	}

	if (lightIndex + 1 < keyLights.size())
	{
		if (keyLights[lightIndex].value != keyLights[lightIndex + 1].value)
		{
			Manager::lightAngles = glm::mix(keyLights[lightIndex].value, keyLights[lightIndex + 1].value,
				lightDuration / keyLights[lightIndex + 1].duration);
			Terrain::updateTerrainShadows = true;
		}

		lightDuration += Time::deltaTime * speed;

		if (lightDuration >= keyLights[lightIndex + 1].duration)
		{
			lightIndex++;
			lightDuration = 0;
		}

		ended = false;
	}

	if (running && ended)
	{
		Stop();
	}
	else
	{
		totalFrames++;
		totalFrameTime += Time::deltaTime;
	}
}

void Cinematic::Stop()
{
	if (!Valid()) return;

	running = false;
	float fps = float(totalFrames) / totalFrameTime;
	std::cout << fps << std::endl;
	Manager::camera.canMove = true;
	Manager::camera.canLook = true;
	//Manager::EnableVsync(true);
}

void Cinematic::Pause()
{
}

void Cinematic::Resume()
{
}

bool Cinematic::Valid()
{
	return (keyPositions.size() > 1 || keyRotations.size() > 1);
}