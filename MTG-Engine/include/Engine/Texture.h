#pragma once

class Texture
{
public:
	Texture() = default;
	~Texture() = default;

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	bool initialize(const char* texturePath, bool flipVertically = true);
	void bind(unsigned int unit) const;
	void shutdown();

private:
	unsigned int textureID = 0;
	int width = 0;
	int height = 0;
	int nrChannels = 0;
};