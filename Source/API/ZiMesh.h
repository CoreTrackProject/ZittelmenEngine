#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include <glm/glm.hpp>

#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanVertex.hpp"

#include "ZiTexture.h"

class ZiMesh {

public:
	ZiMesh(std::vector<VulkanVertex> vertexCollection, std::vector<std::uint16_t> indexCollection, ZiTexture meshTexture);
	~ZiMesh();
	
	size_t GetSize();
	std::vector<VulkanVertex> GetVertexCollection();
	std::vector<std::uint16_t> GetIndexCollection();
	std::shared_ptr<ZiTexture> GetTexture();

public:
	static std::vector<VulkanVertex> GetQuadVertexCollection();
	static std::vector<uint16_t> GetQuadVertexIndexCollection();

private:
	std::vector<VulkanVertex> vertexCollection;
	std::vector<std::uint16_t> indexCollection;
	std::shared_ptr<ZiTexture> meshTexture;

};