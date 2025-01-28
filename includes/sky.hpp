#pragma once

#include "pipeline.hpp"
#include "mesh.hpp"
#include "descriptor.hpp"
#include "buffer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Sky
{
	private:
		

	public:
		static Mesh skyMesh;
		static Pipeline skyPipeline;
		static Descriptor skyDescriptor;

		static void Create();
		static void CreateMesh();
		static void CreatePipeline();
		static void CreateDescriptor();

		static void Destroy();
		static void DestroyMesh();
		static void DestroyPipeline();
		static void DestroyDescriptor();

		static void Start();
		static void Frame();
		static void RecordCommands(VkCommandBuffer commandBuffer);
		static void RenderSky(VkCommandBuffer commandBuffer);
};