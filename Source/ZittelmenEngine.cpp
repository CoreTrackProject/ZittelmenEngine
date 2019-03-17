#include "ZittelmenEngine.h"

ZittelmenEngine::ZittelmenEngine()
{
	this->vulkanController.reset(new VulkanController());
	this->isInitialized = false;
}

ZittelmenEngine::~ZittelmenEngine()
{
	if (this->isInitialized) {
		this->destroy();
	}
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

bool ZittelmenEngine::getInitializedStatus() {
	return this->isInitialized;
}

void ZittelmenEngine::setTargetRenderSurface(QWidget *targetRenderSurface)
{
	this->vulkanController->setTargetRenderSurface(targetRenderSurface->winId());
}

void ZittelmenEngine::setTargetRenderSurface(QWindow *targetRenderSurface)
{
	this->vulkanController->setTargetRenderSurface(targetRenderSurface->winId());
}

void ZittelmenEngine::renderFrame() 
{
	if (this->isInitialized) {
		this->vulkanController->renderFrame();
	}
}

void ZittelmenEngine::resize(uint32_t width, uint32_t height)
{
	// Prevent rendering a frame during reinitialization
	this->isInitialized = false;
	this->vulkanController->resizeTargetRenderSurface(width, height);
	this->isInitialized = true;
}
