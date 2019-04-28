#pragma once

enum EntityType {
	ET_None,
	ET_Mesh,
	ET_Camera
};

class ZiEntity {

public:
	ZiEntity();
	~ZiEntity();


	virtual EntityType GetEntityType() {
		return EntityType::ET_None;
	}

};