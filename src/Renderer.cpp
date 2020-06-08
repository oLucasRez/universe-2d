#include "Renderer.h"
#include <iostream>

void glClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool glLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[ OpenGL ERROR ] " << error << ": " <<
			function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::Clear() const {
	glCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(
	const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {

	shader.Bind();
	va.Bind();
	ib.Bind();
	glCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}