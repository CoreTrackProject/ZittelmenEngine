#include "ZiScene.h"

ZiScene::ZiScene() {}

ZiScene::ZiScene(std::vector<std::shared_ptr<ZiEntity>> entityCollection)
{
	this->entityCollection = entityCollection;
}

ZiScene::ZiScene(ZiEntity singleEntity)
{
	this->entityCollection.push_back(std::make_shared<ZiEntity>(singleEntity));
}

ZiScene::~ZiScene()
{

}



void ZiScene::AddEntity(std::shared_ptr<ZiEntity> entity) {
	this->entityCollection.push_back(entity);
}

void ZiScene::AddEntity(std::shared_ptr<ZiMesh> entity) {
	// Convert ZiMesh to base class (ZiEntity) and then add it to the collection
	this->entityCollection.push_back(
		std::reinterpret_pointer_cast<ZiEntity, ZiMesh>(entity)
	);
}

std::vector<std::shared_ptr<ZiEntity>> ZiScene::GetEntityCollection() {
	return this->entityCollection;
}
