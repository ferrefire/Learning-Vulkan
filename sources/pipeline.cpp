#include "pipeline.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "time.hpp"
#include "shape.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <string>
#include <cstring>

PipelineConfiguration Pipeline::DefaultConfiguration()
{
	PipelineConfiguration configuration{};

	configuration.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	configuration.viewportState.viewportCount = 1;
	configuration.viewportState.scissorCount = 1;

	configuration.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	configuration.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	configuration.inputAssembly.primitiveRestartEnable = VK_FALSE;

	configuration.rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	configuration.rasterization.depthClampEnable = VK_FALSE;
	configuration.rasterization.rasterizerDiscardEnable = VK_FALSE;
	configuration.rasterization.polygonMode = VK_POLYGON_MODE_FILL;
	configuration.rasterization.lineWidth = 1.0f;
	configuration.rasterization.cullMode = VK_CULL_MODE_BACK_BIT;
	configuration.rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE;
	configuration.rasterization.depthBiasEnable = VK_FALSE;
	configuration.rasterization.depthBiasConstantFactor = 0.0f;
	configuration.rasterization.depthBiasClamp = 0.0f;
	configuration.rasterization.depthBiasSlopeFactor = 0.0f;

	configuration.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	configuration.multisampling.sampleShadingEnable = VK_FALSE;
	configuration.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	configuration.multisampling.minSampleShading = 1.0f;
	configuration.multisampling.pSampleMask = nullptr;
	configuration.multisampling.alphaToCoverageEnable = VK_FALSE;
	configuration.multisampling.alphaToOneEnable = VK_FALSE;

	configuration.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	configuration.colorBlendAttachment.blendEnable = VK_FALSE;

	configuration.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	configuration.colorBlending.logicOpEnable = VK_FALSE;
	configuration.colorBlending.logicOp = VK_LOGIC_OP_COPY;
	configuration.colorBlending.attachmentCount = 1;
	configuration.colorBlending.pAttachments = &configuration.colorBlendAttachment;
	configuration.colorBlending.blendConstants[0] = 0.0f;
	configuration.colorBlending.blendConstants[1] = 0.0f;
	configuration.colorBlending.blendConstants[2] = 0.0f;
	configuration.colorBlending.blendConstants[3] = 0.0f;

	configuration.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	configuration.depthStencil.depthTestEnable = VK_TRUE;
	configuration.depthStencil.depthWriteEnable = VK_TRUE;
	configuration.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	configuration.depthStencil.depthBoundsTestEnable = VK_FALSE;
	configuration.depthStencil.minDepthBounds = 0.0f; // Optional
	configuration.depthStencil.maxDepthBounds = 1.0f; // Optional
	configuration.depthStencil.stencilTestEnable = VK_FALSE;
	configuration.depthStencil.front = {}; // Optional
	configuration.depthStencil.back = {}; // Optional

	configuration.renderPass = Manager::currentWindow.renderPass;

	return (configuration);
}

Pipeline::Pipeline(Device &device, Camera &camera) : device{device} , camera{camera}
{
	
}

Pipeline::~Pipeline()
{
    Destroy();
}

void Pipeline::Create()
{
	CreateDescriptorSetLayout();
	CreateGraphicsPipeline("simple", "simple", Mesh::GetVertexInfo(true, true), DefaultConfiguration());

	texture.Create("texture.jpg", &Manager::currentDevice);

	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSets();
}

void Pipeline::CreateGraphicsPipeline(std::string vertexShader, std::string fragmentShader, VertexInfo vertexInfo, PipelineConfiguration configuration)
{
    if (graphicsPipeline) throw std::runtime_error("cannot create graphics pipeline because it already exists");

	std::string currentPath = Utilities::GetPath();

	auto vertexCode = Utilities::FileToBinary((currentPath + "/shaders/" + vertexShader + ".vert.spv").c_str());
	auto fragmentCode = Utilities::FileToBinary((currentPath + "/shaders/" + fragmentShader + ".frag.spv").c_str());

	VkShaderModule vertexShaderModule = CreateShaderModule(vertexCode);
	VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentCode);

	VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
	vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = vertexShaderModule;
	vertexShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStageInfo.module = fragmentShaderModule;
	fragmentShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragmentShaderStageInfo};

	auto bindingDescription = vertexInfo.bindingDescription;
	auto attributeDescriptions = vertexInfo.attributeDescriptions;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = vertexInfo.bindingCount;
	vertexInputInfo.vertexAttributeDescriptionCount = vertexInfo.attributeCount;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	/*
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {};	 // Optional
	*/

	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device.logicalDevice, &pipelineLayoutInfo, nullptr, &graphicsPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout");
	}

	VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
	graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineInfo.stageCount = 2;
	graphicsPipelineInfo.pStages = shaderStages;
	graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
	graphicsPipelineInfo.pInputAssemblyState = &configuration.inputAssembly;
	graphicsPipelineInfo.pViewportState = &configuration.viewportState;
	graphicsPipelineInfo.pRasterizationState = &configuration.rasterization;
	graphicsPipelineInfo.pMultisampleState = &configuration.multisampling;
	graphicsPipelineInfo.pDepthStencilState = &configuration.depthStencil;
	graphicsPipelineInfo.pColorBlendState = &configuration.colorBlending;
	graphicsPipelineInfo.pDynamicState = &dynamicState;
	graphicsPipelineInfo.layout = graphicsPipelineLayout;
	graphicsPipelineInfo.renderPass = configuration.renderPass;
	graphicsPipelineInfo.subpass = configuration.subpass;
	graphicsPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device.logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device.logicalDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(device.logicalDevice, fragmentShaderModule, nullptr);
}

VkShaderModule Pipeline::CreateShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device.logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return (shaderModule);
}

void Pipeline::CreateDescriptorSetLayout()
{
    if (descriptorSetLayout) throw std::runtime_error("cannot create descriptor set layout because it already exists");

	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device.logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout");
	}
}

void Pipeline::CreateUniformBuffers()
{
    if (uniformBuffers.size() != 0 || uniformBuffersMemory.size() != 0)
		throw std::runtime_error("cannot create uniform buffers because they already exist");

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

		vkMapMemory(device.logicalDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}

void Pipeline::CreateDescriptorPool()
{
    if (descriptorPool) throw std::runtime_error("cannot create descriptor pool because it already exists");

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(device.logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool");
	}
}

void Pipeline::CreateDescriptorSets()
{
    if (descriptorSets.size() != 0) throw std::runtime_error("cannot create descriptor sets because they already exist");

	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device.logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture.imageView;
		imageInfo.sampler = texture.sampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device.logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void Pipeline::UpdateUniformBuffer(glm::mat4 translation, uint32_t currentImage)
{
	//ubo.model = glm::rotate(glm::mat4(1.0f), Time::currentFrame * glm::radians(90.0f), glm::vec3(0.5f, 1.0f, 0.25f));
	//ubo.model = glm::mat4(1.0f);
	ubo.model = translation;
	ubo.view = camera.View();
	ubo.projection = camera.Projection();
	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void Pipeline::Bind(VkCommandBuffer commandBuffer, Window &window)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(window.swapChainExtent.width);
	viewport.height = static_cast<float>(window.swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = window.swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 0, 1, &descriptorSets[Manager::currentFrame], 0, nullptr);
}

void Pipeline::Destroy()
{
	DestroyGraphicsPipeline();

	texture.Destroy();

	DestroyUniformBuffers();
	DestroyDescriptorPool();
	DestroyDescriptorSetLayout();
}

void Pipeline::DestroyGraphicsPipeline()
{
    if (graphicsPipeline)
	{
		vkDestroyPipeline(device.logicalDevice, graphicsPipeline, nullptr);
		graphicsPipeline = nullptr;
	}

	if (graphicsPipelineLayout)
	{
		vkDestroyPipelineLayout(device.logicalDevice, graphicsPipelineLayout, nullptr);
		graphicsPipelineLayout = nullptr;
	}
}

void Pipeline::DestroyDescriptorSetLayout()
{
    if (!descriptorSetLayout) return;

	vkDestroyDescriptorSetLayout(device.logicalDevice, descriptorSetLayout, nullptr);
	descriptorSetLayout = nullptr;
}

void Pipeline::DestroyUniformBuffers()
{
    for (VkBuffer &buffer : uniformBuffers)
	{
		vkDestroyBuffer(device.logicalDevice, buffer, nullptr);
	}
	uniformBuffers.clear();

	for (VkDeviceMemory &memory : uniformBuffersMemory)
	{
		vkFreeMemory(device.logicalDevice, memory, nullptr);
	}
	uniformBuffersMemory.clear();
}

void Pipeline::DestroyDescriptorPool()
{
    if (!descriptorPool) return;

	vkDestroyDescriptorPool(device.logicalDevice, descriptorPool, nullptr);
	descriptorPool = nullptr;
}