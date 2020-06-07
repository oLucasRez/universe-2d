#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0) {
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader() {
	glCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream vertexSource, fragmentSource;
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			switch (type) {
			case ShaderType::VERTEX:
				vertexSource << line << '\n'; break;
			case ShaderType::FRAGMENT:
				fragmentSource << line << '\n'; break;
			}
		}
	}

	return { vertexSource.str(), fragmentSource.str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
	glCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	glCall(glShaderSource(id, 1, &src, nullptr));
	glCall(glCompileShader(id));

	int result;
	glCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	if (result == GL_FALSE) {
		int length;
		glCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)_malloca(length * sizeof(char));
		glCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "compile " << (type == GL_VERTEX_SHADER
			? "vertex" : "fragment") << " shader error" << std::endl;
		std::cout << message << std::endl;
		glCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(
	const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glCall(glAttachShader(program, vs));
	glCall(glAttachShader(program, fs));
	glCall(glLinkProgram(program));
	glCall(glValidateProgram(program));

	glCall(glDeleteShader(vs));
	glCall(glDeleteShader(fs));

	return program;
}

void Shader::Bind() const {
	glCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const {
	glCall(glUseProgram(0));
}

void Shader::SetUniform4f(
	const std::string& name, float v0, float v1, float v2, float v3) {
	glCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

unsigned int Shader::GetUniformLocation(const std::string& name) {
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	glCall(unsigned int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	
	m_UniformLocationCache[name] = location;
	return location;
}