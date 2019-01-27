#include "ZittelmenEngine.h"

ZittelmenEngine::ZittelmenEngine()
{
	//QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));


	//this->m_vulkanInstance.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");


	//if (!this->m_vulkanInstance.create()) {
	//	qFatal("Failed to create Vulkan instance: %d", this->m_vulkanInstance.errorCode());
	//}


	//this->m_vulkanWindow = new VulkanWindow;
	//this->m_vulkanWindow->setVulkanInstance(&this->m_vulkanInstance);

	// Qt3d renderer
	//this->renderer = new Qt3DWindow();

	this->m_vulkanBase = new VulkanBase();

}

ZittelmenEngine::~ZittelmenEngine()
{
	delete this->m_vulkanBase;
}

void ZittelmenEngine::setTargetRenderSurface(QWidget *targetRenderSurface)
{
	this->targetRenderSurface = targetRenderSurface;
	this->m_vulkanBase->setTargetRenderSurface(targetRenderSurface);
}

void ZittelmenEngine::initVulkanRenderer()
{
	this->m_vulkanBase->init();
}
