#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <string>

#include <glm/glm.hpp>
#include "Vulkan/VulkanTexture.h"

#include <QImage>

class ZiTexture {

public:
	ZiTexture(std::string imagePath);
	~ZiTexture();
	
	std::shared_ptr<QImage> GetImage();

private:
	std::shared_ptr<QImage> imageTexture;
};