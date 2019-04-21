#include "ZiEngine.h"

ZiEngine::ZiEngine() {
	this->vulkanController = std::make_unique<VulkanController>();
	this->currStatus = ZiEngineStatus::Created;
}

ZiEngine::~ZiEngine() {
	
}

void ZiEngine::Initialize() {
	this->currStatus = ZiEngineStatus::Initializing;
	
	if (this->currScene->GetMeshCollection().size() <= 0) {
		throw new std::exception("No meshdata available");
	}


	// TMP (ATM only support for one model with one texture)
	// It always takes the first model mesh from the mesh collection
	this->vulkanController->ImportData(
		this->currScene->GetMeshCollection().at(0)->GetVertexCollection(),
		this->currScene->GetMeshCollection().at(0)->GetIndexCollection(),
		this->currScene->GetMeshCollection().at(0)->GetTexture()->GetImage()
	);


	this->vulkanController->Initialize();
	this->currStatus = ZiEngineStatus::Ready;
}

void ZiEngine::Destroy() {
	this->vulkanController->Destroy();
	this->vulkanController.reset();
	this->currStatus = ZiEngineStatus::Destroyed;
}

void ZiEngine::SetTargetRenderSurface(QWidget *targetRenderSurface) {
	this->vulkanController->SetTargetRenderSurface(targetRenderSurface->winId());
}

void ZiEngine::SetTargetRenderSurface(QWindow *targetRenderSurface) {
	this->vulkanController->SetTargetRenderSurface(targetRenderSurface->winId());
}
	
void ZiEngine::RenderFrame() {
	// Engine has to be ready for rendering
	if(this->currStatus != ZiEngineStatus::Ready) {
		return;
	}
	this->currStatus = ZiEngineStatus::Rendering;
	this->vulkanController->RenderFrame();
	this->currStatus = ZiEngineStatus::Ready;
}

void ZiEngine::Resize(std::uint32_t width, std::uint32_t height) {
	// Engine has to be ready for resizing
	if(this->currStatus != ZiEngineStatus::Ready) {
		return;
	}
	this->currStatus = ZiEngineStatus::Resizing;
	this->vulkanController->ResizeTargetRenderSurface(width, height);
	this->currStatus = ZiEngineStatus::Ready;
}

ZiEngineStatus ZiEngine::GetStatus() {
	return this->currStatus;
}

void ZiEngine::SetScene(std::shared_ptr<ZiScene> scene)
{
	this->currScene = scene;
}
