#include "manager.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "input.hpp"

void Manager::Start()
{
	try
	{
		InitializeGLFW();
		InitializeVulkan();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		Quit(EXIT_FAILURE);
	}
}

void Manager::Clean()
{
	
}

void Manager::Quit(int exitCode)
{
	Clean();

	graphics.Destroy();

	if (glfwInitialized)
    {
        window.Destroy();
        glfwTerminate();
    }

	exit(exitCode);
}

void Manager::InitializeGLFW()
{
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize glfw library");
    }

    glfwInitialized = true;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window.Create();
}

void Manager::InitializeVulkan()
{
	graphics.Create();

	vulkanInitialized = true;

	std::cout << device.properties.deviceName << std::endl;
}

void Manager::Frame()
{
	if (Input::GetKey(GLFW_KEY_ESCAPE).pressed)
	{
		window.Close();
	}
}

void Manager::DestroyPipelines()
{
	for (Pipeline *pipeline : pipelines)
	{
        if (!pipeline) continue;

		pipeline->Destroy();
		delete(pipeline);
	}
    pipelines.clear();
}

void Manager::DestroyMeshes()
{
	for (Mesh *mesh : meshes)
	{
        if (!mesh) continue;

        mesh->Destroy();
		delete(mesh);
	}
	meshes.clear();
}

void Manager::DestroyObjects()
{
	for (Object *object : objects)
	{
		if (!object) continue;

		delete(object);
	}
	objects.clear();
}

Window &Manager::GetWindow()
{
	return (window);
}

Graphics &Manager::GetGraphics()
{
	return (graphics);
}

Pipeline *Manager::NewPipeline()
{
	Pipeline *pipeline = new Pipeline(device, camera);
	pipelines.push_back(pipeline);

	return (pipeline);
}

Mesh *Manager::NewMesh()
{
	Mesh *mesh = new Mesh();
	meshes.push_back(mesh);

	return (mesh);
}

Object *Manager::NewObject()
{
	Object *object = new Object();
	objects.push_back(object);
	
	return (object);
}

bool Manager::glfwInitialized = false;
bool Manager::vulkanInitialized = false;

Device Manager::device;
Window Manager::window{device};
Camera Manager::camera{window};
std::vector<Mesh *> Manager::meshes;
std::vector<Pipeline *> Manager::pipelines;
Graphics Manager::graphics{device, window};

Device &Manager::currentDevice = Manager::device;
Window &Manager::currentWindow = Manager::window;
Camera &Manager::currentCamera = Manager::camera;
Graphics &Manager::currentGraphics = Manager::graphics;

uint32_t Manager::currentFrame = 0;

std::vector<Object *> Manager::objects;
Settings Manager::settings;