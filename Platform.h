#pragma once
#ifndef __Platform_h_
#define __Platform_h_

#include "Core.h"
#include "Physics.h"

namespace sgping {
	class Platform {
	public:
		rectangle collision_box;
		GLfloat *matrix;

	private:
		float velocity;

	public: 
		Platform(GLfloat *, rectangle );
		void update(double);

		void setVelocity(float velocity) {
			this->velocity = velocity;
		}

		float getVelocity() {
			return this->velocity;
		}
	};
}
#endif