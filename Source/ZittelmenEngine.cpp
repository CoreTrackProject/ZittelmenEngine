#include "ZittelmenEngine.h"

ZittelmenEngine::ZittelmenEngine()
{
	this->isInitialized = false;

	// Old
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

	}

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
	this->isInitialized = true;
}

void ZittelmenEngine::renderFrame()
{
	if (this->isInitialized) {
		this->m_vulkanBase->renderFrame();
	}
}

void ZittelmenEngine::resize(uint32_t width, uint32_t height)
{
	this->m_vulkanBase->resizeTargetRenderSurface(width, height);
}
