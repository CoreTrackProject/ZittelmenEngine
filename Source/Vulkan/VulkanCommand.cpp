#include "VulkanCommand.h"


VulkanCommand::VulkanCommand(VkDevice* logicalDevice, DeviceInfo* deviceInfo) {
	
	this->logicalDevice = logicalDevice;
	this->deviceInfo = deviceInfo;

	this->init_commandPool();
}

VulkanCommand::~VulkanCommand() {
	vkDestroyCommandPool(*this->logicalDevice, this->commandPool, nullptr);
}

void VulkanCommand::init_commandPool()
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = this->deviceInfo->queueFamilyIndexes.graphics;
	poolInfo.flags = 0; // Optional

	VkResult res = vkCreateCommandPool(*this->logicalDevice, &poolInfo, nullptr, &this->commandPool);

}
