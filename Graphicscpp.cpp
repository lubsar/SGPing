#include "Graphics.h"
#include <iostream>

namespace sgping {
	GLint mat_location;
	GLFWwindow *win;

	void set_renderer_up();
	void error_callback(int error, const char* description);

	GLFWwindow* setup(int *width, int *height) {
		//GLFW init
		if (!glfwInit()) {
			std::cout << "FAILED to initialize GLFW";
			return NULL;
		}

		//sets the GLFW error callback
		glfwSetErrorCallback(error_callback);

		//sets version of Opengl
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		//creates glfw window
		GLFWwindow *window = glfwCreateWindow(*width, *height, "Opengl example", NULL, NULL);

		if (!window) {
			glfwTerminate();
			std::cout << "FAILED to create window or opengl context" << std::endl;
			return NULL;
		}

		//sets up glad and Opengl context
		glfwMakeContextCurrent(window);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		glfwGetFramebufferSize(window, width, height);
		glViewport(0, 0, *width, *height);

		glfwSwapInterval(1);
		set_renderer_up();

		win = window;
		return window;
	}

	void clear() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void render_rectangle(GLfloat *matrix) {
		glUniformMatrix4fv(mat_location, 1, GL_FALSE, matrix);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void display() {
		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	void set_renderer_up() {
		//rectangle vertex data
		GLfloat vertices[] = {
			-0.15f, 0.03f, 1.0f, 1.0f, 1.0f,
			-0.15f, -0.03f, 1.0f, 1.0f, 1.0f,
			0.15f, 0.03f, 1.0f, 1.0f, 1.0f,
			0.15f, -0.03f, 1.0f, 1.0f, 1.0f
		};

		//shaders
		static const GLchar* vertex_shader_text =
			"#version 150 core\n"
			"in vec2 position;"
			"in vec3 color;"
			"out vec3 Color;"
			"uniform mat4 mat;"
			"void main()"
			"{"
			"Color = color;"
			"gl_Position = mat * vec4(position, 0.0, 1.0);"
			"};";

		static const GLchar* fragment_shader_text =
			"#version 150 core\n"
			"in vec3 Color;"
			"out vec4 outColor;"
			"void main()"
			"{"
			"outColor = vec4(Color, 1.0);"
			"};";

		GLuint vbo, vao, ebo, vertex_shader, fragment_shader, program;
		GLint position_location, color_location;

		//VAO
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//VBO
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//EBO - rectangle vertex render order
		glGenBuffers(1, &ebo);

		GLuint elements[] = {
			0, 1, 2,
			2, 3, 1
		};

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		//preparing shaders
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

		//setting matrix data
		mat_location = glGetUniformLocation(program, "mat");

		//setting data layout
		position_location = glGetAttribLocation(program, "position");
		glEnableVertexAttribArray(position_location);
		glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, 0);

		color_location = glGetAttribLocation(program, "color");
		glEnableVertexAttribArray(color_location);
		glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(2 * sizeof(GLfloat)));
	}

	//glfw error callback
	void error_callback(int error, const char* description) {
		std::cout << "Error" << error << ": " << description << std::endl;
	}
}