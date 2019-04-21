#pragma once

#include <vector>

#include "ZiMesh.h"

class ZiScene {

public:
	ZiScene(std::vector<std::shared_ptr<ZiMesh>> meshCollection);
	ZiScene(ZiMesh singleMesh);
	ZiScene();
	~ZiScene();

	std::vector<std::shared_ptr<ZiMesh>> GetMeshCollection();
	
	void AddMesh(std::shared_ptr<ZiMesh> mesh);

private:
	std::vector<std::shared_ptr<ZiMesh>> meshCollection;
};