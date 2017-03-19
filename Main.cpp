/*
Copyright (c) 2017 Ľubomír Hlavko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include <iostream>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

//collision box struct
struct rectangle {
	float x, y, h_width, h_height;
};

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

static void update_ball(double deltaTime);
static void update_platform(rectangle* p, float platform_velocity, float* platform_matrix, double deltaTime);

static bool collides_edge(rectangle *r);
static bool out_of_area(rectangle* p);
static bool collides_rectangle(rectangle *r1, rectangle *r2);

static float toWindowCoords(float x);
static float toOpenglCoords(float x, bool hor);

GLFWwindow* setup(int*width, int*height);

//rectangle vertex data
GLfloat vertices[] = {
	-0.15f, 0.03f, 1.0f, 1.0f, 1.0f,
	-0.15f, -0.03f, 1.0f, 1.0f, 1.0f,
	0.15f, 0.03f, 1.0f, 1.0f, 1.0f,
	0.15f, -0.03f, 1.0f, 1.0f, 1.0f
};

//matrices
GLfloat platform_b_matrix[] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, -0.97f, 0.0f, 1.0f
};

GLfloat platform_t_matrix[] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.97f, 0.0f, 1.0f
};

GLfloat ball_matrix[] = {
	0.3f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
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


//ball data
rectangle  ball = {1.0f, 1.0f, 0.05f, 0.03f};

float ball_velocity_x = 0.05f;
float ball_velocity_y = -0.02f;

const float ball_speed = 0.01f;

//platforms
rectangle  platform_b = {1.0f, 1.97f, 0.15f, 0.03f};
rectangle  platform_t = {1.0f, 0.03f, 0.15f, 0.03f};

const float platform_speed = 0.05f;

float platform1_velocity = 0.0f;
float platform2_velocity = 0.0f;

int main() {
	int width, height;
	GLuint vbo, vao, ebo, vertex_shader, fragment_shader, program;
	GLint position_location, color_location, mat_location;

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
	glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5,  0);

	color_location = glGetAttribLocation(program, "color");
	glEnableVertexAttribArray(color_location);
	glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*) (2* sizeof(GLfloat)));

	//game loop
	double lastTime = glfwGetTime();
	double deltaTime = 0.0;
	double nowTime = 0.0;

	double fpsTime = 1.0 / 60.0;

	while (!glfwWindowShouldClose(window)) {
		nowTime = glfwGetTime();
		deltaTime = (nowTime - lastTime) / fpsTime;
		lastTime = nowTime;
		
		while (deltaTime >= 1.0) {
			//update
			update_platform(&platform_b, platform1_velocity, platform_b_matrix, deltaTime);
			update_platform(&platform_t, platform2_velocity, platform_t_matrix, deltaTime);
			update_ball(deltaTime);

			deltaTime--;
		}

		//rendering
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	    glUniformMatrix4fv(mat_location, 1, GL_FALSE, platform_b_matrix);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(mat_location, 1, GL_FALSE, platform_t_matrix);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glUniformMatrix4fv(mat_location, 1, GL_FALSE, ball_matrix);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}

//ball movement
static void update_ball(double deltaTime) {
	//vertical movement
	float step = (float) (deltaTime * ball_velocity_y);
	
	ball.y -= step;
	
	if (collides_rectangle(&ball, &platform_b)) {
		ball.y += step;
		
		ball.y = platform_b.y - (platform_b.h_height + ball.h_height);

		ball_velocity_y = -ball_velocity_y;
	} else if (collides_rectangle(&ball, &platform_t)){
		ball.y += step;
		
		ball.y = platform_t.y + (platform_t.h_height + ball.h_height);

		ball_velocity_y = -ball_velocity_y;
	}

	if (out_of_area(&ball)) {
		ball.y = 1.0f;
	}

	//horizontal movement
	step = (float) (deltaTime * ball_velocity_x);
	ball.x += step;
	if (collides_edge(&ball)) {
		ball.x -= step;
		if (step > 0) {
			ball.x = 2.0f - ball.h_width;
		}
		else if (step < 0) {
			ball.x = ball.h_width;
		}

		ball_velocity_x = -ball_velocity_x;
	}

	ball_matrix[13] = toOpenglCoords(ball.y, false);
	ball_matrix[12] = toOpenglCoords(ball.x, true);
}

static bool out_of_area(rectangle *r){
	if ((r->y + r->h_height >= 2) || (r->y - r->h_height) <= 0) {
		return true;
	}

	return false;
}

static bool collides_rectangle(rectangle *r1, rectangle *r2) {
	//rectangle 1
	float xl1 = r1->x - r1->h_width;
	float yt1 = r1->y - r1->h_height;

	float xr1 = r1->x + r1->h_width;
	float yb1 = r1->y + r1->h_height;

	//rectangle 2
	float xl2 = r2->x - r2->h_width;
	float yt2 = r2->y - r2->h_height;

	float xr2 = r2->x + r2->h_width;
	float yb2 = r2->y + r2->h_height;

	if (xl1 < xr2 &&
		xr1 > xl2 &&
		yt1 < yb2 &&
		yb1 > yt2) {

		return true;
	}

	return false;
}

//platform movement
static void update_platform(rectangle* p, float platform_velocity, float* platform_matrix, double deltaTime) {
	if (platform_velocity != 0) {
		float step = (float)deltaTime * platform_velocity;
		//adds movement
		p->x += step;
		//if collides
		if (collides_edge(p)) {
			//moves back
			p->x -= step;
			//adjusts position of platform
			if (step > 0) {
				p->x = 2.0f - p->h_width;
			}
			else if (step < 0){
				p->x = p->h_width;
			}
		}

		//updates platform matrix (render)
		platform_matrix[12] = toOpenglCoords(p->x, true);
	}
}

//checks if collision box collides with an edge of playing field
static bool collides_edge(rectangle* p) {
	if ((p->x + p->h_width >= 2) || (p->x - p->h_width) <= 0) {
		return true;
	} else {
		return false;
	}
}

static float toWindowCoords(float x) {
	return x + 1;
}

static float toOpenglCoords(float x, bool hor) {
	if (hor)
		return x - 1;
	else
		return -x + 1;
}

GLFWwindow* setup(int*width, int*height) {
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
	GLFWwindow *window = glfwCreateWindow(500, 700, "Opengl example", NULL, NULL);

	if (!window) {
		glfwTerminate();
		std::cout << "FAILED to create window or opengl context" << std::endl;
		return NULL;
	}

	//sets up glad and Opengl context
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSetKeyCallback(window, key_callback);

	glfwGetFramebufferSize(window, width, height);
	glViewport(0, 0, *width, *height);

	glfwSwapInterval(1);

	return window;
}

//glfw error callback
static void error_callback(int error, const char* description) {
	std::cout << "Error" << error << ": " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	//control of the bottom platform
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		platform1_velocity = -platform_speed;
	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
		platform1_velocity = 0.0f;

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		platform1_velocity = platform_speed;
	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
		platform1_velocity = 0.0f;

	//control of the top platform
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		platform2_velocity = -platform_speed;
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
		platform2_velocity = 0.0f;

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		platform2_velocity = platform_speed;
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
		platform2_velocity = 0.0f;
}