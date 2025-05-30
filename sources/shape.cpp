#include "shape.hpp"

#include "utilities.hpp"

Shape::Shape()
{

}

Shape::Shape(int type)
{
    SetShape(type);
}

Shape::Shape(int type, bool coordinate, bool normal, bool color)
{
	this->coordinate = coordinate;
	this->normal = normal;
	this->color = color;
    SetShape(type);
}

Shape::Shape(int type, int resolution)
{
    SetShape(type, resolution);
}

Shape::~Shape()
{

}

void Shape::Clear()
{
	positions.clear();
	coordinates.clear();
	normals.clear();
	colors.clear();

	mergeTopPoints.clear();
	mergeBottomPoints.clear();
	pointBlended.clear();
	pointMerged.clear();

	centerMergePoint = -1;
	createResolution = -1;

	coordinate = false;
	normal = false;
	color = false;
}

void Shape::SetShape(int type, int resolution)
{
	createResolution = resolution;

	if (type == QUAD)
	{
		AddPosition(glm::vec3(-0.5f, -0.5f, 0.0f));
		AddPosition(glm::vec3(0.5f, 0.5f, 0.0f));
		AddPosition(glm::vec3(-0.5f, 0.5f, 0.0f));
		AddPosition(glm::vec3(0.5f, -0.5f, 0.0f));

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
			AddCoordinate(glm::vec2(0.0f, 1.0f));
			AddCoordinate(glm::vec2(1.0f, 0.0f));
		}

		if (normal)
		{
			AddNormal(glm::vec3(0.0f, 0.0f, 1.0f));
			AddNormal(glm::vec3(0.0f, 0.0f, 1.0f));
			AddNormal(glm::vec3(0.0f, 0.0f, 1.0f));
			AddNormal(glm::vec3(0.0f, 0.0f, 1.0f));
		}

		if (color)
		{
			AddColor(glm::vec3(0.0f, 0.0f, 0.0f));
			AddColor(glm::vec3(0.0f, 0.0f, 0.0f));
			AddColor(glm::vec3(0.0f, 0.0f, 0.0f));
			AddColor(glm::vec3(0.0f, 0.0f, 0.0f));
		}
	}
	else if (type == LEAF)
	{
		//normal = true;
		coordinate = false;

		if (resolution == 0)
		{
			float angleStep = 360.0f / 3.0f;

			for (int i = 0; i < 3; i++)
			{
				glm::vec3 newPosition = Utilities::RotateVec(glm::vec3(-0.5f, -0.5f, 0.0f), i * angleStep, glm::vec3(0, 0, 1));
				AddPosition(newPosition);
			}

			//Rotate(45.0f, glm::vec3(0, 0, 1));

			AddIndice(0);
			AddIndice(2);
			AddIndice(1);

			Rotate(90.0f, glm::vec3(1, 0, 0));
		}

		if (resolution == 1)
		{
			float height = 0.25f;
			float width = 0.5f;

			AddPosition(glm::vec3(-height, -height, 0.0f));
			AddPosition(glm::vec3(height, height, 0.0f));
			AddPosition(glm::vec3(width, -width, 0.0f));
			AddPosition(glm::vec3(-width, width, 0.0f));

			Rotate(45.0f, glm::vec3(0, 0, 1));

			AddIndice(0);
			AddIndice(1);
			AddIndice(2);

			AddIndice(3);
			AddIndice(1);
			AddIndice(0);

			Rotate(90.0f, glm::vec3(1, 0, 0));
		}

		/*if (resolution == 0)
		{
			float angleStep = 360.0f / 4.0f;

			for (int i = 0; i < 4; i++)
			{
				glm::vec3 newPosition = Utilities::RotateVec(glm::vec3(-0.5f, -0.5f, 0.0f), i * angleStep, glm::vec3(0, 0, 1));
				AddPosition(newPosition);
			}

			Rotate(45.0f, glm::vec3(0, 0, 1));
			Scale(glm::vec3(1.0f, 0.5f, 1.0f));
			Rotate(-45.0f, glm::vec3(0, 0, 1));

			AddIndice(0);
			AddIndice(2);
			AddIndice(1);

			AddIndice(3);
			AddIndice(2);
			AddIndice(0);

			Rotate(90.0f, glm::vec3(1, 0, 0));
		}*/

		if (resolution == 2)
		{
			float angleStep = 360.0f / 3.0f;

			AddPosition(glm::vec3(0));

			for (int i = 0; i < 3; i++)
			{
				glm::vec3 newPosition = Utilities::RotateVec(glm::vec3(-0.5f, -0.5f, 0.0f), i * angleStep, glm::vec3(0, 0, 1));
				AddPosition(newPosition);
			}

			for (int i = 0; i < 3; i++)
			{
				int base = i + 1;
				int start = base + 1;
				int end = base - 1;
				if (start > 3) start = 1;
				if (end < 1) end = 3;

				glm::vec3 point0 = (positions[base] + positions[0]) * 0.5f;
				glm::vec3 point1 = (positions[base] + positions[start]) * 0.5f;
				glm::vec3 newPosition = (point0 + point1) * 0.5f;
				AddPosition(newPosition);

				point1 = (positions[base] + positions[end]) * 0.5f;
				newPosition = (point0 + point1) * 0.5f;
				AddPosition(newPosition);
			}

			for (int i = 0; i < 3; i++)
			{
				int index = i * 2 + 4;

				AddIndice(0);
				AddIndice(index);
				AddIndice(index + 1);
			}

			for (int i = 0; i < 3; i++)
			{
				int index = i * 2 + 4;

				AddIndice(i + 1);
				AddIndice(index + 1);
				AddIndice(index);
			}
	
			Rotate(90.0f, glm::vec3(1, 0, 0));
		}

		/*if (resolution == 1)
		{
			float angleStep = 360.0f / 3.0f;

			for (int i = 0; i < 3; i++)
			{
				glm::vec3 newPosition = Utilities::RotateVec(glm::vec3(-0.5f, -0.5f, 0.0f), i * angleStep, glm::vec3(0, 0, 1));
				AddPosition(newPosition);
			}

			glm::vec3 newPosition = (positions[0] + positions[1]) * 0.5f;
			newPosition *= 0.5f;
			AddPosition(newPosition);

			newPosition = (positions[1] + positions[2]) * 0.5f;
			newPosition *= 0.5f;
			AddPosition(newPosition);

			newPosition = (positions[2] + positions[0]) * 0.5f;
			newPosition *= 0.5f;
			AddPosition(newPosition);
	
			AddIndice(0);
			AddIndice(5);
			AddIndice(3);

			AddIndice(1);
			AddIndice(3);
			AddIndice(4);

			AddIndice(2);
			AddIndice(4);
			AddIndice(5);

			AddIndice(5);
			AddIndice(4);
			AddIndice(3);
	
			Rotate(90.0f, glm::vec3(1, 0, 0));
		}*/

		/*if (resolution == 1)
		{
			float angleStep = 360.0f / 6.0f;

			for (int i = 0; i < 6; i++)
			{
				glm::vec3 newPosition = Utilities::RotateVec(glm::vec3(-0.5f, -0.5f, 0.0f), i * angleStep, glm::vec3(0, 0, 1));
				AddPosition(newPosition);
			}
	
			Rotate(45.0f, glm::vec3(0, 0, 1));
			Scale(glm::vec3(1.0f, 0.5f, 1.0f));
			Rotate(-45.0f, glm::vec3(0, 0, 1));
	
			AddIndice(0);
			AddIndice(2);
			AddIndice(1);
	
			AddIndice(2);
			AddIndice(4);
			AddIndice(3);
	
			AddIndice(4);
			AddIndice(0);
			AddIndice(5);
	
			AddIndice(0);
			AddIndice(4);
			AddIndice(2);
	
			Rotate(90.0f, glm::vec3(1, 0, 0));
		}*/

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
        Shape front = Shape(QUAD, coordinate, normal, color);
        front.Move(glm::vec3(0.0f, 0.0f, 0.5f));

        Shape back = Shape(QUAD, coordinate, normal, color);
        back.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        back.Move(glm::vec3(0.0f, 0.0f, -0.5f));

        Shape right = Shape(QUAD, coordinate, normal, color);
        right.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        right.Move(glm::vec3(-0.5f, 0.0f, 0.0f));

        Shape left = Shape(QUAD, coordinate, normal, color);
        left.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        left.Move(glm::vec3(0.5f, 0.0f, 0.0f));

        Shape top = Shape(QUAD, coordinate, normal, color);
        top.Rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        top.Move(glm::vec3(0.0f, -0.5f, 0.0f));

        Shape bottom = Shape(QUAD, coordinate, normal, color);
        bottom.Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        bottom.Move(glm::vec3(0.0f, 0.5f, 0.0f));

        Join(front);
        Join(back);
        Join(right);
        Join(left);
        Join(top);
        Join(bottom);

		//RecalculateNormals();
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
		coordinate = true;

		int layer = 1;
		int subLayers = resolution;
		const float BLADE_WIDTH = 0.05f;
		const float BLADE_HEIGHT = 1.0 / (subLayers + 2);

		positions.push_back(glm::vec3(-BLADE_WIDTH, layer * BLADE_HEIGHT, 0.0f));
		coordinates.push_back(glm::vec2(0.0, 0.0));
		positions.push_back(glm::vec3(BLADE_WIDTH, 0, 0.0f));
		coordinates.push_back(glm::vec2(1.0, 0.0));
		positions.push_back(glm::vec3(-BLADE_WIDTH, 0, 0.0f));
		coordinates.push_back(glm::vec2(0.0, 0.0));
		positions.push_back(glm::vec3(BLADE_WIDTH, layer * BLADE_HEIGHT, 0.0f));
		coordinates.push_back(glm::vec2(1.0, 0.0));

		for (int i = 0; i < subLayers; i++)
		{
			layer++;
			positions.push_back(glm::vec3(-BLADE_WIDTH, layer * BLADE_HEIGHT, 0.0f));
			coordinates.push_back(glm::vec2(0.0, 0.0));
			positions.push_back(glm::vec3(BLADE_WIDTH, layer * BLADE_HEIGHT, 0.0f));
			coordinates.push_back(glm::vec2(1.0, 0.0));
		}

		layer++;
		positions.push_back(glm::vec3(0.0f, layer * BLADE_HEIGHT, 0.0f));
		coordinates.push_back(glm::vec2(0.5, 1.0));

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
	else if (type == CROSS)
	{
		std::vector<Shape> shapes(resolution);
		shapes[0] = Shape(QUAD, true, true, false);

		float angleStep = 180.0f / float(resolution);
		for (int i = 1; i < resolution; i++)
		{
			shapes[i] = Shape(QUAD, true, true, false);
			shapes[i].Rotate(-(angleStep * float(i)), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		for (Shape &shape : shapes)
		{
			Join(shape);
		}

		//Shape forwards = Shape(QUAD, true, true);
        //forwards.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//Join(sidewards);
		//Join(forwards);
	}
	else if (type == TRIANGLE)
	{
		AddPosition(glm::vec3(-0.5f, -0.5f, 0.0f));
		AddPosition(glm::vec3(0.5f, 0.5f, 0.0f));
		//AddPosition(glm::vec3(-0.5f, 0.5f, 0.0f));
		AddPosition(glm::vec3(0.5f, -0.5f, 0.0f));

		AddIndice(0);
		AddIndice(2);
		AddIndice(1);

		//AddIndice(0);
		//AddIndice(1);
		//AddIndice(2);

		//AddIndice(3);
		//AddIndice(1);
		//AddIndice(0);

		if (coordinate)
		{
			AddCoordinate(glm::vec2(0.0f, 0.0f));
			AddCoordinate(glm::vec2(1.0f, 1.0f));
			//AddCoordinate(glm::vec2(0.0f, 1.0f));
			AddCoordinate(glm::vec2(1.0f, 0.0f));
		}

		if (normal)
		{
			AddNormal(glm::vec3(0.0f, 0.0f, 1.0f));
			AddNormal(glm::vec3(0.0f, 0.0f, 1.0f));
			//AddNormal(glm::vec3(0.0f, 0.0f, 1.0f));
			AddNormal(glm::vec3(0.0f, 0.0f, 1.0f));
		}

		if (color)
		{
			AddColor(glm::vec3(0.0f, 0.0f, 0.0f));
			AddColor(glm::vec3(0.0f, 0.0f, 0.0f));
			//AddColor(glm::vec3(0.0f, 0.0f, 0.0f));
			AddColor(glm::vec3(0.0f, 0.0f, 0.0f));
		}
	}
	
	else if (type == PRISM)
	{
		Shape front = Shape(TRIANGLE, coordinate, normal, color);
		front.Move(glm::vec3(0.0f, 0.0f, 0.5f));

		Shape back = Shape(TRIANGLE, coordinate, normal, color);
		back.Rotate(-90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		back.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		back.Move(glm::vec3(0.0f, 0.0f, -0.5f));

		Shape right = Shape(QUAD, coordinate, normal, color);
		right.Scale(glm::vec3(1.0f, sqrt(2.0f), 1.0f));
		right.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		right.Rotate(-45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		//right.Move(glm::vec3(-0.25f, 0.25f, 0.0f));

		Shape left = Shape(QUAD, coordinate, normal, color);
		left.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		left.Move(glm::vec3(0.5f, 0.0f, 0.0f));

		Shape top = Shape(QUAD, coordinate, normal, color);
		top.Rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		top.Move(glm::vec3(0.0f, -0.5f, 0.0f));

		//Shape bottom = Shape(QUAD, coordinate, normal, color);
		//bottom.Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		//bottom.Move(glm::vec3(0.0f, 0.5f, 0.0f));

		Join(front);
		Join(back);
		Join(right);
		Join(left);
		Join(top);
		//Join(bottom);
	}
	
}

void Shape::SetCube(bool front, bool back, bool left, bool right, bool top, bool bottom)
{
	if (front)
	{
		Shape frontSide = Shape(QUAD, coordinate, normal, color);
    	frontSide.Move(glm::vec3(0.0f, 0.0f, 0.5f));
		Join(frontSide);
	}

    if (back)
	{
		Shape backSide = Shape(QUAD, coordinate, normal, color);
    	backSide.Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    	backSide.Move(glm::vec3(0.0f, 0.0f, -0.5f));
		Join(backSide);
	}

    if (right)
	{
		Shape rightSide = Shape(QUAD, coordinate, normal, color);
    	rightSide.Rotate(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    	rightSide.Move(glm::vec3(-0.5f, 0.0f, 0.0f));
		Join(rightSide);
	}

    if (left)
	{
		Shape leftSide = Shape(QUAD, coordinate, normal, color);
    	leftSide.Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
   		leftSide.Move(glm::vec3(0.5f, 0.0f, 0.0f));
		Join(leftSide);
	}

    if (top)
	{
		Shape topSide = Shape(QUAD, coordinate, normal, color);
    	topSide.Rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    	topSide.Move(glm::vec3(0.0f, -0.5f, 0.0f));
		Join(topSide);
	}

    if (bottom)
	{
		Shape bottomSide = Shape(QUAD, coordinate, normal, color);
    	bottomSide.Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    	bottomSide.Move(glm::vec3(0.0f, 0.5f, 0.0f));
    	Join(bottomSide);
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

void Shape::AddNormal(glm::vec3 norm)
{
	if (!normal) return;

	normals.push_back(norm);
}

void Shape::AddColor(glm::vec3 col)
{
	if (!color) return;

	colors.push_back(col);
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

	if (color)
	{
		for (glm::vec3 color : joinShape.colors)
		{
			colors.push_back(color);
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

void Shape::Rotate(float degrees, glm::vec3 axis, bool rotateNormal)
{
	if (degrees == 0.0f) return;

    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(degrees), axis);

    for (glm::vec3 &pos : positions)
	{
		pos = rotation * glm::vec4(pos, 1.0f);
	}

	if (rotateNormal)
	{
		for (glm::vec3 &norm : normals)
		{
			norm = glm::normalize(rotation * glm::vec4(norm, 0.0f));
		}
	}
}

void Shape::Rotate90(int times, bool rotateNormal)
{
	if (times == 0) return;

	this->Rotate(90.0f * times, glm::vec3(0.0f, 1.0f, 0.0f), rotateNormal);
}

void Shape::Scale(glm::vec3 scale, bool scaleUV)
{
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	scaleMatrix = glm::scale(scaleMatrix, scale);

	for (glm::vec3 &pos : positions)
	{
		pos = scaleMatrix * glm::vec4(pos, 1.0f);
	}

	if (!scaleUV) return;

	for (glm::vec2 &coord : coordinates)
	{
		coord = scaleMatrix * glm::vec4(coord, 0.0f, 1.0f);
	}
}

void Shape::RecalculateNormals(bool x, bool y, bool z)
{
	normals.clear();

	for (const glm::vec3 &position : positions)
	{
		normals.push_back(glm::normalize(glm::vec3(x ? position.x : 0, y ? position.y : 0, z ? position.z : 0)));
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

void Shape::SetCoordinates(glm::vec2 uv)
{
	for (int i = 0; i < coordinates.size(); i++)
	{
		coordinates[i] = uv;
	}
}

void Shape::SetXCoordinates(float uv)
{
	for (int i = 0; i < coordinates.size(); i++)
	{
		coordinates[i].x = uv;
	}
}

void Shape::SetYCoordinates(float uv)
{
	for (int i = 0; i < coordinates.size(); i++)
	{
		coordinates[i].y = uv;
	}
}

void Shape::ScaleCoordinates(glm::vec2 scale)
{
	for (int i = 0; i < coordinates.size(); i++)
	{
		coordinates[i] *= scale;
	}
}

void Shape::SwapCoordinates()
{
	for (int i = 0; i < coordinates.size(); i++)
	{
		coordinates[i] = glm::vec2(coordinates[i].y, coordinates[i].x);
	}
}

void Shape::InverseCoordinates(bool inverseX, bool inverseY)
{
	for (int i = 0; i < coordinates.size(); i++)
	{
		float newX = coordinates[i].x;
		float newY = coordinates[i].y;
		if (inverseX) newX = 1.0 - newX;
		if (inverseY) newY = 1.0 - newY;
		coordinates[i] = glm::vec2(newX, newY);
	}
}

void Shape::SetColors(glm::vec3 value)
{
	for (int i = 0; i < colors.size(); i++)
	{
		colors[i] = value;
	}
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