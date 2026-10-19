#include "Engine/Shader.h"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

bool Shader::initialize(const char* vertexPath, const char* fragmentPath)
{
	if (ID != 0)
	{
		std::cerr << "Shader program already initialized. Call shutdown() before reinitializing.\n";
		return false; // Already initialized
	}
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (const std::ifstream::failure& e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: "
				  << e.what() << std::endl;
		return false;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	if (vertex == 0)
	{
		std::cerr << "ERROR::SHADER::VERTEX::CREATION_FAILED\n";
		return false;
	}

	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" 
				  << infoLog << std::endl;

		glDeleteShader(vertex); // Clean up the vertex shader
		return false;
	}

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragment == 0)
	{
		std::cerr << "ERROR::SHADER::FRAGMENT::CREATION_FAILED\n";
		glDeleteShader(vertex); // Clean up the vertex shader
		return false;
	}

	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;

		glDeleteShader(vertex); // Clean up the vertex shader
		glDeleteShader(fragment); // Clean up the fragment shader
		return false;
	}

	ID = glCreateProgram();
	if (ID == 0)
	{
		std::cerr << "ERROR::SHADER::PROGRAM::CREATION_FAILED\n";
		glDeleteShader(vertex); // Clean up the vertex shader
		glDeleteShader(fragment); // Clean up the fragment shader
		return false;
	}

	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		shutdown(); // Clean up the program ID
		return false;
	}

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return true;
}

void Shader::use() const
{
	if (ID != 0)
	{
		glUseProgram(ID);
	}
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::shutdown()
{
	if (ID != 0)
	{
		glDeleteProgram(ID);
		ID = 0;
	}
}