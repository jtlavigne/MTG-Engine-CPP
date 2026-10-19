#include "Engine/Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Application::Application(unsigned int width, unsigned int height,
	const char* title)
	: width(width), height(height), title(title)
{
}

Application::~Application()
{
	if (window)
	{
		glfwMakeContextCurrent(window);

		if (rectangleMesh)
		{
			rectangleMesh->shutdown();
			rectangleMesh.reset();
		}

		shader.shutdown();
		tableTexture.shutdown();
		cardTexture.shutdown();

		glfwDestroyWindow(window);
		window = nullptr;
	}

	glfwTerminate();
}

bool Application::initialize()
{
	// initialize GLFW
	if (!glfwInit())
	{
		return false;
	}
	
	//configure GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__ // uncomment this statement to fix compilation on OS X
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window)
	{
		return false;
	}

	// set the user pointer to this instance of Application
	glfwSetWindowUserPointer(window, this);
	// make the OpenGL context current
	glfwMakeContextCurrent(window);
	
	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		glfwDestroyWindow(window);
		window = nullptr;
		return false;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// set the viewport and register the framebuffer size callback
	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

	// set the viewport to match the framebuffer size
	glViewport(0, 0, framebufferWidth, framebufferHeight);
	// register the framebuffer size callback
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	// register the scroll callback
	glfwSetScrollCallback(window, scrollCallback);

	if (!shader.initialize("shaders/shaders.vs", "shaders/shaders.fs"))
	{
		std::cerr << "Failed to initialize shader\n";
		return false;
	}

	std::vector<Vertex> vertices = {
		// positions          // colors           // texture coords
		{ 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f }, // top right
		{ 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f }, // bottom right
		{ -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f }, // bottom left
		{ -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f }  // top left 
	};

	std::vector<unsigned int> indices = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	rectangleMesh = std::make_unique<Mesh>(vertices, indices);

	if (!rectangleMesh->initialize())
	{
		std::cerr << "Failed to initialize rectangle mesh\n";
		return false;
	}

	if (!cardTexture.initialize("textures/commander-lab-card-back.png"))
	{
		std::cerr << "Failed to initialize card texture\n";
		return false;
	}

	if (!tableTexture.initialize("textures/tabletop.png"))
	{
		std::cerr << "Failed to initialize table texture\n";
		return false;
	}

	if (!renderer.initialize())
	{
		std::cerr << "Failed to initialize renderer\n";
		return false;
	}

	tableTransform.position = { 0.0f, 0.0f, -0.5f };
	tableTransform.rotation = { 0.0f, 0.0f, 0.0f };
	tableTransform.scale = { 36.0f, 21.0f, 1.0f };

	glm::vec2 tableCenter{
	tableTransform.position.x,
	tableTransform.position.y
	};

	glm::vec2 halfTableSize{
		tableTransform.scale.x * 0.5f,
		tableTransform.scale.y * 0.5f
	};
	glm::vec2 padding{ halfTableSize.x * 0.5f, halfTableSize.y * 0.5f }; // Add some padding to the bounds

	camera.setBounds(
		tableCenter - halfTableSize - padding,
		tableCenter + halfTableSize + padding
	);

	cardTransform.position = { 2.0f, 1.0f, 0.0f };
	cardTransform.rotation = { 0.0f, 0.0f, 0.0f };
	cardTransform.scale = { 0.716, 1.0f, 1.0f };

	return true;
}

void Application::run()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		processInput();

		int framebufferWidth, framebufferHeight;
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		if (framebufferHeight > 0)
		{
			float aspectRatio = static_cast<float>(framebufferWidth) / framebufferHeight;
			camera.clampPosition(aspectRatio);
		}

		render();

		glfwSwapBuffers(window);
	}
}


void Application::processInput()
{
	// handle mouse dragging for camera movement
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (!isDragging)
		{
			isDragging = true;
			lastMouseX = mouseX;
			lastMouseY = mouseY;
		}
		else
		{
			double deltaX = mouseX - lastMouseX;
			double deltaY = mouseY - lastMouseY;

			int windowWidth, windowHeight;
			glfwGetWindowSize(window, &windowWidth, &windowHeight);

			if (windowHeight > 0 && windowWidth > 0) // Prevent division by zero
			{
				float worldUnitsPerPixel = 
					camera.baseHeight /
					(camera.zoom * windowHeight);

				camera.move({
					-static_cast<float>(deltaX) * worldUnitsPerPixel,
					static_cast<float>(deltaY)* worldUnitsPerPixel
					});
			}

			lastMouseX = mouseX;
			lastMouseY = mouseY;
		}
	}
	else
	{
		isDragging = false;
	}

	// handle keyboard input
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void Application::render()
{
	int framebufferWidth, framebufferHeight;

	glfwGetFramebufferSize(
		window, 
		&framebufferWidth, 
		&framebufferHeight
	);

	if (framebufferWidth == 0 || framebufferHeight == 0)
		return; // Avoid division by zero

	float aspect = static_cast<float>(framebufferWidth) / framebufferHeight;

	glm::mat4 projection = glm::ortho(
		-5.0f * aspect,
		5.0f * aspect,
		-5.0f,
		5.0f,
		-1.0f,
		1.0f
	);

	renderer.beginFrame();

	shader.use();

	shader.setMat4(
		"projection", 
		camera.getProjectionMatrix(aspect)
	);

	shader.setMat4(
		"view", 
		camera.getViewMatrix()
	);

	if (rectangleMesh)
	{
		//draw table first
		renderer.draw(
			*rectangleMesh,
			shader,
			tableTexture,
			tableTransform);

		//draw card on top of table
		renderer.draw(
			*rectangleMesh, 
			shader, 
			cardTexture, 
			cardTransform);
	}
}

void Application::framebufferSizeCallback(
	GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions
	glViewport(0, 0, width, height);
}

void Application::scrollCallback(
	GLFWwindow* window, double xoffset, double yoffset)
{
	Application* app = static_cast<Application*>(
		glfwGetWindowUserPointer(window)
		);

	if (!app)
	{
		std::cerr << "Error: Application instance is null in scroll callback\n";
		return;
	}

	constexpr float zoomFactor = 1.1f;

	if (yoffset > 0.0)
		app->camera.changeZoom(zoomFactor);
	else if (yoffset < 0.0)
		app->camera.changeZoom(1.0f / zoomFactor);
}