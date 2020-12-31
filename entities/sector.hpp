#ifndef HEBY_SECTOR_HPP
#define HEBY_SECTOR_HPP

#include <heby/common/color.hpp>
#include <heby/entities/food.hpp>
#include <heby/common/helper.hpp>
#include <heby/config/config.hpp>
#include <heby/common/skin_manager.hpp>

#include <map>
#include <vector>

namespace heby {

	class sector {
	public:
		sector() {
			x = 0;
			y = 0;
			width = 0;
			height = 0;
		}
		sector(skinmanager *_skinmanager, int _x, int _y, int _width, int _height, int _nb) {
			x = _x;
			y = _y;
			width = _width;
			height = _height;
			
			for (int i = 0; i < _nb; i++) {
				addFood(_skinmanager);
			}
		}
		void addFood(skinmanager *_skinmanager) {
			Rand_int rndX{ 0, (int)width };
			Rand_int rndY{ 0, (int)height };

			Rand_int skinindex{ 0, (int)_skinmanager->food_proportions.size() - 1 };

			foods.push_back(food(_skinmanager,rndX(),rndY(), _skinmanager->food_proportions[skinindex()]));
		}
		void addFood(skinmanager *_skinmanager, bool foodOfHeby) {
			Rand_int rndX{ 0, (int)width };
			Rand_int rndY{ 0, (int)height };

			Rand_int skinindex{ 0, (int)_skinmanager->food_proportions.size() - 1 };

			foods.push_back(food(_skinmanager, rndX(), rndY(), _skinmanager->food_proportions[skinindex()], foodOfHeby));
		}
		void addFood(skinmanager *_skinmanager, double_t x, double_t y, double_t mass, bool respawn) {
			Rand_int skinindex{ 0, (int)_skinmanager->food_proportions.size() - 1 };
			foods.push_back(food(_skinmanager,x,y,mass,respawn, _skinmanager->food_proportions[skinindex()]));
		}
		void addFood(skinmanager *_skinmanager, double_t x, double_t y, double_t mass, bool respawn, bool foodOfHeby) {
			Rand_int skinindex{ 0, (int)_skinmanager->food_proportions.size() - 1 };
			foods.push_back(food(_skinmanager, x, y, mass, respawn, _skinmanager->food_proportions[skinindex()], foodOfHeby));
		}
		std::string getFoodData() {
			std::string data = "";
			for (std::vector<food>::iterator it = foods.begin(); it != foods.end(); ++it) {
				data += std::to_string(it->x) + ":" + std::to_string(it->y) + ":" + std::to_string(it->size) + ":" + std::to_string(it->skin) + "\n";
			}
			if (data.length() > 0) {
				data = data.substr(0, data.length() - 1);
			}
			return data;
		}
		void add(std::string id) {
			m_snakes[id] = true;
		}
		void removePlayer(std::string id) {
			if (m_snakes.find(id) != m_snakes.end()) {
				m_snakes.erase(id);
			}
		}
		std::map<std::string, bool>* getSnakes() {
			return &m_snakes;
		}
		std::vector<food>* getFoods() {
			return &foods;
		}
		std::string toString() {
			std::string pos = std::to_string(x) + ":" + std::to_string(y);
			return pos;
		}
		int x;
		int y;
		int width;
		int height;
		std::vector<food> foods;
	private:
		std::map<std::string, bool> m_snakes;
	};

} // namespace heby

#endif //HEBY_SECTOR_HPP