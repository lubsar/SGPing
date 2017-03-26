#pragma once
#ifndef __Physics_h_
#define __Physics_h_

namespace sgping{
	typedef struct rectangle {
		float x, y, h_width, h_height;
	} rectangle;

	bool out_of_area(rectangle *r);
	bool collides_rectangle(rectangle *r1, rectangle *r2);
	bool collides_edge(rectangle* p);
	
	float toWindowCoords(float x);
	float toOpenglCoords(float x, bool hor);

	const float physics_width = 2.0f;
	const float physics_height = 2.0f;
	}
#endif