#ifndef HEBY_FOOD_HPP
#define HEBY_FOOD_HPP

#include <heby/common/color.hpp>
#include <heby/common/skin_manager.hpp>

#include <string>
#include <map>

#include <heby/common/helper.hpp>
#include <heby/logger/log.hpp>
#include <heby/config/config.hpp>
#include <heby/common/helper.hpp>

#include <heby/common/mode.hpp>

#include <chrono>

#include <iostream>

#ifndef JSON_IS_AMALGAMATION
#define JSON_IS_AMALGAMATION
#include <jsoncpp.hpp>
#endif

namespace heby {

	

	class food {
	public:
		food() {
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
		food(skinmanager *_skinmanager, double_t _x, double_t _y, int _skin) {
			x = _x;
			y = _y;
			skin = _skin;
			foodOfHeby = false;

			extract_data(_skinmanager);
		}
		food(skinmanager *_skinmanager, double_t _x, double_t _y, double_t _mass, int _skin) {
			x = _x;
			y = _y;
			skin = _skin;
			foodOfHeby = false;
			mass = _mass;
			
			extract_data(_skinmanager);
		}
		food(skinmanager *_skinmanager, double_t _x, double_t _y, double_t _mass, int _skin, bool _foodOfHeby) {
			x = _x;
			y = _y;
			skin = _skin;
			foodOfHeby = _foodOfHeby;
			mass = _mass;

			extract_data(_skinmanager);
		}
		food(skinmanager *_skinmanager, double_t _x, double_t _y, double_t _mass, bool _respawn, int _skin) {
			x = _x;
			y = _y;
			skin = _skin;
			foodOfHeby = false;

			extract_data(_skinmanager);

			mass = _mass;
			respawn = _respawn;
		}
		food(skinmanager *_skinmanager, double_t _x, double_t _y, double_t _mass, bool _respawn, int _skin, bool _foodOfHeby) {
			x = _x;
			y = _y;
			skin = _skin;
			foodOfHeby = _foodOfHeby;

			extract_data(_skinmanager);

			mass = _mass;
			respawn = _respawn;
		}
		hyfood get() {
			hyfood f;
			f.x = x;
			f.y = y;
			f.skin = skin;
			f.foodOfHeby = foodOfHeby;
			f.mass = mass;
			f.respawn = respawn;
			f.is_attract = is_attract;
			f.speed_attract = speed_attract;
			f.size = size;
			f.time = time;
			f.timeout = timeout;
			return f;
		}
		void set(hyfood *f) {
			x = f->x;
			y = f->y;
			skin = f->skin;
			foodOfHeby = f->foodOfHeby;
			mass = f->mass;
			respawn = f->respawn;
			is_attract = f->is_attract;
			speed_attract = f->speed_attract;
			size = f->size;
			time = f->time;
			timeout = f->timeout;
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

		void extract_data(skinmanager *_skinmanager) {

			Json::Value data = _skinmanager->food_skins[skin];

			respawn = data.get("respawn", true).asBool();

			if (data.isMember("size")) {
				if (data["size"].isMember("min") && data["size"].isMember("max")) {
					int default_minSize = data["size"].get("min", 5).asInt();
					int default_maxSize = data["size"].get("max", 10).asInt();
					if (default_minSize > default_maxSize) {
						int tmp = default_minSize;
						default_minSize = default_maxSize;
						default_maxSize = tmp;
					}
					Rand_int rnd{ default_minSize,  default_maxSize };
					size = rnd();
				} else {
					size = data["size"].get("value", 5.0).asDouble();
				}
			} else {
				size = 5.0;
			}

			data["size"]["value"] = size;


			Json::Value source = data;
			percrepl(&data, &source);

			if (data.isMember("mass")) {
				mass = data["mass"].get("value", 1).asDouble();
			} else {
				mass = 1.0;
			}

			if (data.isMember("attracted")) {
				is_attract = data["attracted"].get("active", true).asBool();
				speed_attract = data["attracted"].get("speed", 5).asDouble();
				coefficient = data["attracted"].get("coefficient", 5).asDouble();
			}
			else {
				is_attract = true;
				speed_attract = 5;
				coefficient = 4;
			}

			if (foodOfHeby) {
				timeout = data.get("timeoutHebyFood", 10000).asInt();
			} else {
				timeout = data.get("timeout", -1).asInt();
			}
			time = std::chrono::steady_clock::now();
		}
	};


} // namespace heby



#endif //HEBY_FOOD_HPP