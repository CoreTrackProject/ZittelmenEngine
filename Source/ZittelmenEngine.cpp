#include "ZittelmenEngine.h"

ZittelmenEngine::ZittelmenEngine()
{
	QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));


	this->m_vulkanInstance.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");
	if (!this->m_vulkanInstance.create()) {
		qFatal("Failed to create Vulkan instance: %d", this->m_vulkanInstance.errorCode());
	}


	this->m_vulkanWindow = new VulkanWindow;
	this->m_vulkanWindow->setVulkanInstance(&this->m_vulkanInstance);

}

VulkanWindow* ZittelmenEngine::getVulkanWindow()
{
	return  this->m_vulkanWindow;
}
