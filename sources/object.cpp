#include "object.hpp"

#include "manager.hpp"

#include <stdexcept>

Object::Object() : mesh{nullptr} , pipeline{nullptr}
{

}

Object::Object(Mesh *mesh, Pipeline *pipeline) : mesh{mesh} , pipeline{pipeline}
{

}

Object::~Object()
{
	DestroyUniformBuffers();
}

void Object::CreateUniformBuffers()
{
	if (uniformBuffers.size() != 0) throw std::runtime_error("cannot create uniform buffers because they already exist");

	uniformBuffers.resize(Manager::settings.maxFramesInFlight);

	BufferConfiguration configuration;
	configuration.size = sizeof(UniformBufferObject);
	configuration.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	configuration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	configuration.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	configuration.mapped = true;

	for (Buffer &buffer : uniformBuffers)
	{
		buffer.Create(configuration);
	}
}

void Object::DestroyUniformBuffers()
{
	for (Buffer &buffer : uniformBuffers)
	{
		buffer.Destroy();
	}

	uniformBuffers.clear();
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

void Object::UpdateUniformBuffer(uint32_t currentImage)
{
	ubo.model = Translation();
	ubo.view = Manager::currentCamera.View();
	ubo.projection = Manager::currentCamera.Projection();
	memcpy(uniformBuffers[currentImage].mappedBuffer, &ubo, sizeof(ubo));
}