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

	//Manager::camera.UpdateView();
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
	//Manager::camera.Move(glm::vec3(3692.39, -997.286, 2000.46));
	//Manager::camera.Move(glm::vec3(3833.49, -1390.03, 4944.97));
	//Manager::camera.Move(glm::vec3(-12110.2, -1054.11, 2891.48));
	//Manager::camera.Move(glm::vec3(0, 0, 0));
	//Manager::camera.Rotate(glm::vec3(-8.79993, 183.699, 0));
	//Manager::camera.Rotate(glm::vec3(-40.9999, -287.8, 0));
	//Manager::camera.Rotate(glm::vec3(-19.4, -185.799, 0));
    //Manager::camera.Move(glm::vec3(2683.6, -1468.66, 2098.1));
	//Manager::camera.Rotate(glm::vec3(-26.6999, -846.307, 0));
    //Manager::camera.Move(glm::vec3(5272.46, -1923.35, -1738.65));
	//Manager::camera.Rotate(glm::vec3(1.8002, -4247.02, 0));
	//Manager::camera.Move(glm::vec3(24.6182, 19.1743, 14.6599));
	//Manager::camera.Rotate(glm::vec3(-13.4999, -867.011, 0));
	//Manager::camera.Move(glm::vec3(2261.03, -1828.33, 3524.79));
	//Manager::camera.Rotate(glm::vec3(-22.0999, -709.411, 0));
	//Manager::camera.Move(glm::vec3(16453.4, -1764.23, -16834.3));
	//Manager::camera.Rotate(glm::vec3(-20.7998, -1411.04, 0));
	//Manager::camera.Move(glm::vec3(2963.76, -1790.95, 4301.57));
	//Manager::camera.Rotate(glm::vec3(-16.4998, -1486.73, 0));

	Manager::camera.Move(glm::vec3(4558.53, -1246.5, 895.8));
	Manager::camera.Rotate(glm::vec3(-2.9999, -990.925, 0));

	Menu &menu = UI::NewMenu("camera");
	menu.AddText("camera values");
	menu.AddSlider("field of view", Manager::camera.FOV, 25.0f, 180.0f);

	//Manager::camera.TriggerMouse();
	//Manager::camera.canLook = true;
}

void Input::Frame()
{
	ProcessInput();
}

std::map<int, Input::KeyStatus> Input::keys;