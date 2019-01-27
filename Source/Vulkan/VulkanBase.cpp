#include "VulkanBase.h"


VulkanBase::VulkanBase()
{

}

VulkanBase::~VulkanBase()
{

	delete this->swapchain;

	delete this->window;

	delete this->vulkanDevice;

	if (this->enableValidation) {
		delete this->vulkanDebug;
	}

	delete this->instance;


}

void VulkanBase::setTargetRenderSurface(QWidget* target)
{
	this->targetRenderWindow = target;
}

void VulkanBase::init()
{
	this->enableValidation = true;
	this->instance = new VulkanInstance(this->enableValidation);

	if (this->enableValidation) {
		this->vulkanDebug = new VulkanDebug(this->instance->getInstance());
	}

	this->vulkanDevice = new VulkanDevice(this->instance->getInstance());

	if (this->targetRenderWindow != nullptr) {
		this->window = new VulkanWindow(this->instance->getInstance(), this->targetRenderWindow);

		VkPhysicalDevice* tmpDev = this->vulkanDevice->getPhysicalDevice();
		VkDevice* tmpLogicalDev  = this->vulkanDevice->getLogicalDevice();
		DeviceInfo* tmpDevInfo   = this->vulkanDevice->getPhysicalDeviceInfo(tmpDev);
		VkSurfaceKHR* tmpSurface = this->window->getSurface();
		this->swapchain = new VulkanSwapchain(tmpDev, tmpLogicalDev, tmpDevInfo, tmpSurface);

	}
}
