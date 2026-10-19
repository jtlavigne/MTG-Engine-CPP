#include "Engine/Renderer.h"
#include "Engine/Mesh.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "Engine/Transform.h"

#include <glad/glad.h>

bool Renderer::initialize()
{
	// Enable depth testing for 3D rendering
	glEnable(GL_DEPTH_TEST);
	return true;
}

void Renderer::beginFrame()
{
	// Clear the screen with a specific color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(
	const Mesh& mesh, 
	const Shader& shader,
	const Texture& texture,
	const Transform& transform)
{
	shader.use();

	shader.setMat4("model", transform.getModelMatrix());

	texture.bind(0);
	shader.setInt("texture1", 0);

	mesh.draw();
}
