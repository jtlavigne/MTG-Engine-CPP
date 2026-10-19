#include "Engine/Transform.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Transform::getModelMatrix() const
{
	glm::mat4 model = glm::mat4(1.0f); // Identity matrix

	// Apply translation
	model = glm::translate(model, position);

	// Apply rotation (convert degrees to radians)
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Apply scaling
	model = glm::scale(model, scale);

	return model;
}