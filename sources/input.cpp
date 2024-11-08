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
	//Manager::camera.Move(glm::vec3(0.0, 1000.0, -1000.0));
	Manager::camera.Move(glm::vec3(0.0, 1000.0, -1000.0));
	// Rotate(glm::vec3(-25.0, -135.0, 0.0));
	Manager::camera.Rotate(glm::vec3(-25.0, 45.0, 0.0));
}

void Input::Frame()
{
	ProcessInput();
}

std::map<int, Input::KeyStatus> Input::keys;