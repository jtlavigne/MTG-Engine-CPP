#pragma once

#include <glm/glm.hpp>

class Camera2D
{
public:
	glm::vec2 position{ 0.0f, 0.0f };

	float zoom{ 1.0f }; // Zoom level, where 1.0 is the default scale
	float baseHeight = 10.0f; // Base height for zoom calculations

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix(float aspectRatio) const;

	void move(const glm::vec2& delta);
	void changeZoom(float zoomFactor);
	void setBounds(const glm::vec2& minBounds, const glm::vec2& maxBounds);
	void clampPosition(float aspectRatio);
private:
	// You can add private members here if needed in the future
	glm::vec2 boundsMin{ -14.0f, -7.0f }; // Minimum bounds for camera position
	glm::vec2 boundsMax{ 14.0f, 7.0f }; // Maximum bounds for camera position
};