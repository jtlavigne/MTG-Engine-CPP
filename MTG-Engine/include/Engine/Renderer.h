#pragma once

#include "SHADER.h"

class Mesh;
class Shader;
class Texture;
class Transform;

class Renderer
{
public:
	Renderer() = default;

	~Renderer() = default;

	bool initialize();

	void beginFrame();

	void draw(
		const Mesh& mesh, 
		const Shader& shader, 
		const Texture& texture,
		const Transform& transform
	);
};