#ifndef DATARUNNING_MESH_H
#define DATARUNNING_MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>

class Mesh
{

public:

	Mesh(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices);
	void Draw();

private:
	unsigned int VAO, VBO, EBO;
	GLsizei m_indicesSize;
	std::vector<glm::vec3> m_vertices;
	std::vector<unsigned int> m_indices;

	void setupMesh();

};

#endif