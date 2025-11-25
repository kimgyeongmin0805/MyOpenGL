#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class Model {
public:
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;

	Model(const std::string& path, bool gamma = false) : gammaCorrection(false) {
		loadModel(path);
	}
	void Draw(Shader& shader) {
		for (unsigned int i = 0; i < meshes.size(); i++) {
			meshes[i].Draw(shader);
		}
	}
	
private:
	void loadModel(const std::string& path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}
	void processNode(aiNode* node, const aiScene* scene) {
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));;
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}
	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<Texture> textures;
		std::vector<unsigned int> indices;
		// vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			// position
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;
			// normal
			if (mesh->HasNormals()) {
				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;
			}
			// texture coordinates
			if (mesh->HasTextureCoords(0)) {
				vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f);
			}
			// tangent and bitangent
			if (mesh->HasTangentsAndBitangents()) {
				vertex.Tangent.x = mesh->mTangents[i].x;
				vertex.Tangent.y = mesh->mTangents[i].y;
				vertex.Tangent.z = mesh->mTangents[i].z;
				vertex.Bitangent.x = mesh->mBitangents[i].x;
				vertex.Bitangent.y = mesh->mBitangents[i].y;
				vertex.Bitangent.z = mesh->mBitangents[i].z;
			}
			else {
				vertex.Tangent = glm::vec3(0.0f);
				vertex.Bitangent = glm::vec3(0.0f);
			}
			// bone
			if (mesh->HasBones()) {
				for (unsigned int j = 0; j < MAX_BONE_INFLUENCE; j++) {
					vertex.m_BoneIDs[j] = -1;
					vertex.m_Weights[j] = 0.0f;
				}
				for (unsigned int j = 0; j < mesh->mNumBones; j++) {
					aiBone* bone = mesh->mBones[j];
					for (unsigned int k = 0; k < bone->mNumWeights; k++) {
						aiVertexWeight weight = bone->mWeights[k];
						if (weight.mVertexId == i) {
							for (int l = 0; l < MAX_BONE_INFLUENCE; l++) {
								if (vertex.m_BoneIDs[l] == -1) {
									vertex.m_BoneIDs[l] = j;
									vertex.m_Weights[l] = weight.mWeight;
									break;
								}
							}
						}
					}
				}
			}
			else {
				for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
					vertex.m_BoneIDs[j] = -1;
					vertex.m_Weights[j] = 0.0f;
				}
			}
			vertices.push_back(vertex);
		}
		// indices
		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		// textures
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> normalMaps = loadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = loadMaterialTexture(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		return Mesh(vertices, indices, textures);
	}
	std::vector<Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, std::string typeName) {
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
			aiString name;
			material->GetTexture(type, i, &name);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++) {
				if (std::string(name.C_Str()) == textures_loaded[j].name) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				Texture texture;
				texture.id = loadTexture(std::string(name.C_Str()));
				texture.type = typeName;
				texture.name = std::string(name.C_Str());
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}
		return textures;
	}
	unsigned int loadTexture(const std::string& name) {
		std::string path = directory + '/' + name;

		unsigned int texture_id;
		glGenTextures(1, &texture_id);

		int width, height, nrChannels;
		unsigned char* image = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

		if (image) {
			GLenum format;
			if (nrChannels == 1) format = GL_RED;
			if (nrChannels == 3) format = GL_RGB;
			if (nrChannels == 4) format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, texture_id);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else {
			std::cout << "ERROR::MODEL::Failed to load Texture of name: " << name << std::endl;
		}
		stbi_image_free(image);
		return texture_id;
	}
};


#endif	// MODEL_H
