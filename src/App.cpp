#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath) {
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

static unsigned int CompileShader(unsigned int type, const std::string& source) {
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

static unsigned int CreateShader(
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

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "glew error" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		// positions contem informações de 3 vertex (2 floats por vertex)
		float positions[] = {
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.5f, 0.5f,
			-0.5f, 0.5f,
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		unsigned int vao;
		glCall(glGenVertexArrays(1, &vao));
		glCall(glBindVertexArray(vao));

		VertexBuffer vb(positions, 8 * sizeof(float));

		glCall(glEnableVertexAttribArray(0));
		glCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		IndexBuffer ib(indices, 6);

		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
		glCall(glUseProgram(shader));

		glCall(int location = glGetUniformLocation(shader, "uColor"));
		ASSERT(location != -1)
			glCall(glUniform4f(location, 0.8f, 0.4f, 0.2f, 1.0f));

		glCall(glBindVertexArray(0));
		glCall(glUseProgram(0));
		glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		float r = 0.0f;
		float i = 0.05f;
		// loop eterno até que se feche a janela
		while (!glfwWindowShouldClose(window))
		{
			glCall(glClear(GL_COLOR_BUFFER_BIT));

			glCall(glUseProgram(shader));
			glCall(glUniform4f(location, r, 0.4f, 0.2f, 1.0f));

			glCall(glBindVertexArray(vao));
			ib.Bind();

			glCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			if (r > 1.0f) i = -0.05f;
			else if (r < 0.0f) i = 0.05;

			r += i;

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glCall(glDeleteProgram(shader));
	}
	glfwTerminate();
	return 0;
}