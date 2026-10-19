#pragma once

#include <memory>

#include "Engine/Renderer.h"
#include "Engine/Mesh.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "Engine/Transform.h"
#include "Engine/Camera2D.h"

struct GLFWwindow;

class Application
{
public:
	Application(unsigned int width, unsigned int height,
		const char* title);

	~Application();

	bool initialize();
	void run();

private:
	void processInput();
	void render();

	static void framebufferSizeCallback(
		GLFWwindow* window, int width, int height);

	static void scrollCallback(
		GLFWwindow* window, double xoffset, double yoffset);

	GLFWwindow* window = nullptr;

	unsigned int width;
	unsigned int height;
	const char* title;

	Renderer renderer;
	Shader shader;
	Texture tableTexture;
	Transform tableTransform;
	Texture cardTexture;
	Transform cardTransform;
	Camera2D camera;

	bool isDragging = false;
	double lastMouseX = 0.0;
	double lastMouseY = 0.0;
	
	std::unique_ptr<Mesh> rectangleMesh;
};