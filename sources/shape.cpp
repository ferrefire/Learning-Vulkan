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

void Shape::SetShape(int type)
{
	if (type == QUAD)
	{
		AddPosition(glm::vec3(-0.5f, -0.5f, 0.0f));
		AddPosition(glm::vec3(0.5f, -0.5f, 0.0f));
		AddPosition(glm::vec3(0.5f, 0.5f, 0.0f));
		AddPosition(glm::vec3(-0.5f, 0.5f, 0.0f));

		AddIndice(0);
		AddIndice(1);
		AddIndice(2);

		AddIndice(2);
		AddIndice(3);
		AddIndice(0);

		if (positionsOnly) return;

		AddCoordinate(glm::vec2(0.0f, 0.0f));
		AddCoordinate(glm::vec2(1.0f, 0.0f));
		AddCoordinate(glm::vec2(1.0f, 1.0f));
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

void Shape::AddIndice(uint16_t index)
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

	for (uint16_t index : joinShape.indices)
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