#include "ZiScene.h"

ZiScene::ZiScene() {}

ZiScene::ZiScene(std::vector<std::shared_ptr<ZiMesh>> meshCollection)
{
	this->meshCollection = meshCollection;
}

ZiScene::ZiScene(ZiMesh singleMesh)
{
	this->meshCollection.push_back(std::make_shared<ZiMesh>(singleMesh));
}

ZiScene::~ZiScene()
{

}

void ZiScene::AddMesh(std::shared_ptr<ZiMesh> mesh) {
	this->meshCollection.push_back(mesh);
}

std::vector<std::shared_ptr<ZiMesh>> ZiScene::GetMeshCollection()
{
	return this->meshCollection;
}
