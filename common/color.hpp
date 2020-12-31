#ifndef HEBY_COLOR_HPP
#define HEBY_COLOR_HPP

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>

namespace heby {

	// A class to store color data
	// float_t r, g, b, a;
	class color {
	public:
		color() {
			r = 0;
			g = 0;
			b = 0;
			a = 1;
		}

		color(int _r, int _g, int _b, float _a) {
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}

		std::string toString() {
			std::string __r = "rgba(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "," + std::to_string(a) + ")";
			return __r;
		}

		int r;
		int g;
		int b;
		float a;
	private:
	};

} // namespace heby



#endif //HEBY_PLAYER_HPP