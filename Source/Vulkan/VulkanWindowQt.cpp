#include "VulkanWindowQt.h"


// This code is used from the qt vulkan examples

VulkanWindowQt::VulkanWindowQt()
{
	this->setSurfaceType(QSurface::VulkanSurface);
}

QVulkanWindowRenderer *VulkanWindowQt::createRenderer()
{
	return new VulkanRenderer(this);
}
