#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <map>
//#include "camera.hpp"

class Input
{
    private:
        

    public:
        struct KeyStatus
        {
            bool down = false;
            float downDuration = 0;

            bool pressed = false;
            bool released = false;

			bool mouse = false;
        };

        static std::map<int, KeyStatus> keys;

		static void ProcessInput();
		static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

        static void AddKey(int keyCode);
        static KeyStatus GetKey(int keyCode);
		static void AddKey(int keyCode, bool mouse);
		static KeyStatus GetKey(int keyCode, bool mouse);
		static void SetKeyStatus();

		static void Start();
		static void Frame();
};