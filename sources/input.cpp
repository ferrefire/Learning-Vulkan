#include "input.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "manager.hpp"
#include "time.hpp"

void Input::AddKey(int keyCode)
{
	AddKey(keyCode, false);
}

void Input::AddKey(int keyCode, bool mouse)
{
    keys[keyCode] = Input::KeyStatus();
	keys[keyCode].mouse = mouse;
}

void Input::SetKeyStatus()
{
    for (auto &key : keys)
    {
        bool lastFrameDown = key.second.down;
        if (key.second.mouse) key.second.down = glfwGetMouseButton(Manager::currentWindow.data, key.first) == GLFW_PRESS;
        else key.second.down = glfwGetKey(Manager::currentWindow.data, key.first) == GLFW_PRESS;

        key.second.pressed = (key.second.down && !lastFrameDown);
        key.second.released = (!key.second.down && lastFrameDown);
    }
}

void Input::ProcessInput()
{
    SetKeyStatus();

	Manager::camera.UpdateMovement();
}

void Input::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	Manager::camera.UpdateRotation(xpos, ypos);
}

void Input::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if ((float)yoffset > 0.0f) Manager::camera.speed *= 1.25f;
    else if ((float)yoffset < 0.0f) Manager::camera.speed *= 0.8f;
}

Input::KeyStatus Input::GetKey(int keyCode)
{
	return (GetKey(keyCode, false));
}

Input::KeyStatus Input::GetKey(int keyCode, bool mouse)
{
    if (keys.find(keyCode) == keys.end())
    {
        AddKey(keyCode, mouse);
        return (Input::KeyStatus());
    }

    return (keys[keyCode]);
}

void Input::Start()
{
	Manager::camera.UpdateProjection();

	Manager::camera.canLook = false;
	
	//Manager::camera.Move(glm::vec3(0.0, 1000.0, -1000.0));
	//Manager::camera.Move(glm::vec3(-300.0, 500.65, -2000.0));
	//Manager::camera.Move(glm::vec3(-276.0, 763, -2127.0));

	//Manager::camera.Move(glm::vec3(-21.3, 504.7, -2006.0));
	//Manager::camera.Move(glm::vec3(2583.95, -731.684, 1943.87));
	//Manager::camera.Move(glm::vec3(2583.95, 4268.32 - 5000.0, 1948.87));
	//Manager::camera.Move(glm::vec3(3235.13, -1081.11, 912.83));
	Manager::camera.Move(glm::vec3(3692.39, -997.286, 2000.46));

	//Manager::camera.Move(glm::vec3(-21.3, 504.7, -2001.0));
	//Manager::camera.Move(glm::vec3(-21.3, 504.7, -2001.0));
	// Rotate(glm::vec3(-25.0, -135.0, 0.0));
	// Manager::camera.Rotate(glm::vec3(-25.0, 45.0, 0.0));
	//Manager::camera.Rotate(glm::vec3(-25.0, 25.0, 0.0));
	//Manager::camera.Rotate(glm::vec3(-19, -849.8, 0.0));

	//Manager::camera.Rotate(glm::vec3(-27.6, -135.4, 0.0));
	//Manager::camera.Rotate(glm::vec3(-34.8999, 213.799, 0));
	//Manager::camera.Rotate(glm::vec3(3.19995, 155.999, 0));
	//Manager::camera.Rotate(glm::vec3(-16.5, 378.999, 0));
	Manager::camera.Rotate(glm::vec3(-8.79993, 183.699, 0));

	// Manager::camera.SetRotation(glm::vec3(1.4, -8.8, 0.0));
	Manager::camera.canLook = true;
}

void Input::Frame()
{
	ProcessInput();
}

std::map<int, Input::KeyStatus> Input::keys;