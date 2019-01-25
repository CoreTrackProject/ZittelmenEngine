#include "VulkanBase.h"


VulkanBase::VulkanBase()
{
	this->enableValidation = true;
	this->instance = new VulkanInstance(this->enableValidation);

	if (this->enableValidation) {
		this->vulkanDebug = new VulkanDebug(this->instance->getInstance());
	}

	this->vulkanDevice = new VulkanDevice(this->instance->getInstance());
}

VulkanBase::~VulkanBase()
{

	delete this->vulkanDevice;

	if (this->enableValidation) {
		delete this->vulkanDebug;
	}

	delete this->instance;


}
