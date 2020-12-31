#ifndef HEBY_MODE_HPP
#define HEBY_MODE_HPP

#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace heby {

	enum STATE {
		DEAD = 0x0,
		PLAYING = 0x1,
		DYING = 0x2,
		SPECTATING = 0x3,
		RESPAWN = 0x4,
		HOMEPAGE = 0x5
	};

	enum direction {
		none = 0x0,
		left = 0x1,
		right = 0x2
	};

	class hysection {
	public:
		hysection() {
			x = 0;
			y = 0;
		}
		hysection(double_t _x, double_t _y) {
			x = _x;
			y = _y;
		}
		double_t x;
		double_t y;
	private:
	};

	class hyfood {
	public:
		hyfood() {
			x = 0;
			y = 0;
			skin = 0;
			foodOfHeby = false;
			mass = 1;
			respawn = true;
			is_attract = true;
			speed_attract = 5;
			size = 5;
			time = std::chrono::steady_clock::now();
			timeout = -1;
		}
		double_t x;
		double_t y;
		double_t size;
		double_t mass;
		bool foodOfHeby;
		bool respawn;
		bool is_attract;
		double_t speed_attract;
		double_t coefficient;
		std::chrono::time_point<std::chrono::steady_clock> time;
		int timeout;
		int skin;
	private:
	};

	// Define the Snake class
	class hysnake {
	public:
		hysnake() {}

		int precision;

		std::string id;
		std::string name;

		direction m_direction;

		double_t length;

		int size;

		int skin;
		int current_size;

		int radius;

		int rang;

		hysection head;

		STATE state;

		std::chrono::time_point<std::chrono::steady_clock> timeDead;
	private:
	};

	class mode {
	public:
		mode();

		virtual void onload();

		virtual void on_join(hysnake *player);

		virtual std::string on_join_data_send(std::string data);

		virtual void on_left(hysnake *player);

		virtual void on_play(hysnake *s);


		// When a snake is killed by other snake
		virtual void on_kill(hysnake *s, hysnake *other);

		// When a food is eaten
		virtual void on_food_eaten(hyfood *f, hysnake *s);

		// When a snake collide a food
		virtual bool on_food_collide(hyfood *f, hysnake *s);

		// When snake go outside the map
		virtual void on_outside_map(hysnake *s);

		// When a snake collide an other snake
		virtual bool on_collide(hysnake *s, hysnake *other);


		std::string name;
	private:
	};

}

#endif //HEBY_MODE_HPP