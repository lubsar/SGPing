#include "Platform.h"

namespace sgping {
	Platform::Platform(GLfloat *platform_matrix, rectangle collision_box) {
		this->matrix = platform_matrix;
		this->collision_box = collision_box;
	}

	void Platform::update(double deltaTime) {
		if (velocity != 0) {
			float step = (float)deltaTime * velocity;
			//adds movement
			collision_box.x += step;
			//if collides
			if (collides_edge(&collision_box)) {
				//moves back
				collision_box.x -= step;
				//adjusts position of platform
				if (step > 0) {
					collision_box.x = 2.0f - collision_box.h_width;
				}
				else if (step < 0) {
					collision_box.x = collision_box.h_width;
				}
			}
			//updates platform matrix (render)
			matrix[12] = toOpenglCoords(collision_box.x, true);
		}
	}
}