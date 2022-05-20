#include "Mesh.h"
#include <utility>

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices) :
	m_vertices(vertices),
	m_indices(indices)
{

	m_indicesSize = static_cast<GLsizei>(indices.size());
	setupMesh();
}

void Mesh::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, m_indicesSize, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);

	glBindVertexArray(0);
}
