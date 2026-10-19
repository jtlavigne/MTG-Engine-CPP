#pragma once

#include <vector>

struct Vertex {
	float x, y, z; // Position
	float r, g, b; // Color
	float u, v;    // Texture coordinates
};

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, 
		const std::vector<unsigned int>& indices);

	~Mesh();

	bool initialize();
	void draw() const;
	void shutdown();

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
};