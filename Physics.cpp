#include "Physics.h"
#include <iostream>

namespace sgping {
	bool out_of_area(rectangle *r) {
		if ((r->y + r->h_height >= physics_height) || (r->y - r->h_height) <= 0) {
			return true;
		}

		return false;
	}

	bool collides_rectangle(rectangle *r1, rectangle *r2) {
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

	//checks if collision box collides with an edge of playing field
	bool collides_edge(rectangle* p) {
		if ((p->x + p->h_width >= physics_width) || (p->x - p->h_width) <= 0) {
			return true;
		}
		else {
			return false;
		}
	}

	float toWindowCoords(float x) {
		return x + 1;
	}

	float toOpenglCoords(float x, bool hor) {
		if (hor)
			return x - 1;
		else
			return -x + 1;
	}
}