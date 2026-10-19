#include "Engine/Camera2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

glm::mat4 Camera2D::getViewMatrix() const
{
	glm::mat4 view = glm::mat4(1.0f); // Identity matrix

	view = glm::translate(
		view, 
		glm::vec3(-position, 0.0f)
	); // Translate by negative position
	
	return view;
}

glm::mat4 Camera2D::getProjectionMatrix(float aspectRatio) const
{
	float halfHeight = baseHeight / (2.0f * zoom);
	float halfWidth = halfHeight * aspectRatio;

	return glm::ortho(
		-halfWidth, halfWidth,
		-halfHeight, halfHeight,
		-10.0f, 10.0f
	);
}

void Camera2D::move(const glm::vec2& delta)
{
	position += delta;
}

void Camera2D::changeZoom(float zoomFactor)
{
	zoom = std::clamp(zoom * zoomFactor, 0.1f, 5.0f); // Prevent zoom from going below 0.1
}

void Camera2D::setBounds(const glm::vec2& minBounds, const glm::vec2& maxBounds)
{
	boundsMin = minBounds;
    boundsMax = maxBounds;
}

void Camera2D::clampPosition(float aspectRatio)
{
    float halfHeight = baseHeight / (2.0f * zoom);
    float halfWidth = halfHeight * aspectRatio;

    glm::vec2 halfView{ halfWidth, halfHeight };

    glm::vec2 minPosition = boundsMin + halfView;
    glm::vec2 maxPosition = boundsMax - halfView;

    // Horizontal bounds
    if (minPosition.x > maxPosition.x)
    {
        position.x = (boundsMin.x + boundsMax.x) * 0.5f;
    }
    else
    {
        position.x = std::clamp(
            position.x,
            minPosition.x,
            maxPosition.x
        );
    }

    // Vertical bounds
    if (minPosition.y > maxPosition.y)
    {
        position.y = (boundsMin.y + boundsMax.y) * 0.5f;
    }
    else
    {
        position.y = std::clamp(
            position.y,
            minPosition.y,
            maxPosition.y
        );
    }
}