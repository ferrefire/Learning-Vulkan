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
		static Pipeline transmittancePipeline;
		static Pipeline scatterPipeline;
		static Pipeline viewPipeline;
		static Pipeline aerialPipeline;

		static Descriptor skyDescriptor;
		static Descriptor transmittanceDescriptor;
		static Descriptor scatterDescriptor;
		static Descriptor viewDescriptor;
		static Descriptor aerialDescriptor;

		static Texture transmittanceTexture;
		static Texture scatterTexture;
		static Texture viewTexture;
		static Texture aerialTexture;

		static Buffer colorBuffer;

		static bool transmittanceComputed;
		static bool transmittanceReady;
		static bool scatterComputed;
		static bool scatterReady;
		static bool viewComputed;
		static bool viewReady;
		static bool shouldUpdateView;
		static bool aerialComputed;
		static bool shouldUpdateAerial;

		static void Create();
		static void CreateMesh();
		static void CreatePipelines();
		static void CreateTextures();
		static void CreateBuffers();
		static void CreateDescriptors();

		static void Destroy();
		static void DestroyMesh();
		static void DestroyPipelines();
		static void DestroyTextures();
		static void DestroyBuffers();
		static void DestroyDescriptors();

		static void Start();
		static void Frame();
		static void RecordCommands(VkCommandBuffer commandBuffer);
		static void RecordComputeCommands(VkCommandBuffer commandBuffer);
		static void RenderSky(VkCommandBuffer commandBuffer);
		static void ComputeTransmittance();
		static void ComputeScattering();
		static void ComputeView(VkCommandBuffer commandBuffer);
		static void ComputeAerial(VkCommandBuffer commandBuffer);
		static void Recompute();
};