#pragma once

#include "window.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class Camera
{
	private:
		Window &window;

		glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 angles = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 side = glm::vec3(1.0f, 0.0f, 0.0f);

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 viewOffset = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

	public:
		Camera(Window &window);
		~Camera();

		float near = 0.1f;
		//float far = 1000.0f;
		float far = 25000.0f;
		float FOV = 45.0f;
		//float speed = 1.0f;
		float speed = 25.0f;

		bool canMove = true;
		bool canLook = true;

		float lastX = 0;
		float lastY = 0;
		float sensitivity = 0.1f;

		uint32_t cameraWidth = 800;
		uint32_t cameraHeight = 600;

		void Move(const glm::vec3 &amount);
		void SetPosition(const glm::vec3 &newPosition);
		void Rotate(const glm::vec3 &degrees);
		void SetRotation(const glm::vec3 &newRotation);
		void UpdateProjection();
		void UpdateView();

		void UpdateMovement();
		void UpdateRotation(double xpos, double ypos);
		void PrintStatus();

		const glm::vec3 &Position();
		const glm::vec3 &Front();
		const glm::vec3 &Up();
		const glm::vec3 &Side();
		const glm::vec3 &Angles();

		const glm::mat4 &View();
		const glm::mat4 &ViewOffset();
		const glm::mat4 &Projection();

		std::vector<glm::vec4> GetFrustumCorners(float nearDis, float farDis);
		std::vector<glm::vec4> GetFrustumCorners(float nearDis, float farDis, glm::mat4 tempView);
		glm::mat4 GetTempProjection(float nearDis, float farDis);
};
