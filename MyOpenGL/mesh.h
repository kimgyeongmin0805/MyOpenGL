#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string name;
};

class Mesh {
public:
	unsigned int VAO;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
		: vertices_(vertices), indices_(indices), textures_(textures) {
		setupMesh();
	}
	void Draw(Shader& shader) {
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < textures_.size(); i++) {
			std::string type = textures_[i].type;
			std::string name;
			if (type == "texture_diffuse") {
				name = type + std::to_string(diffuseNr++);
			}
			if (type == "texture_specular") {
				name = type + std::to_string(specularNr++);
			}
			if (type == "texture_normal") {
				name = type + std::to_string(normalNr++);
			}
			if (type == "texture_height") {
				name = type + std::to_string(heightNr++);
			}

			unsigned int location = glGetUniformLocation(shader.ID, name.c_str());
			if (location != -1) {
				glUniform1i(location, i);
			}
			else {
				// std::cout << "ERROR::MESH::Failed to get Uniform Location of name: " << name << std::endl;
			}
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures_[i].id);
		}
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

private:
	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indices_;
	std::vector<Texture> textures_;
	unsigned int VBO;
	unsigned int VEO;
	
	void setupMesh() {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &VEO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VEO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
		glEnableVertexAttribArray(6);

		glBindVertexArray(0);
	}
};

#endif	// MESH_H
