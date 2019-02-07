#include "ZittelmenEngine.h"

ZittelmenEngine::ZittelmenEngine()
{
	this->m_vulkanBase.reset(new VulkanBase());
	this->isInitialized = false;
}

ZittelmenEngine::~ZittelmenEngine()
{
	this->destroy();
}

void ZittelmenEngine::setTargetRenderSurface(QWidget *targetRenderSurface)
{
	this->m_vulkanBase->setTargetRenderSurface(targetRenderSurface);
}

void ZittelmenEngine::renderFrame() 
{
	if (this->isInitialized) {
		this->m_vulkanBase->renderFrame();
	}
}

bool ZittelmenEngine::getInitializedStatus() {
	return this->isInitialized;
}

void ZittelmenEngine::initialize()
{
	if (!this->isInitialized) {
		this->m_vulkanBase->initialize();
		this->isInitialized = true;
	}
}

void ZittelmenEngine::destroy()
{
	if (this->isInitialized) {
		this->isInitialized = false;
		this->m_vulkanBase->destroy();
	}
}

void ZittelmenEngine::resize(uint32_t width, uint32_t height)
{
	this->m_vulkanBase->resizeTargetRenderSurface(width, height);
}
