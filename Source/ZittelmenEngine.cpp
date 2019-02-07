#include "ZittelmenEngine.h"

ZittelmenEngine::ZittelmenEngine()
{
	this->vulkanController.reset(new VulkanController());
	this->isInitialized = false;
}

ZittelmenEngine::~ZittelmenEngine()
{
	this->destroy();
}

void ZittelmenEngine::setTargetRenderSurface(QWidget *targetRenderSurface)
{
	this->vulkanController->setTargetRenderSurface(targetRenderSurface);
}

void ZittelmenEngine::renderFrame() 
{
	if (this->isInitialized) {
		this->vulkanController->renderFrame();
	}
}

bool ZittelmenEngine::getInitializedStatus() {
	return this->isInitialized;
}

void ZittelmenEngine::initialize()
{
	if (!this->isInitialized) {
		this->vulkanController->initialize();
		this->isInitialized = true;
	}
}

void ZittelmenEngine::destroy()
{
	if (this->isInitialized) {
		this->isInitialized = false;
		this->vulkanController->destroy();
	}
}

void ZittelmenEngine::resize(uint32_t width, uint32_t height)
{
	this->vulkanController->resizeTargetRenderSurface(width, height);
}
