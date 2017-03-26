#pragma once
#ifndef __Game_h_
#define __Game_h_

#include "Platform.h"
#include "Ball.h"

namespace sgping {
	class Game {
	public:
		static Platform top;
		static Platform bottom;
		static Ball ball;
	};
}
#endif
