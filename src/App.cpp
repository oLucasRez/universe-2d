#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static int CreateShader(
	const std::string& vertexShader, const std::string& fragmentShader) {
	
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit()) return -1;

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "glew error" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	// positions contem informações de 3 vertex (2 floats por vertex)
	float positions[6] = {
		-0.5f, -0.5f,
		0.0f, 0.5f,
		0.5f, -0.5f
	};

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);// habilita o atributo 0 do vertex
	// glVertexAttribPointer params
	// index ~> índice do atributo do vertex
	// size ~> número de valores que definirão o vector
	// type ~> tipo de dado q fornecerá
	// normalized ~> informa se o valor se encontra normalizado
	// stride ~> tamanho do atributo em bytes
	// pointer ~> offset do atributo em relação ao proprio vertex
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// loop eterno até que se feche a janela
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}