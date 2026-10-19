#include "Engine/Mesh.h"

#include <glad/glad.h>
#include <cstddef>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
	: vertices(vertices), indices(indices)
{
}

Mesh::~Mesh()
{
	// GPU resource cleanup
}

bool Mesh::initialize()
{
	if (VAO != 0 || VBO != 0 || EBO != 0)
	{
		return false; // Already initialized
	}

	// Generate and bind VAO, VBO, EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
	glEnableVertexAttribArray(1);
	// Texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

	if (!VAO || !VBO || !EBO)
	{
		shutdown(); // Clean up any generated buffers
		return false; // Failed to generate buffers
	}

	return true;
}

void Mesh::draw() const
{
	// protect against drawing uninitialized mesh
	if (VAO == 0 || VBO == 0 || EBO == 0)
	{
		return; // Mesh not initialized
	}

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 
		static_cast<GLsizei>(indices.size()), 
		GL_UNSIGNED_INT, 
		nullptr
	);

	glBindVertexArray(0); // Unbind VAO
}

void Mesh::shutdown()
{
	if (EBO)
	{
		glDeleteBuffers(1, &EBO);
		EBO = 0;
	}
	if (VBO)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	if (VAO)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}