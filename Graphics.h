#pragma once
#ifndef __Graphics_h_
#define __Graphics_h_

#include "Core.h"

namespace sgping {
	GLFWwindow* setup(int *width, int *height);
	void clear();
	void render_rectangle(GLfloat *matrix);
	void display();
}
#endif 
