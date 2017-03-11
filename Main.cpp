#include <stdlib.h>
#include <iostream>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLFWwindow* setup(int*width, int*height);

//rectangle data
GLfloat vertices[] = {
	-0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
	0.5f, -0.5f, 1.0f, 1.0f, 1.0f
};

//shaders
static const GLchar* vertex_shader_text =
"#version 150 core\n"
"in vec2 position;"
"in vec3 color;"
"out vec3 Color;"
"void main()"
"{"
"Color = color;"
"gl_Position = vec4(position, 0.0, 1.0);"
"};";

static const GLchar* fragment_shader_text =
"#version 150 core\n"
"in vec3 Color;"
"out vec4 outColor;"
"void main()"
"{"
"outColor = vec4(Color, 1.0);"
"};";

int main() {
	int width, height;
	GLuint vbo, vao, ebo, vertex_shader, fragment_shader, program;
	GLint position_location, color_location;

	GLFWwindow *window = setup(&width, &height);

	if (!window) {
		return EXIT_FAILURE;
	}

	//VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//EBO
	glGenBuffers(1, &ebo);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 1
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	//preparing shaders & program
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glBindFragDataLocation(program, 0, "outColor");
	glLinkProgram(program);
	glUseProgram(program);

	//setting data layout
	position_location = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(position_location);
	glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5,  0);

	color_location = glGetAttribLocation(program, "color");
	glEnableVertexAttribArray(color_location);
	glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*) (2* sizeof(GLfloat)));

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}

GLFWwindow* setup(int*width, int*height) {
	if (!glfwInit()) {
		std::cout << "FAILED to initialize GLFW";
		return NULL;
	}

	glfwSetErrorCallback(error_callback);


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow *window = glfwCreateWindow(500, 500, "Opengl example", NULL, NULL);

	if (!window) {
		glfwTerminate();
		std::cout << "FAILED to create window or opengl context" << std::endl;
		return NULL;
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSetKeyCallback(window, key_callback);


	glfwGetFramebufferSize(window, width, height);
	glViewport(0, 0, *width, *height);

	glfwSwapInterval(1);

	return window;
}

static void error_callback(int error, const char* description) {
	std::cout << "Error" << error << ": " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}