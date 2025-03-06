#pragma once

#include "pipeline.hpp"
#include "texture.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Capture
{
    private:
        
    public:
        static uint32_t captureWidth;
        static uint32_t captureHeight;
        static glm::mat4 captureProjection;

        static VkRenderPass capturePass;

        static Texture colorTexture;
        static Texture depthTexture;
        static std::vector<Texture> captureTextures;
        static std::vector<VkFramebuffer> captureFramebuffers;

        static VkCommandBuffer captureCommandBuffer;

        static int captureCount;
        static bool capturing;

        //static Pipeline graphicsPipeline;

        static void Create();
        static void CreateRenderPass();
        static void CreateTextures();
        static void CreateFramebuffers();
        //static void CreatePipeline();

        static void Destroy();
        static void DestroyRenderPass();
        static void DestroyTextures();
        static void DestroyFramebuffers();
        //static void DestroyPipeline();

        static void StartCapturing(int iteration);
        static void StopCapturing();
};