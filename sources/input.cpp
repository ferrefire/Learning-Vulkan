#include "input.hpp"

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
        if (key.second.mouse) key.second.down = glfwGetMouseButton(Manager::currentWindow->data, key.first) == GLFW_PRESS;
        else key.second.down = glfwGetKey(Manager::currentWindow->data, key.first) == GLFW_PRESS;

        key.second.pressed = (key.second.down && !lastFrameDown);
        key.second.released = (!key.second.down && lastFrameDown);
    }
}

void Input::ProcessInput()
{
    SetKeyStatus();

    //if (Input::canMove) CameraMovement();
}

/*
void Input::CameraMovement()
{
	if (glfwGetKey(Manager::window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Manager::camera.Move(Manager::camera.Front() * Manager::camera.speed * Time::deltaTime);
	}
	if (glfwGetKey(Manager::window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Manager::camera.Move(-Manager::camera.Front() * Manager::camera.speed * Time::deltaTime);
	}
	if (glfwGetKey(Manager::window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Manager::camera.Move(Manager::camera.Side() * Manager::camera.speed * Time::deltaTime);
	}
	if (glfwGetKey(Manager::window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Manager::camera.Move(-Manager::camera.Side() * Manager::camera.speed * Time::deltaTime);
	}
	if (glfwGetKey(Manager::window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Manager::camera.Move(Manager::camera.Up() * Manager::camera.speed * Time::deltaTime);
	}
	if (glfwGetKey(Manager::window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Manager::camera.Move(-Manager::camera.Up() * Manager::camera.speed * Time::deltaTime);
	}
}
*/

void Input::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (!canLook) return ;

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    //Manager::camera.Rotate(Manager::camera.Angles() + glm::vec3(yoffset, xoffset, 0.0f));
}

void Input::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    //if ((float)yoffset > 0.0f) Manager::camera.speed *= 1.25f;
    //else if ((float)yoffset < 0.0f) Manager::camera.speed *= 0.8f;
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

void Input::Frame()
{
	ProcessInput();
}

std::map<int, Input::KeyStatus> Input::keys;
//= std::map<int, Input::KeyStatus>()

bool Input::canMove = false;
bool Input::canLook = false;

float Input::lastX = 0;
float Input::lastY = 0;
float Input::sensitivity = 1;