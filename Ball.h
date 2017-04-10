#pragma once
#ifndef __Ball_h_
#define __Ball_h_

#include "Core.h"
#include "Physics.h"

namespace sgping {
	class Ball {
		public:
			rectangle collision_box;
			float velocity_x, velocity_y;
			GLfloat *matrix;

			Ball(GLfloat*, rectangle);
			void update();
	};
}
#endif 
