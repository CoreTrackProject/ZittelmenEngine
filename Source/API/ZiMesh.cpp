#include "ZiMesh.h"

ZiMesh::ZiMesh() {}

ZiMesh::ZiMesh(std::vector<VulkanVertex> vertexCollection, std::vector<std::uint32_t> indexCollection, std::shared_ptr<ZiTexture> meshTexture)
{
	this->vertexCollection = vertexCollection;
	this->indexCollection  = indexCollection;
	this->meshTexture      = meshTexture;
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

std::vector<uint32_t> ZiMesh::GetQuadVertexIndexCollection() {
	std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
	};
	return indices;
}

std::shared_ptr<ZiMesh> ZiMesh::LoadOBJ(std::string meshFile, std::string textureFile)
{
	Assimp::Importer fileImporter;

	int aiFlags = 0;
	const aiScene *scene = fileImporter.ReadFile(meshFile, aiFlags);

	// Check if the scene has meshes
	if (!scene && !scene->HasMeshes()) {
		return std::make_shared<ZiMesh>();
	}

	// TODO: Paralellize (Intel TBB? OpenMP?)
	
	// Get vertices 
	std::vector<VulkanVertex> vertexCollection;
	std::vector<std::uint32_t> indices;
	
	// Iterate through all meshes and extract for each mesh all vertecies
	for (std::uint32_t i = 0; i < scene->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[i];
		
		vertexCollection.reserve(mesh->mNumVertices);

		// Extract 
		for (std::uint32_t j = 0; j < mesh->mNumVertices; j++) {
			VulkanVertex vertex;
			{
				
				vertex.pos.x = mesh->mVertices[j].x;
				vertex.pos.y = mesh->mVertices[j].y;
				vertex.pos.z = mesh->mVertices[j].z;

				// Extract vertex colors
				if (mesh->HasVertexColors(0)) {
					vertex.color.r = mesh->mColors[j]->r;
					vertex.color.g = mesh->mColors[j]->g;
					vertex.color.b = mesh->mColors[j]->b;
					// No alpha atm
					//vertex.color.a = mesh->mColors[j]->a;

				} else {
					vertex.color = { 1.0f, 1.0f, 1.0f };
				}

				// Extract uv coordinates
				if (mesh->HasTextureCoords(0)) {
					vertex.texCoord.x = mesh->mTextureCoords[0][j].x;
					// Flip y coordinate because of vulkan
					vertex.texCoord.y = 1.0f - mesh->mTextureCoords[0][j].y;
				}


			}
			vertexCollection.emplace_back(vertex);
		}

		indices.reserve(mesh->mNumFaces);

		// Get indices
		for (std::uint32_t j = 0; j < mesh->mNumFaces; ++j) {
			aiFace face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; ++k) { 
				indices.emplace_back(face.mIndices[k]); 
			}
		}
	}


	std::shared_ptr<ZiTexture> texture = std::make_shared<ZiTexture>(textureFile);


	return std::make_shared<ZiMesh>(vertexCollection, indices, texture);
}

size_t ZiMesh::GetSize()
{
	return this->vertexCollection.size();
}

std::vector<VulkanVertex> ZiMesh::GetVertexCollection()
{
	return this->vertexCollection;
}

std::vector<std::uint32_t> ZiMesh::GetIndexCollection()
{
	return this->indexCollection;
}

std::shared_ptr<ZiTexture> ZiMesh::GetTexture()
{
	return this->meshTexture;
}

void ZiMesh::AddVertex(VulkanVertex vertex)
{
	this->vertexCollection.push_back(vertex);
}

void ZiMesh::AddIndex(std::uint32_t index)
{
	this->indexCollection.push_back(index);
}

EntityType ZiMesh::GetEntityType()
{
	return EntityType::ET_Mesh;
}

