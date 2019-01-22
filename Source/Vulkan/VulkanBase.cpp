#include "VulkanBase.h"


VulkanBase::VulkanBase()
{
	this->m_instance = new VulkanInstance(false);
}

VulkanBase::~VulkanBase()
{
	delete this->m_instance;
}
