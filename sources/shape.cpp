#include "shape.hpp"

Shape::Shape()
{

}

Shape::Shape(int type)
{
    SetShape(type);
}

Shape::~Shape()
{

}

void Shape::SetShape(int type, int resolution)
{
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

		if (positionsOnly) return;

		AddCoordinate(glm::vec2(0.0f, 0.0f));
		AddCoordinate(glm::vec2(1.0f, 1.0f));
		AddCoordinate(glm::vec2(1.0f, 0.0f));
		AddCoordinate(glm::vec2(0.0f, 1.0f));
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
        positionsOnly = true;

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
		positionsOnly = true;

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
}

void Shape::AddPosition(glm::vec3 pos)
{
	positions.push_back(pos);
}

void Shape::AddCoordinate(glm::vec2 uv)
{
	if (positionsOnly) return;

	coordinates.push_back(uv);
}

void Shape::AddIndice(indexType index)
{
	indices.push_back(index);
}

void Shape::Join(Shape &joinShape)
{
    int count = glm::max(positions.size(), coordinates.size());

    for (glm::vec3 pos : joinShape.positions)
    {
        positions.push_back(pos);
    }

	for (indexType index : joinShape.indices)
	{
		indices.push_back(index + count);
	}

	if (positionsOnly) return;

	for (glm::vec2 coord : joinShape.coordinates)
    {
        coordinates.push_back(coord);
    }
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
}