#include "VulkanFactory.h"

VulkanFactory::VulkanFactory(VkPhysicalDevice &physicalDev, VkDevice &logicalDev) : 
	physicalDev(physicalDev), logicalDev(logicalDev) {


}

VulkanFactory::~VulkanFactory() {
	

}