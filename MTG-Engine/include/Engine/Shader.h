#pragma once

#include <string>
#include <glm/mat4x4.hpp>

class Shader
{
public:
    Shader() = default;
    ~Shader() = default;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool initialize(const char* vertexPath, const char* fragmentPath);
    void shutdown();

    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, const glm::mat4& matrix) const;

private:
    unsigned int ID = 0;
};