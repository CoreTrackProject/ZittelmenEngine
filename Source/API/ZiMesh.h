#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <string>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanVertex.hpp"

#include "ZiTexture.h"

class ZiMesh {

public:
	ZiMesh();
	ZiMesh(std::vector<VulkanVertex> vertexCollection, std::vector<std::uint32_t> indexCollection, std::shared_ptr<ZiTexture> meshTexture);
	~ZiMesh();
	
	size_t GetSize();
	std::vector<VulkanVertex> GetVertexCollection();
	std::vector<std::uint32_t> GetIndexCollection();
	std::shared_ptr<ZiTexture> GetTexture();

	void AddVertex(VulkanVertex vertex);
	void AddIndex(std::uint32_t index);

public:
	static std::vector<VulkanVertex> GetQuadVertexCollection();
	static std::vector<uint32_t> GetQuadVertexIndexCollection();

	static std::shared_ptr<ZiMesh> LoadOBJ(std::string meshFile, std::string textureFile);
	

private:
	std::vector<VulkanVertex> vertexCollection;
	std::vector<std::uint32_t> indexCollection;
	std::shared_ptr<ZiTexture> meshTexture;

};