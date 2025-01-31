#pragma once

#include "pipeline.hpp"
#include "mesh.hpp"
#include "descriptor.hpp"
#include "buffer.hpp"
#include "texture.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Sky
{
	private:

	public:
		static Mesh skyMesh;

		static Pipeline skyPipeline;
		static Pipeline scatterComputePipeline;

		static Descriptor skyDescriptor;
		static Descriptor scatterComputeDescriptor;

		static Texture scatterTexture;

		static bool scatterComputed;

		static void Create();
		static void CreateMesh();
		static void CreatePipelines();
		static void CreateTextures();
		static void CreateDescriptors();

		static void Destroy();
		static void DestroyMesh();
		static void DestroyPipelines();
		static void DestroyTextures();
		static void DestroyDescriptors();

		static void Start();
		static void Frame();
		static void RecordCommands(VkCommandBuffer commandBuffer);
		static void RenderSky(VkCommandBuffer commandBuffer);
		static void ComputeInScattering();
};