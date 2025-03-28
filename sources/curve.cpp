#include "curve.hpp"

Curve::Curve()
{

}

Curve::Curve(int pointCount, int type) 
{
	this->pointCount = pointCount;
	points.resize(pointCount);

	for (int i = 0; i < pointCount; i++)
	{
		float value = float(i) / float(pointCount - 1);
		if (type == EXPONENT) value *= value;
		SetPoint(i, value);
	}
}

Curve::~Curve()
{

}

float Curve::GetPoint(int index)
{
	if (index < 0 || index >= pointCount)
		return (0.0f);

	return (points[index]);
}

void Curve::SetPoint(int index, float value)
{
	if (index < 0 || index >= pointCount) return;

	points[index] = glm::clamp(value, 0.0f, 1.0f);
}

float Curve::Evaluate(float t)
{
	if (pointCount == 0) return (0.0f);

	t = glm::clamp(t, 0.0f, 1.0f);

	float inter = t * (pointCount - 1);

	int a = glm::clamp(int(glm::floor(inter)), 0, pointCount);
	int b = glm::clamp(int(glm::ceil(inter)), 0, pointCount);
	float c = glm::clamp(inter - a, 0.0f, 1.0f);

	float result = glm::mix(points[a], points[b], c);

	return (result);
}