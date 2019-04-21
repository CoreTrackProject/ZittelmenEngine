#include "ZiTexture.h"


ZiTexture::ZiTexture(std::string imagePath)
{
	this->imageTexture = std::make_shared<QImage>(QImage(imagePath.c_str()));
}

ZiTexture::~ZiTexture()
{

}

std::shared_ptr<QImage> ZiTexture::GetImage()
{
	return this->imageTexture;
}
