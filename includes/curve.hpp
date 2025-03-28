#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define LINEAR 0
#define EXPONENT 1

class Curve
{
	private:
		std::vector<float> points;
		int pointCount = 0;

	public:
		Curve();
		Curve(int pointCount, int type);
		~Curve();

		//void AddPoint(float point);
		float GetPoint(int index);
		void SetPoint(int index, float value);

		float Evaluate(float t);
};