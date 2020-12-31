#ifndef HEBY_MAP_HPP
#define HEBY_MAP_HPP

#include <heby/common/manager.hpp>
#include <heby/entities/sector.hpp>
#include <heby/config/config.hpp>
#include <heby/common/skin_manager.hpp>

#include <map>

namespace heby {

	class map {
	public:
		map() {}
		map(manager *_m) {
			m_manager = _m;
			m_config = m_manager->configuration;
			m_skinmanager = m_manager->skinmanager;

			width = m_config.mapSizeWidth;
			height = m_config.mapSizeHeight;

			sector_width = m_config.sectorSizeWidth;
			sector_height = m_config.sectorSizeHeight;
		}

		void generate() {
			int x = 0;
			int y = 0;

			int i = 0;

			int t = (width / sector_width) * (height / sector_height);

			while (x < width) {

				y = 0;

				while (y < height) {

					m_sectors[std::to_string(x) + ":" + std::to_string(y)] = sector(m_skinmanager,x,y,sector_width,sector_height,(int)(m_config.foodstartAmount/t));

					y += sector_height;

					i++;
				}

				x += sector_width;
			}

			m_manager->console.game(std::cout) << i << " Sectors created!" << std::endl;

		}

		void remove(std::string id, std::map<std::string, bool> *m_list) {
			std::map<std::string, bool>::iterator it;

			for (it = m_list->begin(); it != m_list->end(); it++) {
				if (m_sectors.find(it->first) != m_sectors.end()) {
					m_sectors[it->first].removePlayer(id);
				}
			}

			m_list->clear();
		}
		bool add(double_t ___xdff, double_t ___ydff, std::string id, int* __x, int *__y) {

			// out of map
			if (___xdff < 0) return false;
			if (___ydff < 0) return false;
			if (___xdff > width) return false;
			if (___ydff > height) return false;

			*__x = (int)round(___xdff) - (int)round(___xdff) % sector_width;
			*__y = (int)round(___ydff) - (int)round(___ydff) % sector_height;
			m_sectors[std::to_string(*__x) + ":" + std::to_string(*__y)].add(id);

			return true;
		}
		std::string getFoodData(std::string pos) {
			std::string data = "";
			if (m_sectors.find(pos) != m_sectors.end()) {
				data = m_sectors[pos].getFoodData();
			}
			return data;
		}
		std::vector<sector*> get(double_t _x, double_t _y, double_t _width, double_t _height) {

			double_t _ysave = _y;

			int _checkx = sector_width + 1;
			int _checky = sector_height + 1;


			double_t xend = _x + _width + 1.0;
			double_t yend = _y + _height + 1.0;

			std::map<std::string, bool> test;

			while (_x < xend) {

				_y = _ysave;

				while (_y < yend) {

					double_t _xnormalize = _x;
					double_t _ynormalize = _y;

					if (_x < 0) {
						_xnormalize = 0;
					}
					if (_y < 0) {
						_ynormalize = 0;
					}

					int __x = (int)round(_xnormalize) - (int)round(_xnormalize) % sector_width;
					int __y = (int)round(_ynormalize) - (int)round(_ynormalize) % sector_height;

					if (_checkx != __x || _checky != __y) {
						_checkx = __x;
						_checky = __y;
						test[std::to_string(__x) + ":" + std::to_string(__y)] = true;
					}

					_y += _height;

				}


				_x += _width;

			}

			std::vector<sector*> list;
			std::map<std::string, bool>::iterator it;
			for (it = test.begin(); it != test.end(); it++) {
				list.push_back(&m_sectors[it->first]);
			}
			
			return list;

		}

		
		sector* get_sector(double_t _x, double_t _y) {
			int __x = (int)round(_x) - (int)round(_x) % sector_width;
			int __y = (int)round(_y) - (int)round(_y) % sector_height;

			if (__x < 0) {
				__x = 0;
			}
			if (__y < 0) {
				__y = 0;
			}
			if (__x >= width) {
				__x = width - sector_width;
			}
			if (__y >= height) {
				__y = height - sector_height;
			}

			if (m_sectors.find(std::to_string(__x)+":"+ std::to_string(__y)) != m_sectors.end()) {
				return &m_sectors.at(std::to_string(__x) + ":" + std::to_string(__y));
			}
			else {
				return &m_sectors.at("0:0");
			}
		}
		

		int width;
		int height;

		int sector_width;
		int sector_height;
	private:
		manager *m_manager;
		config m_config;
		skinmanager *m_skinmanager;
		std::map<std::string, sector> m_sectors;
	};

} // namespace heby



#endif //HEBY_MAP_HPP