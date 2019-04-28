#pragma once

#include <vector>

#include "ZiMesh.h"
#include "ZiEntity.h"

class ZiScene {

public:
	ZiScene(std::vector<std::shared_ptr<ZiEntity>> entityCollection);
	ZiScene(ZiEntity singleEntity);

	ZiScene();
	~ZiScene();

	std::vector<std::shared_ptr<ZiEntity>> GetEntityCollection();

	void AddEntity(std::shared_ptr<ZiEntity> entity);
	void AddEntity(std::shared_ptr<ZiMesh> meshEntity);


private:
	std::vector<std::shared_ptr<ZiEntity>> entityCollection;
};