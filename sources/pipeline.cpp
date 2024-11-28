#include "pipeline.hpp"

#include "manager.hpp"
#include "utilities.hpp"
#include "time.hpp"
#include "shape.hpp"
#include "texture.hpp"
#include "shadow.hpp"
#include "culling.hpp"

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
	configuration.rasterization.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

	return (configuration);
}

Pipeline::Pipeline(Device &device, Camera &camera) : device{device} , camera{camera}
{
	
}

Pipeline::~Pipeline()
{
    Destroy();
}

void Pipeline::CreateGraphicsPipeline(std::string shader, std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig, PipelineConfiguration &pipelineConfig, VertexInfo &vertexInfo)
{
    if (graphicsPipeline) throw std::runtime_error("cannot create graphics pipeline because it already exists");

	if (pipelineConfig.shadow)
	{
		if (Shadow::trapezoidal) pipelineConfig.renderPass = Shadow::shadowTrapezoidPass;
		else pipelineConfig.renderPass = Shadow::shadowCascadePass;
	}
	else if (pipelineConfig.cull) pipelineConfig.renderPass = Culling::cullPass;
	else pipelineConfig.renderPass = Manager::currentWindow.renderPass;

	//CreateGlobalDescriptorSetLayout(descriptorsInfo.globalDescriptorLayoutConfig);
	globalDescriptorSetLayout = Manager::globalDescriptorSetLayout;
	CreateObjectDescriptorSetLayout(descriptorLayoutConfig);

	//if (!globalDescriptorSetLayout || !objectDescriptorSetLayout) throw std::runtime_error("cannot create graphics pipeline because the descriptor set layouts do not exist");
	if (!globalDescriptorSetLayout || (descriptorLayoutConfig.size() > 0 && !objectDescriptorSetLayout)) 
		throw std::runtime_error("cannot create graphics pipeline because the descriptor set layouts do not exist");

	std::string currentPath = Utilities::GetPath();

	std::vector<char> vertexCode = Utilities::FileToBinary((currentPath + "/shaders/" + shader + ".vert.spv").c_str());
	std::vector<char> tesselationControlCode;
	std::vector<char> tesselationEvaluationCode;
	//std::vector<char> fragmentCode = Utilities::FileToBinary((currentPath + "/shaders/" + shader + ".frag.spv").c_str());
	std::vector<char> fragmentCode;

	VkShaderModule vertexShaderModule = CreateShaderModule(vertexCode);
	VkShaderModule tesselationControlShaderModule;
	VkShaderModule tesselationEvaluationShaderModule;
	//VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentCode);
	VkShaderModule fragmentShaderModule;

	VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
	vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage = VERTEX_STAGE;
	vertexShaderStageInfo.module = vertexShaderModule;
	vertexShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo tesselationControlShaderStageInfo{};

	VkPipelineShaderStageCreateInfo tesselationEvaluationShaderStageInfo{};

	//VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
	//fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	//fragmentShaderStageInfo.stage = FRAGMENT_STAGE;
	//fragmentShaderStageInfo.module = fragmentShaderModule;
	//fragmentShaderStageInfo.pName = "main";
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};

	//VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragmentShaderStageInfo};

	VkPipelineTessellationStateCreateInfo tesselationInfo{};
	tesselationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	tesselationInfo.patchControlPoints = 3;

	int stageCount = 1 + (pipelineConfig.tesselation ? 2 : 0) + ((pipelineConfig.shadow || pipelineConfig.cull) ? 0 : 1);
	int currentStage = 0;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(stageCount);

	shaderStages[currentStage++] = vertexShaderStageInfo;

	if (pipelineConfig.tesselation)
	{
		pipelineConfig.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

		tesselationControlCode = Utilities::FileToBinary((currentPath + "/shaders/" + shader + ".tesc.spv").c_str());
		tesselationEvaluationCode = Utilities::FileToBinary((currentPath + "/shaders/" + shader + ".tese.spv").c_str());

		tesselationControlShaderModule = CreateShaderModule(tesselationControlCode);
		tesselationEvaluationShaderModule = CreateShaderModule(tesselationEvaluationCode);

		tesselationControlShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		tesselationControlShaderStageInfo.stage = TESSELATION_CONTROL_STAGE;
		tesselationControlShaderStageInfo.module = tesselationControlShaderModule;
		tesselationControlShaderStageInfo.pName = "main";

		tesselationEvaluationShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		tesselationEvaluationShaderStageInfo.stage = TESSELATION_EVALUATION_STAGE;
		tesselationEvaluationShaderStageInfo.module = tesselationEvaluationShaderModule;
		tesselationEvaluationShaderStageInfo.pName = "main";

		shaderStages[currentStage++] = tesselationControlShaderStageInfo;
		shaderStages[currentStage++] = tesselationEvaluationShaderStageInfo;
	}

	if (!pipelineConfig.shadow && !pipelineConfig.cull)
	{
		fragmentCode = Utilities::FileToBinary((currentPath + "/shaders/" + shader + ".frag.spv").c_str());

		fragmentShaderModule = CreateShaderModule(fragmentCode);

		fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStageInfo.stage = FRAGMENT_STAGE;
		fragmentShaderStageInfo.module = fragmentShaderModule;
		fragmentShaderStageInfo.pName = "main";

		shaderStages[currentStage++] = fragmentShaderStageInfo;
	}

	auto bindingDescription = vertexInfo.bindingDescription;
	auto attributeDescriptions = vertexInfo.attributeDescriptions;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = vertexInfo.bindingCount;
	vertexInputInfo.vertexAttributeDescriptionCount = vertexInfo.attributeCount;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	//std::vector<VkDescriptorSetLayout> descriptorSetLayouts =
	//{
	//	globalDescriptorSetLayout,
	//	objectDescriptorSetLayout
	//};

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	if (globalDescriptorSetLayout) descriptorSetLayouts.push_back(globalDescriptorSetLayout);
	if (objectDescriptorSetLayout) descriptorSetLayouts.push_back(objectDescriptorSetLayout);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	VkPushConstantRange pushConstantRange{};
	if (pipelineConfig.pushConstantCount > 0)
	{
		pushConstantRange.stageFlags = pipelineConfig.pushConstantStage;
		pushConstantRange.offset = 0;
		pushConstantRange.size = pipelineConfig.pushConstantSize * pipelineConfig.pushConstantCount;
		//pipelineLayoutInfo.pushConstantRangeCount = pipelineConfig.pushConstantCount;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	}

	if (vkCreatePipelineLayout(device.logicalDevice, &pipelineLayoutInfo, nullptr, &graphicsPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout");
	}

	if (Manager::settings.wireframe)
	{
		pipelineConfig.rasterization.polygonMode = VK_POLYGON_MODE_LINE;
		//pipelineConfig.rasterization.lineWidth = 3.0f;
	}

	pipelineConfig.multisampling.rasterizationSamples = device.MaxSampleCount();

	if (pipelineConfig.foliage)
	{
		pipelineConfig.rasterization.cullMode = VK_CULL_MODE_NONE;
	}

	if (pipelineConfig.cullFront)
	{
		pipelineConfig.rasterization.cullMode = VK_CULL_MODE_FRONT_BIT;
	}

	if (pipelineConfig.shadow)
	{
		//if (!pipelineConfig.foliage) pipelineConfig.rasterization.cullMode = VK_CULL_MODE_FRONT_BIT;
		pipelineConfig.rasterization.depthBiasEnable = VK_TRUE;
		pipelineConfig.rasterization.depthBiasConstantFactor = 4.0f;
		pipelineConfig.rasterization.depthBiasSlopeFactor = 1.5f;
	}

	VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
	graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	graphicsPipelineInfo.pStages = shaderStages.data();
	graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
	graphicsPipelineInfo.pInputAssemblyState = &pipelineConfig.inputAssembly;
	if (pipelineConfig.tesselation) graphicsPipelineInfo.pTessellationState = &tesselationInfo;
	graphicsPipelineInfo.pViewportState = &pipelineConfig.viewportState;
	graphicsPipelineInfo.pRasterizationState = &pipelineConfig.rasterization;
	graphicsPipelineInfo.pMultisampleState = &pipelineConfig.multisampling;
	graphicsPipelineInfo.pDepthStencilState = &pipelineConfig.depthStencil;
	graphicsPipelineInfo.pColorBlendState = &pipelineConfig.colorBlending;
	graphicsPipelineInfo.pDynamicState = &dynamicState;
	graphicsPipelineInfo.layout = graphicsPipelineLayout;
	graphicsPipelineInfo.renderPass = pipelineConfig.renderPass;
	graphicsPipelineInfo.subpass = pipelineConfig.subpass;
	graphicsPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device.logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device.logicalDevice, vertexShaderModule, nullptr);
	if (pipelineConfig.tesselation)
	{
		vkDestroyShaderModule(device.logicalDevice, tesselationControlShaderModule, nullptr);
		vkDestroyShaderModule(device.logicalDevice, tesselationEvaluationShaderModule, nullptr);
	}
	if (!pipelineConfig.shadow && !pipelineConfig.cull)
	{
		vkDestroyShaderModule(device.logicalDevice, fragmentShaderModule, nullptr);
	}
}

void Pipeline::CreateComputePipeline(std::string shader, std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig)
{
	if (computePipeline) throw std::runtime_error("cannot create compute pipeline because it already exists");

	//CreateGlobalDescriptorSetLayout(descriptorsInfo.globalDescriptorLayoutConfig);
	globalDescriptorSetLayout = Manager::globalDescriptorSetLayout;
	CreateObjectDescriptorSetLayout(descriptorLayoutConfig);

	if (!globalDescriptorSetLayout || !objectDescriptorSetLayout) throw std::runtime_error("cannot create graphics pipeline because the descriptor set layouts do not exist");

	std::string currentPath = Utilities::GetPath();

	auto computeCode = Utilities::FileToBinary((currentPath + "/shaders/" + shader + ".comp.spv").c_str());

	VkShaderModule computeShaderModule = CreateShaderModule(computeCode);

	VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
	computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computeShaderStageInfo.stage = COMPUTE_STAGE;
	computeShaderStageInfo.module = computeShaderModule;
	computeShaderStageInfo.pName = "main";

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts =
	{
		globalDescriptorSetLayout,
		objectDescriptorSetLayout
	};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

	if (vkCreatePipelineLayout(device.logicalDevice, &pipelineLayoutInfo, nullptr, &computePipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create compute pipeline layout");
	}

	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.layout = computePipelineLayout;
	pipelineInfo.stage = computeShaderStageInfo;

	if (vkCreateComputePipelines(device.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create compute pipeline");
	}

	vkDestroyShaderModule(device.logicalDevice, computeShaderModule, nullptr);
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

//void Pipeline::CreateGlobalDescriptorSetLayout(std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig)
//{
//	if (globalDescriptorSetLayout) throw std::runtime_error("cannot create global descriptor set layout because it already exists");
//
//	CreateDescriptorSetLayout(descriptorLayoutConfig, &globalDescriptorSetLayout);
//}

void Pipeline::CreateObjectDescriptorSetLayout(std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig)
{
    if (objectDescriptorSetLayout) throw std::runtime_error("cannot create object descriptor set layout because it already exists");

	CreateDescriptorSetLayout(descriptorLayoutConfig, &objectDescriptorSetLayout);
}

void Pipeline::CreateDescriptorSetLayout(std::vector<DescriptorLayoutConfiguration> &descriptorLayoutConfig, VkDescriptorSetLayout *descriptorSetLayout)
{
	if (descriptorLayoutConfig.size() == 0) return;

	std::vector<VkDescriptorSetLayoutBinding> bindings;
	bindings.resize(descriptorLayoutConfig.size());

	int index = 0;
	for (DescriptorLayoutConfiguration &config : descriptorLayoutConfig)
	{
		bindings[index].binding = index;
		bindings[index].descriptorCount = config.count;
		bindings[index].descriptorType = config.type;
		bindings[index].stageFlags = config.stages;
		bindings[index].pImmutableSamplers = nullptr;
		index++;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(Manager::currentDevice.logicalDevice, &layoutInfo, nullptr, descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout");
	}
}

void Pipeline::BindGraphics(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, GRAPHICS_BIND_POINT, graphicsPipeline);

	/*
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
	*/
}

void Pipeline::BindCompute(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, COMPUTE_BIND_POINT, computePipeline);
}

void Pipeline::Destroy()
{
	DestroyGraphicsPipeline();
	DestroyComputePipeline();
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

void Pipeline::DestroyComputePipeline()
{
	if (computePipeline)
	{
		vkDestroyPipeline(device.logicalDevice, computePipeline, nullptr);
		computePipeline = nullptr;
	}

	if (computePipelineLayout)
	{
		vkDestroyPipelineLayout(device.logicalDevice, computePipelineLayout, nullptr);
		computePipelineLayout = nullptr;
	}
}

void Pipeline::DestroyDescriptorSetLayout()
{
	if (objectDescriptorSetLayout)
	{
		vkDestroyDescriptorSetLayout(device.logicalDevice, objectDescriptorSetLayout, nullptr);
		objectDescriptorSetLayout = nullptr;
	}

	//if (globalDescriptorSetLayout)
	//{
	//	vkDestroyDescriptorSetLayout(device.logicalDevice, globalDescriptorSetLayout, nullptr);
	//	globalDescriptorSetLayout = nullptr;
	//}
}
