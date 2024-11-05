#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>

class Cinematic
{
private:
	int positionIndex = 0;
	int rotationIndex = 0;

	float positionDuration = 0;
	float rotationDuration = 0;

	float totalFrameTime = 0;
	int totalFrames = 0;

	float lastKeyTime = 0;

public:
	struct CinematicKey
	{
		glm::vec3 value;
		float duration;
	};

	Cinematic();
	~Cinematic();

	bool running = false;
	bool creating = false;
	float speed = 1;

	std::string name = "";

	std::vector<CinematicKey> keyPositions = std::vector<CinematicKey>();
	std::vector<CinematicKey> keyRotations = std::vector<CinematicKey>();

	void Load(const char *path);
	void Create(const char *path);

	void AddKeyPosition(glm::vec3 value, float duration);
	void AddKeyRotation(glm::vec3 value, float duration);
	void AddKey(glm::vec3 position, glm::vec3 rotation, float duration = -1);

	void Start();
	void Play();
	void Stop();
	void Pause();
	void Resume();

	bool Valid();
};