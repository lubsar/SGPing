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

#include "Core.h"
#include "Physics.h"
#include "Game.h"
#include "Graphics.h"

namespace sgping {
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

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

	Platform Game::bottom = Platform(platform_b_matrix, { 1.0f, 1.97f, 0.15f, 0.03f });
	Platform Game::top = Platform(platform_t_matrix, { 1.0f, 0.03f, 0.15f, 0.03f });
	Ball Game::ball = Ball(ball_matrix, { 1.0f, 1.0f, 0.05f, 0.03f });

	float ball_speed_h = 1.0f/60.0f;
	float ball_speed_v = -1.0f/60.0f;
	const float platform_speed = 2.0f/60.0f;

	void set_game_up() {
		Game::ball.velocity_x = ball_speed_h;
		Game::ball.velocity_y = ball_speed_v;
	}

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		//control of the bottom platform
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
			Game::bottom.setVelocity(-platform_speed);
		if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
			Game::bottom.setVelocity(0.0f);

		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
			Game::bottom.setVelocity(platform_speed);
		if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
			Game::bottom.setVelocity(0.0f);

		//control of the top platform
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
			Game::top.setVelocity(-platform_speed);
		if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
			Game::top.setVelocity(0.0f);

		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
			Game::top.setVelocity(platform_speed);
		if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
			Game::top.setVelocity(0.0f);
	}
}

using namespace sgping;

int main() {
	set_game_up();

	int width = 500;
	int height = 700;

	GLFWwindow *window = setup(&width, &height);
	glfwSetKeyCallback(window, key_callback);

	if (!window) {
		return EXIT_FAILURE;
	}

	//game loop
	double lastTime = glfwGetTime();
	double delta = 0.0;
	
	double updateTime = 1.0 / 60.0;
	double outT = lastTime;

	unsigned int tps = 0,fps = 0;

	while (!glfwWindowShouldClose(window)) {
		delta += (glfwGetTime() - lastTime);
		fps++;

		while (delta >= updateTime) {
			//update
			Game::bottom.update();
			Game::top.update();

			Game::ball.update();
			delta -= updateTime;
			tps++;
		}

		if (glfwGetTime() - outT >= 1.0)  {
			std::cout << "tps: " <<tps << " delta: " << (glfwGetTime() - outT) / fps * 1000  << "ms fps: " << fps << std::endl;
			tps = 0;
			outT = glfwGetTime();
			fps = 0;
		}

		//rendering
		clear();

		render_rectangle(Game::bottom.matrix);
		render_rectangle(Game::top.matrix);
		render_rectangle(Game::ball.matrix);
	
		lastTime = glfwGetTime();
		display();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}
