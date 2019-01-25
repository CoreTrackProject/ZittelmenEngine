#include "VulkanBase.h"


VulkanBase::VulkanBase()
{
	this->enableValidation = true;

	this->m_instance = new VulkanInstance(this->enableValidation);

	if (this->enableValidation) {
		this->m_vulkanDebug = new VulkanDebug(this->m_instance->getInstance());
	}

}

VulkanBase::~VulkanBase()
{
	delete this->m_instance;

	if (this->enableValidation) {
		delete this->m_vulkanDebug;
	}


}
