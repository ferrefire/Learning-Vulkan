#include "camera.hpp"

#include "time.hpp"
#include "utilities.hpp"
#include "input.hpp"
#include "terrain.hpp"
#include "data.hpp"

#include <iostream>

Camera::Camera(Window &window) : window{window}
{
	far = 50000.0;
	//UpdateProjection();
	//Move(glm::vec3(0.0, 100.0, -100.0));
	////Rotate(glm::vec3(-25.0, -135.0, 0.0));
	//Rotate(glm::vec3(-25.0, 45.0, 0.0));
}

Camera::~Camera()
{

}

void Camera::UpdateProjection()
{
	cameraWidth = window.width;
	cameraHeight = window.height;

	projection = glm::perspective(glm::radians(FOV), (float)cameraWidth / (float)cameraHeight, near, far);
	projection[1][1] *= -1;

	projectionLod = glm::perspective(glm::radians(FOV), (float)cameraWidth / (float)cameraHeight, nearLod, farLod);
	projectionLod[1][1] *= -1;
}

void Camera::UpdateView()
{
	view = glm::lookAt(position, position + front, up);
	viewOffset = glm::lookAt(position + Y3Y(Terrain::terrainOffset), position + Y3Y(Terrain::terrainOffset) + front, up);
}

void Camera::Move(const glm::vec3 &amount)
{
	position += amount;

	if (!flying) position.y = glm::clamp(position.y, Data::GetGeneralData().viewHeight + 4.0f, 25000.0f);

	UpdateView();
}

void Camera::SetPosition(const glm::vec3 &newPosition)
{
	position = newPosition;

	UpdateView();
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

	UpdateView();
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

	UpdateView();
}

void Camera::SetProjection(const glm::mat4 &newProjection)
{
	this->projection = newProjection;
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

const glm::mat4 &Camera::ViewOffset()
{
	return (viewOffset);
}

const glm::mat4 &Camera::Projection()
{
	if (cameraWidth != window.width || cameraHeight != window.height) UpdateProjection();
	return (projection);
}

const glm::mat4 &Camera::ProjectionLod()
{
	return (projectionLod);
}

void Camera::PrintStatus()
{
	std::cout << "position: (" << Position().x + Terrain::terrainOffset.x << ", " << Position().y + Terrain::terrainOffset.y - 2500.0 << ", " << Position().z + Terrain::terrainOffset.z << ") rotation: (" << Angles().x << ", " << 
		Angles().y << ", " << Angles().z << ")" << std::endl;

	//glm::mat4 tempView = glm::lookAt(glm::vec3(0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	//std::vector<glm::vec4> corners = GetFrustumCorners(1, far, tempView);
	//for (glm::vec4 corner : corners)
	//{
	//	Utilities::PrintVec(corner);
	//}
}

void Camera::UpdateMovement()
{
	if (FOV != oldFOV)
	{
		oldFOV = FOV;
		UpdateProjection();
	}

	if (Input::GetKey(GLFW_KEY_M).pressed)
	{
		window.SetMouseVisibility(!window.mouseVisible);

		canMove = !window.mouseVisible;
		canLook = !window.mouseVisible;
	}

	if (!canMove) return;

	if (Input::GetKey(GLFW_KEY_F).pressed)
	{
		flying = !flying;
	}

	if (Input::GetKey(GLFW_KEY_W).down)
	{
		if (flying) Move(Front() * speed * Time::deltaTime);
		else if (!flying) Move(glm::normalize(XZ3XZ(Front())) * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_S).down)
	{
		if (flying) Move(-Front() * speed * Time::deltaTime);
		else if (!flying) Move(glm::normalize(XZ3XZ(-Front())) * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_D).down)
	{
		if (flying) Move(Side() * speed * Time::deltaTime);
		else if (!flying) Move(glm::normalize(XZ3XZ(Side())) * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_A).down)
	{
		if (flying) Move(-Side() * speed * Time::deltaTime);
		else if (!flying) Move(glm::normalize(XZ3XZ(-Side())) * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_SPACE).down)
	{
		Move(Up() * speed * Time::deltaTime);
	}
	if (Input::GetKey(GLFW_KEY_LEFT_CONTROL).down)
	{
		Move(-Up() * speed * Time::deltaTime);
	}

	if (!flying) Move(glm::vec3(0.0f, -9.81f * Time::deltaTime * 10.0f, 0.0f));
}

void Camera::UpdateRotation(double xpos, double ypos)
{
	if (!canLook) return;

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	//xoffset *= sensitivity * Time::deltaTime;
	//yoffset *= sensitivity * Time::deltaTime;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Rotate(Angles() + glm::vec3(yoffset, xoffset, 0.0f));
}

std::vector<glm::vec4> Camera::GetFrustumCorners(float nearDis, float farDis, glm::mat4 tempView)
{
	glm::mat4 tempProjection = glm::perspective(glm::radians(FOV), (float)cameraWidth / (float)cameraHeight, nearDis, farDis);
	glm::mat4 inverse = glm::inverse(tempProjection * tempView);
    std::vector<glm::vec4> corners;

    std::vector<glm::vec4> clipSpaceCorners = 
	{
        {-1, -1, 0, 1}, { 1, -1, 0, 1}, { 1,  1, 0, 1}, {-1,  1, 0, 1},
        {-1, -1,  1, 1}, { 1, -1,  1, 1}, { 1,  1,  1, 1}, {-1,  1,  1, 1}
    };

    for (const glm::vec4& corner : clipSpaceCorners)
	{
		glm::vec4 transformedCorner = inverse * corner;
        transformedCorner /= transformedCorner.w;
        corners.push_back(transformedCorner);
    }

    return corners;
}

//std::vector<glm::vec4> Camera::GetFrustumCorners(float nearDis, float farDis)
//{
//	return (GetFrustumCorners(nearDis, farDis, view));
//}

glm::mat4 Camera::GetTempProjection(float nearDis, float farDis)
{
	return (glm::perspective(glm::radians(FOV), (float)cameraWidth / (float)cameraHeight, nearDis, farDis));
}

glm::mat4 Camera::GetBoundedProjection(float nearDis, float farDis)
{
	float aspectRatio = (float)cameraWidth / (float)cameraHeight;
	float top = nearDis * tan(glm::radians(FOV) / 2.0f);
	float bottom = -top;
	float right = top * aspectRatio;
	float left = -right;

	glm::mat4 boundedProjection = glm::ortho(left, right, bottom, top, nearDis, farDis);

	return (boundedProjection);
}

std::vector<glm::vec4> Camera::GetFrustumCorners(float nearDis, float farDis)
{
	glm::mat4 tempProj = GetTempProjection(nearDis, farDis);
    glm::mat4 invCamVP = glm::inverse(tempProj * view);
    
    std::vector<glm::vec4> frustumCorners;

    for (int x = -1; x <= 1; x += 2)
	{
        for (int y = -1; y <= 1; y += 2)
		{
            for (int z = 0; z <= 1; z += 1)
			{
                glm::vec4 ndc(x, y, z, 1.0f);
                glm::vec4 corner = invCamVP * ndc;
                corner /= corner.w;
                frustumCorners.push_back(corner);
            }
        }
    }
    return (frustumCorners);
}

glm::vec3 Camera::WorldToClip(glm::vec3 worldSpace)
{
	glm::vec4 viewSpace = projection * view * glm::vec4(worldSpace, 1.0);

	glm::vec3 clipSpace = viewSpace;
	clipSpace /= viewSpace.w;

	clipSpace.x = clipSpace.x * 0.5 + 0.5;
	clipSpace.y = clipSpace.y * 0.5 + 0.5;
	clipSpace.z = viewSpace.w / far;

	return (clipSpace);
}

bool Camera::InView(glm::vec3 worldSpace, float tolerance)
{
	glm::vec3 clipSpace = WorldToClip(worldSpace);

	bool xInView = clipSpace.x >= 0.0 - tolerance && clipSpace.x <= 1.0 + tolerance;
	bool yInView = clipSpace.y >= 0.0 - tolerance && clipSpace.y <= 1.0 + tolerance;
	bool zInView = clipSpace.z >= 0.0 && clipSpace.z <= 1.0;

	return ((xInView && yInView && zInView) ? 1 : 0);
}

bool Camera::AreaInView(glm::vec3 worldSpace, float size, int range, float tolerance)
{
	for (int x = -range; x <= range; x++)
	{
		for (int z = -range; z <= range; z++)
		{
			for (int y = -range; y <= range; y++)
		{
			if (InView(worldSpace + glm::vec3(x, y, z) * size, tolerance))
			{
				return (true);
			}
		}
		}
	}

	return (false);
}