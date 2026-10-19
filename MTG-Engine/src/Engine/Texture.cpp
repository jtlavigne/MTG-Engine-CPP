#include "Engine/Texture.h"
#include "Engine/stb_image.h"

#include <glad/glad.h>
#include <iostream>

bool Texture::initialize(const char* texturePath, bool flipVertically)
{
	if (textureID != 0)
	{
		return false; // Already initialized
	}
	glGenTextures(1, &textureID);
	if (textureID == 0)
	{
		std::cerr << "Failed to generate texture ID\n";
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
	// Set texture wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(flipVertically);
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		switch (nrChannels)
		{
			case 1:
				format = GL_RED;
				break;
			case 2:
				format = GL_RG;
				break;
			case 3:
				format = GL_RGB;
				break;
			case 4:
				format = GL_RGBA;
				break;
			default:
				std::cerr << "Unsupported number of channels: " 
						  << nrChannels << std::endl;
				stbi_image_free(data);
				shutdown(); // Clean up the texture ID
				return false;
		}
		GLint previousAlignment;
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment); // Save previous alignment

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Set pixel alignment

		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			format, 
			width, 
			height, 
			0, 
			format, 
			GL_UNSIGNED_BYTE, 
			data
		);

		glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment); // Restore previous alignment

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
		stbi_image_free(data);
		return true;
	}
	else
	{
		std::cerr << "Failed to load texture: " 
				  << texturePath << std::endl;

		shutdown(); // Clean up the texture ID
		return false;
	}
}

void Texture::bind(unsigned int unit) const
{
	GLint maxTextureUnits = 0;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	if (unit >= static_cast<unsigned int>(maxTextureUnits))
	{
		std::cerr << "Texture unit " << unit
			<< " is out of range (0-" << maxTextureUnits - 1 << ")" << std::endl;
		return;
	}
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::shutdown()
{
	if (textureID != 0)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
	width = 0;
	height = 0;
	nrChannels = 0;
}