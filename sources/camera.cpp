#include "camera.hpp"

#include "time.hpp"
#include "utilities.hpp"
#include "input.hpp"

#include <iostream>

Camera::Camera(Window &window) : window{window}
{
	UpdateProjection();
	Move(glm::vec3(0.0));
	Rotate(glm::vec3(25.0, -135.0, 0.0));
}

Camera::~Camera()
{

}

void Camera::UpdateProjection()
{
	projection = glm::perspective(glm::radians(FOV), (float)window.width / (float)window.height, near, far);
	projection[1][1] *= -1;
}

void Camera::Move(const glm::vec3 &amount)
{
	position += amount;

	view = glm::lookAt(position, position + front, up);
}

void Camera::SetPosition(const glm::vec3 &newPosition)
{
	position = newPosition;

	view = glm::lookAt(position, position + front, up);
}

void Camera::Rotate(const glm::vec3 &degrees)
{
	this->angles = degrees;

	if (angles.x > 89.0f) angles.x = 89.0f;
	if (angles.x < -89.0f) angles.x = -89.0f;

	direction.x = cos(glm::radians(angles.y)) * cos(glm::radians(angles.x));
	direction.y = sin(glm::radians(angles.x));
	direction.z = sin(glm::radians(angles.y)) * cos(glm::radians(angles.x));

	front = glm::normalize(direction);
	side = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(side, front));

	view = glm::lookAt(position, position + front, up);
}

void Camera::SetRotation(const glm::vec3 &newRotation)
{
	this->angles = newRotation;

	if (angles.x > 89.0f) angles.x = 89.0f;
	if (angles.x < -89.0f) angles.x = -89.0f;

	direction.x = cos(glm::radians(angles.y)) * cos(glm::radians(angles.x));
	direction.y = sin(glm::radians(angles.x));
	direction.z = sin(glm::radians(angles.y)) * cos(glm::radians(angles.x));

	front = glm::normalize(direction);
	side = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(side, front));

	view = glm::lookAt(position, position + front, up);
}

const glm::vec3 &Camera::Position()
{
	return (position);
}

const glm::vec3 &Camera::Front()
{
	return (front);
}

const glm::vec3 &Camera::Up()
{
	return (up);
}

const glm::vec3 &Camera::Side()
{
	return (side);
}

const glm::vec3 &Camera::Angles()
{
	return (angles);
}

const glm::mat4 &Camera::View()
{
	return (view);
}

const glm::mat4 &Camera::Projection()
{
	return (projection);
}

void Camera::PrintStatus()
{
	std::cout << "position: (" << Position().x << ", " << Position().y << ", " << Position().z << ") rotation: (" << Angles().x << ", " << 
		Angles().y << ", " << Angles().z << ")" << std::endl;
}

void Camera::UpdateMovement()
{
	if (Input::GetKey(GLFW_KEY_M).pressed)
	{
		canMove = !canMove;
		canLook = !canLook;

		window.SetMouseVisibility(!canLook);
	}

	if (!canMove) return;

	if (Input::GetKey(GLFW_KEY_W).down)
	{
		Move(Front() * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_S).down)
	{
		Move(-Front() * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_D).down)
	{
		Move(Side() * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_A).down)
	{
		Move(-Side() * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_SPACE).down)
	{
		Move(Up() * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_LEFT_CONTROL).down)
	{
		Move(-Up() * speed * Time::deltaTime);
	}
}

void Camera::UpdateRotation(double xpos, double ypos)
{
	if (!canLook) return;

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Rotate(Angles() + glm::vec3(yoffset, xoffset, 0.0f));
}