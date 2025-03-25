#include "input.hpp"

#include "manager.hpp"
#include "time.hpp"
#include "ui.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

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
	Manager::camera.canLook = false;
	Manager::camera.UpdateProjection();
	Manager::camera.Move(glm::vec3(3692.39, -997.286, 2000.46));
	Manager::camera.Rotate(glm::vec3(-8.79993, 183.699, 0));

	Menu &menu = UI::NewMenu("camera");
	menu.AddText("camera values");
	menu.AddSlider("field of view", Manager::camera.FOV, 25.0f, 180.0f);
}

void Input::Frame()
{
	ProcessInput();
}

std::map<int, Input::KeyStatus> Input::keys;