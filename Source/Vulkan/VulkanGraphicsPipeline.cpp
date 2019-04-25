#include "VulkanGraphicsPipeline.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkPhysicalDevice &physicalDevice, VkDevice &logicalDevice, VkShaderModule &vertexShaderModule, VkShaderModule  &fragmentShaderModule, VkExtent2D &swapchainExtent, VkSurfaceFormatKHR  &swapchainImageFormat, VkDescriptorSetLayout &descriptorSetLayout) :
	physicalDevice(physicalDevice),
	logicalDevice(logicalDevice),
	vertexShaderModule(vertexShaderModule),
	fragmentShaderModule(fragmentShaderModule),
	swapchainExtent2D(swapchainExtent), 
	swapchainImageFormat(swapchainImageFormat), 
	//swapchainImageCollection(swapchainImageCollection),
	descriptorSetLayout(descriptorSetLayout)
	//depthImageView(depthImageView)
{
	this->init_renderpass();
	this->init_graphicsPipelineLayout();
	//this->init_framebuffer();
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() 
{

	/*for (auto framebuffer : this->swapchainFramebufferCollection) {
		vkDestroyFramebuffer(this->logicalDevice, framebuffer, nullptr);
	}*/

	vkDestroyPipeline(this->logicalDevice, this->graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(this->logicalDevice, this->pipelineLayout, nullptr);
	vkDestroyRenderPass(this->logicalDevice, this->renderPass, nullptr);

}

//std::vector<VkFramebuffer> &VulkanGraphicsPipeline::GetFramebufferCollection()
//{
//	return this->swapchainFramebufferCollection;
//}

VkRenderPass &VulkanGraphicsPipeline::GetRenderPass()
{
	return this->renderPass;
}

VkPipeline &VulkanGraphicsPipeline::GetGraphicsPipeline()
{
	return this->graphicsPipeline;
}

VkPipelineLayout &VulkanGraphicsPipeline::GetGraphicsPipelineLayout()
{
	return this->pipelineLayout;
}

//void VulkanGraphicsPipeline::ResizeFrameBuffer(std::uint32_t width, std::uint32_t height, std::vector<Image> swapchainImgCollection, VkImageView depthImageView)
//{
//	this->depthImageView           = depthImageView;
//	this->swapchainImageCollection = swapchainImgCollection;
//
//
//	// Important: Make shure when this function is called no other vulkan object relies on the framebuffer
//	for (auto framebuffer : this->swapchainFramebufferCollection) {
//		vkDestroyFramebuffer(this->logicalDevice, framebuffer, nullptr);
//	}
//	this->swapchainFramebufferCollection.clear();
//
//
//	this->swapchainExtent2D.width  = width;
//	this->swapchainExtent2D.height = height;
//
//	this->init_framebuffer();
//}

void VulkanGraphicsPipeline::init_graphicsPipelineLayout()
{
	// Configure the parts for the Graphics pipeline

	//Vertex input
	VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
	vertexShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageInfo.module = this->vertexShaderModule;
	vertexShaderStageInfo.pName  = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = this->fragmentShaderModule;
	fragShaderStageInfo.pName  = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragShaderStageInfo };



	
	// Vertex data format from VulkanVertex;
	auto bindingDesc = VulkanVertexData::GetBindingDescription();
	auto attribDesc  = VulkanVertexData::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount   = 1;
	vertexInputInfo.pVertexBindingDescriptions      = &bindingDesc; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribDesc.size());
	vertexInputInfo.pVertexAttributeDescriptions    = attribDesc.data(); // Optional




	//Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType					 = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology				 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;



	//Viewports and scissors
	VkViewport viewport = {};
	viewport.x		  = 0.0f;
	viewport.y		  = 0.0f;
	viewport.width    = (float)this->swapchainExtent2D.width;
	viewport.height   = (float)this->swapchainExtent2D.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor  = {};
	scissor.offset    = {0, 0};
	scissor.extent    = this->swapchainExtent2D;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports	= &viewport;
	viewportState.scissorCount	= 1;
	viewportState.pScissors		= &scissor;


	//Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


	//Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable   = VK_FALSE;
	multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading      = 1.0f;     // Optional
	multisampling.pSampleMask           = nullptr;  // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable      = VK_FALSE; // Optional


	//Depth and stencil testing
	//---------------------

	//Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask		 = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable		 = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp		 = VK_BLEND_OP_ADD;		 // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp		 = VK_BLEND_OP_ADD;      // Optional


	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional



	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional



	//Dynamic state
	//-------------
	std::vector<VkDynamicState> dynStateCollection;
	dynStateCollection.push_back(VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT);
	dynStateCollection.push_back(VkDynamicState::VK_DYNAMIC_STATE_SCISSOR);

	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
	pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineDynamicStateCreateInfo.pNext = nullptr;
	pipelineDynamicStateCreateInfo.flags = 0;
	pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<std::uint32_t>(dynStateCollection.size());
	pipelineDynamicStateCreateInfo.pDynamicStates = dynStateCollection.data();



	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; // Used for Uniform Buffers
	pipelineLayoutInfo.pSetLayouts = &this->descriptorSetLayout; // Used for Uniform Buffers
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	
	VkResult res = vkCreatePipelineLayout(this->logicalDevice, &pipelineLayoutInfo, nullptr, &this->pipelineLayout);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create the pipeline layout!");
	}


	// ====================================
	// Create Pipeline
	// ====================================


	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &pipelineDynamicStateCreateInfo; // Optional
	pipelineInfo.layout = this->pipelineLayout;
	pipelineInfo.renderPass = this->renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	res = vkCreateGraphicsPipelines(this->logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->graphicsPipeline);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create the graphics pipeline");
	}

}

void VulkanGraphicsPipeline::init_renderpass()
{
	// Attachment description
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format		   = this->swapchainImageFormat.format;
	colorAttachment.samples		   = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


	//Subpasses and attachment references
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	// -------------------------------------------

	// Depth attachement for depth buffering
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = VulkanUtils::FindDepthFormat(this->physicalDevice);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;


	// -------------------------------------------


	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;



	VkResult res = vkCreateRenderPass(this->logicalDevice, &renderPassInfo, nullptr, &this->renderPass);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create renderpass");
	}
	
}

//void VulkanGraphicsPipeline::init_framebuffer()
//{
//	uint32_t swapchainImageCount = static_cast<uint32_t>(this->swapchainImageCollection.size());
//
//	this->swapchainFramebufferCollection.resize(swapchainImageCount);
//
//	for (size_t i = 0; i < swapchainImageCount; i++) {
//		
//		std::array<VkImageView, 2> attachments = {
//			this->swapchainImageCollection.at(i).imageView,
//			this->depthImageView
//		};
//
//		VkFramebufferCreateInfo framebufferInfo = {};
//		framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//		framebufferInfo.renderPass				= this->renderPass;
//		framebufferInfo.attachmentCount			= static_cast<uint32_t>(attachments.size());
//		framebufferInfo.pAttachments		    = attachments.data();
//		framebufferInfo.width					= this->swapchainExtent2D.width;
//		framebufferInfo.height					= this->swapchainExtent2D.height;
//		framebufferInfo.layers					= 1;
//		
//		VkResult res = vkCreateFramebuffer(this->logicalDevice, &framebufferInfo, nullptr, &this->swapchainFramebufferCollection[i]);
//		if (res != VK_SUCCESS) {
//			qDebug() << "Failed to create the framebuffer";
//		}
//	}
//
//}

