#include "ZiCamera.h"

ZiCamera::ZiCamera() {}

ZiCamera::~ZiCamera() {}

EntityType ZiCamera::GetEntityType()
{
	return EntityType::ET_Mesh;
}