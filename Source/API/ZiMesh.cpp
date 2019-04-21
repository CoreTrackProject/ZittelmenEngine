#include "ZiMesh.h"

ZiMesh::ZiMesh(std::vector<VulkanVertex> vertexCollection, std::vector<std::uint16_t> indexCollection, ZiTexture meshTexture)
{
	this->vertexCollection = vertexCollection;
	this->indexCollection = indexCollection;
	this->meshTexture = std::make_shared<ZiTexture>(meshTexture);
}

ZiMesh::~ZiMesh()
{

}

std::vector<VulkanVertex> ZiMesh::GetQuadVertexCollection() {
	std::vector<VulkanVertex> vertices = {
				{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
				{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
				{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

				{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
				{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
				{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};
	return vertices;
}

std::vector<uint16_t> ZiMesh::GetQuadVertexIndexCollection() {
	std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
	};
	return indices;
}

size_t ZiMesh::GetSize()
{
	return this->vertexCollection.size();
}

std::vector<VulkanVertex> ZiMesh::GetVertexCollection()
{
	return this->vertexCollection;
}

std::vector<std::uint16_t> ZiMesh::GetIndexCollection()
{
	return this->indexCollection;
}

std::shared_ptr<ZiTexture> ZiMesh::GetTexture()
{
	return this->meshTexture;
}
