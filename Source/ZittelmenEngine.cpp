#include "ZittelmenEngine.h"

ZittelmenEngine::ZittelmenEngine()
{
	this->isInitialized = false;
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

bool ZittelmenEngine::initialized() {
	return this->isInitialized;
}

void ZittelmenEngine::resize(uint32_t width, uint32_t height)
{
	if (this->isInitialized) {
		this->m_vulkanBase->resizeTargetRenderSurface(width, height);
	}
}