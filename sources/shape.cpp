#include "shape.hpp"

#include "utilities.hpp"

Shape::Shape()
{

}

Shape::Shape(int type)
{
    SetShape(type);
}

Shape::Shape(int type, int resolution)
{
    SetShape(type, resolution);
}

Shape::~Shape()
{

}

void Shape::SetShape(int type, int resolution)
{
	createResolution = resolution;

	if (type == QUAD)
	{
		AddPosition(glm::vec3(-0.5f, -0.5f, 0.0f));
		AddPosition(glm::vec3(0.5f, 0.5f, 0.0f));
		AddPosition(glm::vec3(0.5f, -0.5f, 0.0f));
		AddPosition(glm::vec3(-0.5f, 0.5f, 0.0f));

		AddIndice(0);
		AddIndice(1);
		AddIndice(2);

		AddIndice(3);
		AddIndice(1);
		AddIndice(0);

		if (coordinate)
		{
			AddCoordinate(glm::vec2(0.0f, 0.0f));
			AddCoordinate(glm::vec2(1.0f, 1.0f));
			AddCoordinate(glm::vec2(1.0f, 0.0f));
			AddCoordinate(glm::vec2(0.0f, 1.0f));
		}
	}
	else if (type == LEAF)
	{
		//normal = true;
		coordinate = false;

		AddPosition(glm::vec3(-0.25f, -0.25f, 0.0f));
		AddPosition(glm::vec3(0.25f, 0.25f, 0.0f));
		AddPosition(glm::vec3(0.5f, -0.5f, 0.0f));
		AddPosition(glm::vec3(-0.5f, 0.5f, 0.0f));

		Rotate(45.0f, glm::vec3(0, 0, 1));

		AddIndice(0);
		AddIndice(1);
		AddIndice(2);

		AddIndice(3);
		AddIndice(1);
		AddIndice(0);

		if (coordinate)
		{
			AddCoordinate(glm::vec2(0.0f, 0.0f));
			AddCoordinate(glm::vec2(1.0f, 1.0f));
			AddCoordinate(glm::vec2(1.0f, 0.0f));
			AddCoordinate(glm::vec2(0.0f, 1.0f));
		}
	}
	else if (type == CUBE)
    {
        Shape front = Shape(QUAD);
        front.Move(glm::vec3(0.0f, 0.0f, -0.5f));

        Shape back = Shape(QUAD);
        back.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        back.Move(glm::vec3(0.0f, 0.0f, 0.5f));

        Shape right = Shape(QUAD);
        right.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        right.Move(glm::vec3(0.5f, 0.0f, 0.0f));

        Shape left = Shape(QUAD);
        left.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        left.Move(glm::vec3(-0.5f, 0.0f, 0.0f));

        Shape top = Shape(QUAD);
        top.Rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        top.Move(glm::vec3(0.0f, 0.5f, 0.0f));

        Shape bottom = Shape(QUAD);
        bottom.Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        bottom.Move(glm::vec3(0.0f, -0.5f, 0.0f));

        Join(front);
        Join(back);
        Join(right);
        Join(left);
        Join(top);
        Join(bottom);
    }
    else if (type == PLANE)
    {
        int sideVertCount = resolution;
        float halfLength = 0.5f;

        for (int i = 0, x = 0; x <= sideVertCount; ++x)
        {
            for (int z = 0; z <= sideVertCount; ++z, ++i)
            {
                positions.push_back(glm::vec3(((float)x / sideVertCount) - halfLength, 0, ((float)z / sideVertCount) - halfLength));
            }
        }

        for (int ti = 0, vi = 0, x = 0; x < sideVertCount; ++vi, ++x)
        {
            for (int z = 0; z < sideVertCount; ti += 6, ++vi, ++z)
            {
                indices.push_back(vi);
                indices.push_back(vi + 1);
                indices.push_back(vi + sideVertCount + 2);
                indices.push_back(vi);
                indices.push_back(vi + sideVertCount + 2);
                indices.push_back(vi + sideVertCount + 1);
            }
        }
    }
	else if (type == BLADE)
	{
		int layer = 1;
		int subLayers = resolution;
		const float BLADE_WIDTH = 0.05f;
		const float BLADE_HEIGHT = 1.0 / (subLayers + 2);

		positions.push_back(glm::vec3(-BLADE_WIDTH, layer * BLADE_HEIGHT, 0.0f));
		positions.push_back(glm::vec3(BLADE_WIDTH, 0, 0.0f));
		positions.push_back(glm::vec3(-BLADE_WIDTH, 0, 0.0f));
		positions.push_back(glm::vec3(BLADE_WIDTH, layer * BLADE_HEIGHT, 0.0f));

		for (int i = 0; i < subLayers; i++)
		{
			layer++;
			positions.push_back(glm::vec3(-BLADE_WIDTH, layer * BLADE_HEIGHT, 0.0f));
			positions.push_back(glm::vec3(BLADE_WIDTH, layer * BLADE_HEIGHT, 0.0f));
		}

		layer++;
		positions.push_back(glm::vec3(0.0f, layer * BLADE_HEIGHT, 0.0f));

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(0);
		indices.push_back(3);
		indices.push_back(1);

		int max = 3;
		int top = 0;
		for (int i = 0; i < subLayers; i++)
		{
			indices.push_back(max + 1);
			indices.push_back(max);
			indices.push_back(top);
			indices.push_back(max + 1);
			indices.push_back(max + 2);
			indices.push_back(max);
			top = max + 1;
			max = max + 2;
		}

		indices.push_back(max + 1);
		indices.push_back(max);
		indices.push_back(top);
	}
	else if (type == CYLINDER)
	{
		normal = true;
		coordinate = true;

		Shape plane = Shape(PLANE, resolution);
		plane.Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		Join(plane);

		std::vector<unsigned int> startVertices;
		std::vector<unsigned int> endVertices;

		int index = 0;

		for (glm::vec3 &position : positions)
		{
			float x = round((position.x + 0.5) * (resolution - 1));
			float y = round((position.y + 0.5) * (resolution - 1));

			if (x == 0) startVertices.push_back(index);
			else if (x == (resolution - 1)) endVertices.push_back(index);

			if (y == 0) mergeBottomPoints.push_back(index);
			else if (y == (resolution - 1)) mergeTopPoints.push_back(index);

			glm::vec3 newPosition = glm::normalize(Utilities::RotateVec(glm::vec3(0, 0, -1), (360.0 / resolution) * -x, glm::vec3(0, 1, 0)));

			position.x = newPosition.x;
			position.z = newPosition.z;

			index++;
		}

		for (int i = 0; i < startVertices.size() - 1; i++)
		{
			indices.push_back(endVertices[i + 1]);
			indices.push_back(startVertices[i]);
			indices.push_back(endVertices[i]);

			indices.push_back(endVertices[i + 1]);
			indices.push_back(startVertices[i + 1]);
			indices.push_back(startVertices[i]);
		}

		for (int i = 0; i < mergeTopPoints.size(); i++)
		{
			pointMerged.push_back(glm::ivec2(0));
		}

		if (normal) RecalculateNormals();

		if (resolution > 4)
		{
			centerMergePoint = positions.size();
			positions.push_back(TopMergePointsCenter() + glm::vec3(0, 0.05, 0));
			if (normal) normals.push_back(glm::vec3(0, 1, 0));
			if (coordinate) coordinates.push_back(glm::vec2(1));
			//coordinates.push_back(glm::vec2(0, 1));
		}

		Move(glm::vec3(0, 0.5, 0));
		Scale(glm::vec3(1, 15, 1));

		if (coordinate) RecalculateCoordinates();
	}
}

void Shape::AddPosition(glm::vec3 pos)
{
	positions.push_back(pos);
}

void Shape::AddCoordinate(glm::vec2 uv)
{
	if (!coordinate) return;

	coordinates.push_back(uv);
}

void Shape::AddIndice(indexType index)
{
	indices.push_back(index);
}

void Shape::Join(Shape &joinShape)
{
	//int count = glm::max(positions.size(), coordinates.size());
	int count = positions.size();

	for (glm::vec3 pos : joinShape.positions)
    {
        positions.push_back(pos);
    }

	for (indexType index : joinShape.indices)
	{
		indices.push_back(index + count);
	}

	if (coordinate)
	{
		for (glm::vec2 coord : joinShape.coordinates)
    	{
    	    coordinates.push_back(coord);
    	}
	}

	if (normal)
	{
		for (glm::vec3 normal : joinShape.normals)
    	{
    	    normals.push_back(normal);
    	}
	}
}

int CalculateIndex(int i)
{
	int index = 0;

	if (i % 2 == 0)
	{
		index += i / 2;
	}
	else
	{
		index -= i / 2 + 1;
	}

	return (index);
}

void Shape::Merge(Shape &joinShape, int mainBlendRange, int joinBlendRange)
{
	int count = positions.size();
	int joinCount = joinShape.mergeBottomPoints.size();
	int mainCount = mergeTopPoints.size();
	int minCount = glm::min(joinCount, mainCount);

	int closestMainPoint = ClosestMergeIndex(joinShape.BottomMergePointsCenter(), true, true);
	int furthestJoinPoint = joinShape.ClosestMergeIndex(TopMergePointsCenter(), false, false);

	for (int i = 0; i < minCount; i++)
	{
		int index = CalculateIndex(i);

		int i1 = index + furthestJoinPoint;
		int i2 = index + 1 + furthestJoinPoint;

		if (i1 >= joinCount) i1 -= joinCount;
		if (i2 >= joinCount) i2 -= joinCount;
		if (i1 < 0) i1 += joinCount;
		if (i2 < 0) i2 += joinCount;

		int mi1 = index + closestMainPoint;
		int mi2 = index + 1 + closestMainPoint;

		if (mi1 >= mainCount) mi1 -= mainCount;
		if (mi1 < 0) mi1 += mainCount;
		if (mi2 >= mainCount) mi2 -= mainCount;
		if (mi2 < 0) mi2 += mainCount;

		indices.push_back(joinShape.mergeBottomPoints[i1] + count);
		indices.push_back(mergeTopPoints[mi2]);
		indices.push_back(mergeTopPoints[mi1]);

		indices.push_back(joinShape.mergeBottomPoints[i1] + count);
		indices.push_back(joinShape.mergeBottomPoints[i2] + count);
		indices.push_back(mergeTopPoints[mi2]);

		int mergeJoinRange = glm::clamp(joinBlendRange, 1, joinShape.createResolution);
		int mergeMainRange = glm::clamp(mainBlendRange, 0, createResolution);
		float mergeAmount = 1.0 / ((mergeJoinRange + mergeMainRange) + 1);

		glm::vec3 direction = joinShape.positions[joinShape.mergeBottomPoints[i1]] - positions[mergeTopPoints[mi1]];
		glm::ivec2 joinCoords = joinShape.GetPositionCoordinates(joinShape.mergeBottomPoints[i1]);
		glm::ivec2 mainCoords = GetPositionCoordinates(mergeTopPoints[mi1]);

		for (int j = 0; j < mergeJoinRange; j++)
		{
			float mergeStrength = mergeJoinRange - j;
			float mergeFalloff = pow(0.9, j);
			int mergeIndex = joinShape.GetPositionIndex(joinCoords.x + j, joinCoords.y);
			joinShape.positions[mergeIndex] -= direction * (mergeStrength * mergeAmount * mergeFalloff);
		}

		bool canBlend = true;
		for (unsigned int blendedIndex : pointBlended)
		{
			if (blendedIndex == mergeTopPoints[mi1])
			{
				canBlend = false;
				break;
			}
		}

		if (canBlend)
		{
			for (int j = 0; j < mergeMainRange; j++)
			{
				float mergeStrength = mergeMainRange - j;
				float mergeFalloff = pow(0.9, j);
				int mergeIndex = GetPositionIndex(mainCoords.x - j, mainCoords.y);
				positions[mergeIndex] += direction * (mergeStrength * mergeAmount * mergeFalloff);
				if (mainBlendRange > 0) RecalculateNormal(mergeIndex);
			}
			pointBlended.push_back(mergeTopPoints[mi1]);
		}

		pointMerged[mi1].x = 1;
		pointMerged[mi2].y = 1;
	}

	if (centerMergePoint != -1)
	{
		int index = CalculateIndex(minCount - 1);
		int i1 = index + 1 + furthestJoinPoint;
		if (i1 >= joinCount) i1 -= joinCount;
		if (i1 < 0) i1 += joinCount;

		int mi1 = index + 1 + closestMainPoint;
		if (mi1 >= mainCount) mi1 -= mainCount;
		if (mi1 < 0) mi1 += mainCount;

		indices.push_back(joinShape.mergeBottomPoints[i1] + count);
		indices.push_back(centerMergePoint);
		indices.push_back(mergeTopPoints[mi1]);

		index = CalculateIndex(minCount - 2);

		mi1 = index + closestMainPoint;
		if (mi1 >= mainCount) mi1 -= mainCount;
		if (mi1 < 0) mi1 += mainCount;

		indices.push_back(joinShape.mergeBottomPoints[i1] + count);
		indices.push_back(mergeTopPoints[mi1]);
		indices.push_back(centerMergePoint);
	}

	Join(joinShape);
}

void Shape::Move(glm::vec3 movement)
{
	for (glm::vec3 &pos : positions)
	{
		pos += movement;
	}
}

void Shape::Rotate(float degrees, glm::vec3 axis)
{
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(degrees), axis);

    for (glm::vec3 &pos : positions)
	{
		pos = rotation * glm::vec4(pos, 1.0f);
	}

	for (glm::vec3 &norm : normals)
	{
		norm = rotation * glm::vec4(norm, 0.0f);
	}
}

void Shape::Scale(glm::vec3 scale)
{
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	scaleMatrix = glm::scale(scaleMatrix, scale);

	for (glm::vec3 &pos : positions)
	{
		pos = scaleMatrix * glm::vec4(pos, 1.0f);
	}
}

void Shape::RecalculateNormals()
{
	normals.clear();

	for (const glm::vec3 &position : positions)
	{
		normals.push_back(glm::normalize(glm::vec3(position.x, 0, position.z)));
	}
}

void Shape::RecalculateCoordinates()
{
	coordinates.clear();

	for (const glm::vec3 &position : positions)
	{
		coordinates.push_back(glm::vec2(position.x, position.y));
	}
}

void Shape::RecalculateNormal(unsigned int index)
{
	normals[index] = glm::normalize(glm::vec3(positions[index].x, 0, positions[index].z));
}

glm::vec3 Shape::BottomMergePointsCenter()
{
	glm::vec3 center = glm::vec3(0);

	for (const unsigned int &i : mergeBottomPoints)
	{
		center += positions[i];
	}

	center /= mergeBottomPoints.size();

	return (center);
}

glm::vec3 Shape::TopMergePointsCenter()
{
	glm::vec3 center = glm::vec3(0);

	for (const unsigned int &i : mergeTopPoints)
	{
		center += positions[i];
	}

	center /= mergeTopPoints.size();

	return (center);
}

int Shape::GetPositionIndex(int x, int y)
{
	int root = createResolution + 1;

	return (x + y * root);
}

glm::ivec2 Shape::GetPositionCoordinates(int i)
{
	int root = createResolution + 1;

	glm::ivec2 coords = glm::ivec2(0);
	coords.y = i / root;
	coords.x = i % root;

	return (coords);
}

int Shape::ClosestMergeIndex(glm::vec3 position, bool closest, bool top)
{
	int index = 0;
	int closestIndex = 0;
	float closestDistance = 0;

	if (top) closestDistance = glm::distance(position, positions[mergeTopPoints[0]]);
	else closestDistance = glm::distance(position, positions[mergeBottomPoints[0]]);

	for (const int &i : (top ? mergeTopPoints : mergeBottomPoints))
	{
		float currentDistance = glm::distance(position, positions[i]);
		if (closest)
		{
			if (currentDistance < closestDistance)
			{
				closestDistance = currentDistance;
				closestIndex = index;
			}
		}
		else
		{
			if (currentDistance > closestDistance)
			{
				closestDistance = currentDistance;
				closestIndex = index;
			}
		}
		index++;
	}

	return (closestIndex);
}

void Shape::CloseUnusedPoints()
{
	if (centerMergePoint == -1) return;

	glm::vec3 center = TopMergePointsCenter();

	int i = 0;
	for (const unsigned int &point : mergeTopPoints)
	{
		if (pointMerged[i].x == 0)
		{
			int mi1 = i;
			int mi2 = i + 1;
			if (mi2 >= mergeTopPoints.size()) mi2 -= mergeTopPoints.size();

			indices.push_back(centerMergePoint);
			indices.push_back(mergeTopPoints[mi2]);
			indices.push_back(mergeTopPoints[mi1]);
		}
		i++;
	}
}