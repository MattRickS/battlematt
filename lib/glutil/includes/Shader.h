#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


std::string LoadFile(const char* filename);
GLuint CompileShader(const char* source, GLenum shaderType);
GLuint CompileProgram(GLuint vertexShader, GLuint fragmentShader);
GLuint LoadShadersProgram(const char* vertexShaderFile, const char* fragmetnShaderFile);

class Shader
{
public:
	GLuint ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();
	// Utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat3(const std::string& name, float x, float y, float z) const;
	void setVec3(const std::string& name, glm::vec3 vec) const;
	void setFloat4(const std::string& name, float x, float y, float z, float w) const;
	void setMat4(const std::string& name, glm::mat4 matrix) const;
private:
	GLuint getLocation(const std::string& name) const;
};


std::string LoadFile(const char* filename)
{
	std::string code;
	std::ifstream stream;
	stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	try
	{
		stream.open(filename);
		std::stringstream sstr;
		sstr << stream.rdbuf();
		stream.close();
		code = sstr.str();
	}
	catch(std::ifstream::failure const&)
	{
		std::cout << "Unable to open " << filename << std::endl;
	}

	return code;
}

GLuint CompileShader(const char* source, GLenum shaderType)
{
	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &source, NULL);
	glCompileShader(shaderID);

	GLint success;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cout << "Error compiling shader:" << std::endl << infoLog << std::endl;
		return 0;
	}

	return shaderID;
}

GLuint CompileProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	GLint success;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		std::cout << "Error linking program:" << std::endl << infoLog << std::endl;
		return 0;
	}

	return programID;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexSource = LoadFile(vertexPath);
	std::string fragmentSource = LoadFile(fragmentPath);

	GLuint vertexShader = CompileShader(vertexSource.c_str(), GL_VERTEX_SHADER);
	GLuint fragmentShader = CompileShader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);

	ID = CompileProgram(vertexShader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use()
{
	glUseProgram(ID);
}

// Utility uniform functions
GLuint Shader::getLocation(const std::string& name) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1)
	{
		std::cout << "Shader uniform not found: " << name << std::endl;
	}
	return location;
}


void Shader::setBool(const std::string& name, bool value) const
{
	GLuint location = getLocation(name);
	glUniform1i(location, (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	GLuint location = getLocation(name);
	glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	GLuint location = getLocation(name);
	glUniform1f(location, value);
}

void Shader::setFloat3(const std::string& name, float x, float y, float z) const
{
	GLuint location = getLocation(name);
	glUniform3f(location, x, y, z);
}

void Shader::setVec3(const std::string& name, glm::vec3 vec) const
{
	GLuint location = getLocation(name);
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::setFloat4(const std::string& name, float x, float y, float z, float w) const
{
	GLuint location = getLocation(name);
	glUniform4f(location, x, y, z, w);
}

void Shader::setMat4(const std::string& name, glm::mat4 matrix) const
{
	GLuint location = getLocation(name);
	// Location, Number of Matrices, Transpose?, matrices
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
