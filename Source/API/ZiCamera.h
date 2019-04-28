#pragma once

#include "ZiEntity.h"

class ZiCamera : ZiEntity {

public:
	ZiCamera();
	~ZiCamera();

	EntityType GetEntityType();
};