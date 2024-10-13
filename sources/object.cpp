#include "object.hpp"

#include "manager.hpp"

Object::Object(Mesh *mesh, Pipeline *pipeline) : mesh{mesh} , pipeline{pipeline}
{

}

Object::~Object()
{

}

void Object::Move(glm::vec3 amount)
{
    position += amount;
}

void Object::Rotate(glm::vec3 amount)
{
    rotation += amount;
}

void Object::Resize(glm::vec3 amount)
{
    scale = amount;
}

glm::mat4 Object::Translation()
{
    translation = glm::mat4(1.0f);

    translation = glm::translate(translation, position);
    translation = glm::scale(translation, scale);

    translation = glm::rotate(translation, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    translation = glm::rotate(translation, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    translation = glm::rotate(translation, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    return (translation);
}