#ifndef HEBY_SNAKE_HPP
#define HEBY_SNAKE_HPP

typedef websocketpp::server<websocketpp::config::asio> websocketppserver;

namespace heby {
	class snake;
} // namespace heby

#include <heby/entities/map.hpp>
#include <heby/entities/sector.hpp>
#include <heby/common/helper.hpp>
#include <heby/common/color.hpp>
#include <heby/config/config.hpp>

#include <heby/common/mode.hpp>

#include <cmath>
#include <vector>
#include <queue>
#include <chrono>

namespace heby {

	// Contain the data of one vertebrate
	/*
		color color;
		int x;
		int y;
	*/
	class section {
	public:
		section() {
			x = 0;
			y = 0;
		}
		section(double_t _x, double_t _y) {
			x = _x;
			y = _y;
		}
		double_t x;
		double_t y;
	private:
	};


	class point {
	public:
		point() {
			x = 0;
			y = 0;
		}
		point(double_t _x, double_t _y) {
			x = _x;
			y = _y;
		}
		double_t x;
		double_t y;
	private:
	};

	// Should contain the box information of each "heby"
	/*
		int x;
		int y;
		int width;
		int height;
	*/
	class box {
	public:
		box() {
			x = 0;
			y = 0;
			width = 0;
			height = 0;
		}
		box(double_t _x, double_t _y, double_t _width, double_t _height) {
			x = _x;
			y = _y;
			width = _width;
			height = _height;
		}
		double_t x;
		double_t y;
		double_t width;
		double_t height;
	private:
	};

	class snake {

	public:

		typedef std::map<std::string, snake> snakelist;
		typedef std::set<connection_hdl, std::owner_less<connection_hdl> > con_list;

		snake() {}

		// HUMAN ENTRY
		snake(std::string _id, connection_hdl hdl, manager *_manager, int _skin) {
			configuration = _manager->configuration;
			init(hdl);
			id = _id;
			m_is_bot = false;
			if (_skin < 0) {
				setSkin(_manager);
			} else {
				skin = _skin;
			}
			calc_property();
			initSections(size);
		}

		// BOT ENTRY
		snake(std::string _id, manager *_manager, int _skin) {
			configuration = _manager->configuration;
			init();
			id = _id;
			m_is_bot = true;
			setName(_manager);
			if (_skin < 0) {
				setSkin(_manager);
			} else {
				skin = _skin;
			}
			calc_property();
			initSections(size);
		}

		void initSections(int len) {
			int s = m_sections.size() - 1;
			for (int i = 0; i < len; i++) {
				m_sections.push_back(section(m_sections[s].x, m_sections[s].y));
				for (int j = 0; j < precision; j++) {
					m_headPath.push_back(point(m_sections[s].x, m_sections[s].y));
				}
			}
			current_size = m_sections.size();
		}

		std::string get_data() {
			std::string raw_data = "";
			for (unsigned i = 0; i < m_sections.size(); i++) {
				raw_data += std::to_string(m_sections[i].x) + ":" + std::to_string(m_sections[i].y) + "\n";
			}
			if (raw_data.length() > 0) {
				raw_data = raw_data.substr(0, raw_data.length() - 1);
			}
			return raw_data;
		}

		void update_angle(double_t angle) {
			//m_angle = angle;
		}

		void update_position(map *_m, manager *_manager, websocketppserver *_serv, con_list *_conn) {

			if (state == STATE::DYING && configuration.HebyTimeoutWhenDie >= 0) {
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timeDead);
				if (elapsed.count() > configuration.HebyTimeoutWhenDie) {
					state = STATE::HOMEPAGE;
				}
			}

			if (m_is_bot) {
				random_turn();
			}

			if (state == STATE::PLAYING) {
				if (configuration.HebyLengthDecayRate > 0 && length > configuration.HebyMinLengthDecay) {
					length -= configuration.HebyLengthDecayRate;
				}
			}

			if (state == STATE::PLAYING) {
				turn_snake(m_direction);
				if (!m_is_bot) {
					websocketppserver::connection_ptr con = _serv->get_con_from_hdl(m_hdl);
					std::string resp("1\n" + std::to_string(m_angle));
					con->send(resp, websocketpp::frame::opcode::text);
				}
			}

			// update the head's position
			if (state == STATE::PLAYING) {
				m_headPath.clear();
				generateHeadPath();
				interpolate(point(m_sections[0].x - m_speed * cos(m_angle), m_sections[0].y - m_speed * sin(m_angle)));
			}


			// remove this snake of the map
			if (state == STATE::PLAYING) {
				_m->remove(id, &m_sectorslist);
			}

			// calc the sections positions
			if (state == STATE::PLAYING) {
				int index = 0;
				for (std::vector<section>::iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
					it->x = m_headPath.at(index).x;
					it->y = m_headPath.at(index).y;

					//std::cout << index << " : " << m_headPath.at(index).x << ", " << m_headPath.at(index).y << std::endl;
					int _xdd;
					int _ydd;
					bool check = _m->add(it->x, it->y, id, &_xdd, &_ydd);
					if (check) {
						m_sectorslist[std::to_string(_xdd) + ":" + std::to_string(_ydd)] = true;
					}
					index = findNextPointIndex(index);
				}
			}

			// calc the viewbox
			m_viewbox.x = m_sections[0].x - m_zoom * 200;
			m_viewbox.y = m_sections[0].y - m_zoom * 200;
			m_viewbox.width = 2 * m_zoom * 200;
			m_viewbox.height = 2 * m_zoom * 200;

			// get all sectors intersected with the viewbox
			// if there a "new sector", send to this snake all remaining snake in the sector just discovered (one by one) and send the food content sector
			std::vector<sector*> hhhsectorslist = _m->get(m_viewbox.x, m_viewbox.y, m_viewbox.width, m_viewbox.height);
			

			if (hhhsectorslist.size() > 0) {

				snakelistpointer testmap;

				typedef std::map<std::string, bool> snakelistit;

				std::map<std::string, bool> sectorslisttestcheck;

				// Add All snakes in adapted std::map object
				for (unsigned i = 0; i < hhhsectorslist.size(); i++) {

					sector* v = hhhsectorslist[i];

					if (state == STATE::PLAYING) {
						collision_food(v, _manager); // O(1)
					}

					snakelistit *snakes_sector = v->getSnakes();
					for (snakelistit::iterator ita = snakes_sector->begin(); ita != snakes_sector->end(); ++ita) {
						if (_manager->snakes->find(ita->first) != _manager->snakes->end()) {
							snake *p = &(((*_manager->snakes))[ita->first]);
							if (id != p->id) {
								testmap[p->id] = p;
							}
						}
					}

					std::string pos = std::to_string(v->x) + ":" + std::to_string(v->y);
					if (!m_is_bot) {
						try {
							websocketppserver::connection_ptr con = _serv->get_con_from_hdl(m_hdl);
							std::string resp("7\n" + pos + "\n" + _m->getFoodData(pos));
							con->send(resp, websocketpp::frame::opcode::text);
						} catch (...) {}
					}
					if (m_testsector.find(pos) == m_testsector.end()) {

						// add sector
						//if (!m_is_bot) {
						//	websocketppserver::connection_ptr con = _serv->get_con_from_hdl(m_hdl);
						//	std::string resp("7;" + pos + ";" + _m->getFoodData(pos));
						//	con->send(resp, websocketpp::frame::opcode::text);
						//}

						m_testsector[pos] = true;

					}
					sectorslisttestcheck[pos] = true;


					// Add Visible snakes
					for (snakelistpointer::iterator itb = testmap.begin(); itb != testmap.end(); ++itb) {
						if (view_snake.find(itb->first) == view_snake.end()) {

							if (!m_is_bot) {
								try {
									websocketppserver::connection_ptr con = _serv->get_con_from_hdl(m_hdl);
									std::string resp("5\n" + itb->second->id + "\n" + itb->second->name + "\n" + std::to_string(itb->second->precision) + "\n" + std::to_string(itb->second->length) + "\n" + std::to_string(itb->second->radius) + "\n" + std::to_string(itb->second->skin) + "\n" + itb->second->get_data());
									con->send(resp, websocketpp::frame::opcode::text);
								} catch (...) {}
							}

							view_snake[itb->first] = itb->second;
						}
					}

				}

				if (state == STATE::PLAYING) {
					collision_snake(&testmap, _m, _manager, _serv, _conn); // O(1)
				}

				// Remove Invisible snakes
				for (snakelistpointer::iterator itc = view_snake.begin(); itc != view_snake.end(); ++itc) {
					if (testmap.find(itc->first) == testmap.end()) {

						if (!m_is_bot) {
							try {
								websocketppserver::connection_ptr con = _serv->get_con_from_hdl(m_hdl);
								std::string resp("6\n" + itc->second->id);
								con->send(resp, websocketpp::frame::opcode::text);
							} catch (...) {}
						}

						view_snake.erase(itc->first);
					}
				}


				/////////////////////////////////////
				for (std::map<std::string, bool>::iterator it = m_testsector.begin(); it != m_testsector.end(); ++it) {
					if (sectorslisttestcheck.find(it->first) == sectorslisttestcheck.end()) {

						// remove sector
						if (!m_is_bot) {
							try {
								websocketppserver::connection_ptr con = _serv->get_con_from_hdl(m_hdl);
								std::string resp("8\n" + it->first);
								con->send(resp, websocketpp::frame::opcode::text);
							} catch (...) {}
						}

						m_testsector.erase(it->first);
					}
				}

			}

			
		}

		void send_data(websocketppserver *serv) {
			if (!m_is_bot) {
				websocketppserver::connection_ptr con = serv->get_con_from_hdl(m_hdl);

				std::string other_snakes = "";
				for (snakelistpointer::iterator it = view_snake.begin(); it != view_snake.end(); ++it) {
					other_snakes += it->second->id + ":" + std::to_string((int)it->second->state) + ":" + std::to_string(it->second->m_sections[0].x) + ":" + std::to_string(it->second->m_sections[0].y) + ":" + std::to_string(it->second->m_is_boost) + ":" + std::to_string(it->second->current_size) + ":" + std::to_string(it->second->length) + ":" + std::to_string(it->second->radius) + "\n";
				}
				if (other_snakes.length() > 0) {
					other_snakes = other_snakes.substr(0, other_snakes.length() - 1);
				}

				std::string resp("4\n" + std::to_string((int)state) + "\n" + std::to_string(m_headPath[0].x) + ":" + std::to_string(m_headPath[0].y) + "\n" + std::to_string(m_is_boost) + "\n" + std::to_string(current_size) + "\n" + std::to_string(length) + "\n" + std::to_string(radius) + "\n" + std::to_string(m_viewbox.x) + ":" + std::to_string(m_viewbox.y) + ":" + std::to_string(m_viewbox.width) + ":" + std::to_string(m_viewbox.height) + "\n" + other_snakes);
				con->send(resp, websocketpp::frame::opcode::text);
			}
		}

		void send_leaderboard(websocketppserver *serv, std::string leaderbord, bool showCurrentRang) {
			if (!m_is_bot) {
				websocketppserver::connection_ptr con = serv->get_con_from_hdl(m_hdl);
				std::string resp("13\n" + leaderbord);
				if (showCurrentRang) {
					resp += "\t" + std::to_string(rang);
				}
				con->send(resp, websocketpp::frame::opcode::text);
			}
		}

		void turn(std::string _direction, bool isDown, websocketppserver *serv) {

			if (state == STATE::PLAYING) {
				m_direction = _direction == "LEFT" ? direction::left : _direction == "RIGHT" ? direction::right : direction::none;
				if (isDown) {
					m_speed = configuration.HebyFastSpeed;
					m_is_boost = 1;
				}
				else {
					m_speed = configuration.HebySlowSpeed;
					m_is_boost = 0;
				}
			}

		}

		void kill(map *_m, manager *_manager, websocketppserver *_serv, con_list *_conn) {

			if (state == STATE::PLAYING) {

				_m->remove(id, &m_sectorslist);

				int foods_quantity = 2;
				double_t average_mass = length / m_sections.size() / 2;

				Rand_int rnd{ 0, 2 * 314159265 };
				Rand_int rndradius{ (int)radius*-1, (int)radius };

				for (std::vector<section>::iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
					double_t angle = rnd() / 100000000;
					sector *v = _m->get_sector(it->x, it->y);
					double_t x = abs(v->x - it->x) + rndradius()*cos(angle);
					double_t y = abs(v->y - it->y) + rndradius()*sin(angle);
					for (int i = 0; i < foods_quantity; i++) {
						v->addFood(_manager->skinmanager, x, y, average_mass, false, true);
					}
				}

				point head = point(m_sections[0].x, m_sections[0].y);

				m_sections.clear();
				m_headPath.clear();

				m_sections.push_back(section(head.x, head.y));

				if (m_is_bot) {
					if (configuration.botRespawn) {
						state = STATE::RESPAWN;
						respawn();
					}
					else {
						state = STATE::DEAD;
					}
				}
				else {
					state = STATE::DYING;
				}

				timeDead = std::chrono::steady_clock::now();

			}

		}

		void kick() {

		}

		std::string isBot() {
			std::string r = "";
			if (m_is_bot) {
				r = "BOT";
			}
			else {
				r = "HUMAIN";
			}
			return r;
		}

		void setName(std::string _name) {
			name = _name;
			if (name.length() > configuration.HebyMaxNickLength) {
				if (configuration.HebyMaxNickLength - configuration.HebyCutNickname.length() >= 0) {
					name = name.substr(0, configuration.HebyMaxNickLength - configuration.HebyCutNickname.length()) + configuration.HebyCutNickname;
				}
				else if (configuration.HebyMaxNickLength > 0) {
					name = name.substr(0, configuration.HebyMaxNickLength);
				}
				else {
					name = "";
				}
			}
		}

		void setName(manager *_manager) {
			if (_manager->botNamesSimpleList) {
				Rand_int rnd{ 0, (int)_manager->botNamesList.size()-1 };
				botname_index = rnd();
				setName(_manager->botNamesList[botname_index]);
			} else {
				Rand_int rnd{ 0, (int)_manager->botnames_proportions.size() - 1 };
				botname_index = _manager->botnames_proportions[rnd()];
				
				if (_manager->botNamesComplexList[botname_index].isMember("name")) {
					if (_manager->botNamesComplexList[botname_index]["name"].isString()) {
						setName(_manager->botNamesComplexList[botname_index]["name"].asString());
					} else {
						setName(configuration.HebyDefaultNickname);
					}
				} else {
					setName(configuration.HebyDefaultNickname);
				}
			}
		}

		void setSkin(manager *_manager) {
			int skinindex = 0;

			Rand_int skin_rnd{ 0, (int)_manager->skinmanager->skins.size() - 1 };

			if (_manager->botNamesSimpleList) {
				skinindex = skin_rnd();
			} else if (m_is_bot) {
				if (_manager->botNamesSkinData.find(botname_index) != _manager->botNamesSkinData.end()) {
					if (_manager->botNamesSkinData[botname_index].size() > 0) {
						Rand_int skin_rnd2{ 0, (int)_manager->botNamesSkinData[botname_index].size() - 1 };
						skinindex = _manager->botNamesSkinData[botname_index][skin_rnd2()];
					}
					else {
						skinindex = skin_rnd();
					}
				} else {
					skinindex = skin_rnd();
				}
			} else {
				skinindex = skin_rnd();
			}
			skin = skinindex;
		}

		void play() {
			state = STATE::PLAYING;
		}

		connection_hdl get_hdl() {
			return m_hdl;
		}

		double_t get_angle() {
			return m_angle;
		}

		hysnake get() {
			hysnake s;
			s.id = id;
			s.length = length;
			s.name = name;
			s.precision = precision;
			s.m_direction = m_direction;
			s.size = size;
			s.skin = skin;
			s.current_size = current_size;
			s.radius = radius;
			s.rang = rang;
			s.head = hysection(m_sections[0].x, m_sections[0].y);
			s.state = state;
			s.timeDead = timeDead;
			return s;
		}

		void set(hysnake *s) {
			id = s->id;
			length = s->length;
			name = s->name;
			precision = s->precision;
			m_direction = s->m_direction;
			size = s->size;
			skin = s->skin;
			current_size = s->current_size;
			radius = s->radius;
			rang = s->rang;
			m_sections[0].x = s->head.x;
			m_sections[0].y = s->head.y;
			state = s->state;
			timeDead = s->timeDead;
		}

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

		std::vector<section> m_sections;

		STATE state;

		std::chrono::time_point<std::chrono::steady_clock> timeDead;

	private:

		double_t distformula(double_t x1, double_t y1, double_t x2, double_t y2) {
			double_t dx = x2 - x1;
			double_t dy = y2 - y1;
			return sqrt(dx*dx + dy * dy);
		}

		void interpolate(point next_point) {
			double_t xs = m_sections[0].x;
			double_t ys = m_sections[0].y;

			double_t px = abs(xs - next_point.x) / precision;
			double_t py = abs(ys - next_point.y) / precision;

			double_t signX = xs > next_point.x ? 1 : -1;
			double_t signY = ys > next_point.y ? 1 : -1;

			for (int i = 0; i < precision; i++) {
				m_headPath.insert(m_headPath.begin(), point(xs - i * px*signX, ys - i * py*signY));
				m_headPath.pop_back();
			}
			m_headPath.insert(m_headPath.begin(), point(next_point.x, next_point.y));
			m_headPath.pop_back();
		}

		void generateHeadPath() {

			for (unsigned i = 0; i < m_sections.size() - 1; i++) {

				double_t xs = m_sections[i].x;
				double_t ys = m_sections[i].y;

				double_t px = abs(xs - m_sections[i + 1].x) / precision;
				double_t py = abs(ys - m_sections[i + 1].y) / precision;

				double_t signX = xs > m_sections[i + 1].x ? 1 : -1;
				double_t signY = ys > m_sections[i + 1].y ? 1 : -1;

				for (int k = 0; k < precision; k++) {
					m_headPath.push_back(point(xs - k * px*signX, ys - k * py*signY));
				}
				m_headPath.push_back(point(m_sections[i + 1].x, m_sections[i + 1].y));

			}
		}

		int findNextPointIndex(int currentIndex) {
			
			double_t prefDist = configuration.HebyMinDistance * radius;
			double_t len = 0;
			int i = currentIndex+1;
			while (i + 1 < m_headPath.size()) {
				double_t dist = distformula(
					m_headPath[i].x, m_headPath[i].y,
					m_headPath[i + 1].x, m_headPath[i + 1].y
				);
				len += dist;
				if (len > prefDist) {
					if (i > m_headPath.size() - 1) {
						i = m_headPath.size() - 1;
					}
					return i;
				}
				i++;
			}
			if (i > m_headPath.size() - 1) {
				i = m_headPath.size() - 1;
			}
			return i;

		}

		void default_settings() {
			state = STATE::HOMEPAGE;

			name = configuration.HebyDefaultNickname;

			length = configuration.HebyStartLength;
			precision = 20;

			rang = -1;

			botname_index = 0;

			m_angle = (double_t)0;
			m_speed = configuration.HebySlowSpeed;
			skin = 0;

			m_is_boost = 0;

			// add the head
			Rand_int rndX{ 100, (int)configuration.mapSizeWidth - 100 };
			Rand_int rndY{ 100, (int)configuration.mapSizeHeight - 100 };
			section head(rndX(), rndY());
			m_sections.push_back(head);
			m_lastHeadPosition = point(m_sections[0].x, m_sections[0].y);
		}

		void init(connection_hdl hdl) {
			m_hdl = hdl;

			default_settings();
		}

		void init() {
			default_settings();
		}

		void random_turn() {
			Rand_int rnd{ 5,40 };
			if (m_bot_tick <= 0) {
				m_bot_tick = rnd();
				Rand_int rnd2{ 0,1000 };
				int r = rnd2();
				if (r > 666) {
					m_direction = direction::left;
				}
				else if (r > 333) {
					m_direction = direction::right;
				}
				else {
					m_direction = direction::none;
				}
			}
			m_bot_tick--;
		}

		void turn_snake(direction d) {
			double_t coeff = m_speed / configuration.HebySlowSpeed;
			if (d == direction::left) {
				m_angle += M_PI_ / m_zoom / (double_t)20 * coeff * configuration.HebyTurnSpeed * 1.5;
			}
			else if (d == direction::right) {
				m_angle -= M_PI_ / m_zoom / (double_t)20 * coeff * configuration.HebyTurnSpeed * 1.5;
			}
			m_angle = std::fmod(m_angle, 2 * M_PI_);
		}

		void collision_food(sector* v, manager* m) {

				std::vector<food> *foods = v->getFoods();
				std::vector<int> food_to_delete;
				std::vector<bool> food_to_respawn;
				for (unsigned j = 0; j < foods->size(); j++) {
					double_t pos_x = v->x + (*foods)[j].x;
					double_t pos_y = v->y + (*foods)[j].y;
					double_t dist = distformula(m_sections[0].x, m_sections[0].y, pos_x, pos_y);

					if ((*foods)[j].timeout > 0) {

						auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - (*foods)[j].time);

						if (elapsed.count() > (*foods)[j].timeout) {
							food_to_delete.push_back(j);
							if ((*foods)[j].respawn) {
								food_to_respawn.push_back(true);
							}
							continue;
						}
					}

					if (dist < radius) {
						double_t angle_test = atan2(m_sections[0].y - pos_y, m_sections[0].x - pos_x);
						(*foods)[j].x += dist * cos(angle_test);
						(*foods)[j].y += dist * sin(angle_test);
						if (length < configuration.HebyMaxLength || configuration.HebyMaxLength < 0) {
							length += (*foods)[j].mass;
						}
						bool is_eaten = true;
						if (m->gamemode_loaded) {
							hysnake s = get();
							hyfood f = (*foods)[j].get();
							m->gamemode->on_food_collide(&f, &s);
							set(&s);
						}
						if (is_eaten) {
							food_to_delete.push_back(j);
							if ((*foods)[j].respawn) {
								food_to_respawn.push_back(true);
							}
						}
						if (m->gamemode_loaded && is_eaten) {
							hysnake s = get();
							hyfood f = (*foods)[j].get();
							m->gamemode->on_food_eaten(&f,&s);
							set(&s);
						}
					}
					if ((*foods)[j].is_attract) {
						double_t angle_test = atan2(m_sections[0].y - pos_y, m_sections[0].x - pos_x);
						for (double_t k = 0; k < (*foods)[j].coefficient+1.0; k += 0.5) {
							double_t test_pos_x = v->x + (*foods)[j].x;
							double_t test_pos_y = v->y + (*foods)[j].y;
							double_t dist2 = distformula(m_sections[0].x, m_sections[0].y, test_pos_x, test_pos_y);
							if (dist < radius * k) {
								double_t s = (*foods)[j].speed_attract > dist2 ? dist2 : (*foods)[j].speed_attract;
								(*foods)[j].x += s * cos(angle_test);
								(*foods)[j].y += s * sin(angle_test);
							}
						}
					}
				}
				for (unsigned j = 0; j < food_to_delete.size(); j++) {
					(*foods)[food_to_delete[j]] = foods->back();
					foods->pop_back();
				}
				for (unsigned j = 0; j < food_to_respawn.size(); j++) {
					v->addFood(m->skinmanager);
				}
				calc_property();
				resize();
		}

		void collision_snake(std::map<std::string, snake*>* list_snake, map *_m, manager *_manager, websocketppserver *_serv, con_list *_conn) {

			bool is_dead = false;

			for (snakelistpointer::iterator it = list_snake->begin(); it != list_snake->end(); ++it) {

				for (unsigned i = 0; i < it->second->m_sections.size(); i++) {

					double_t dist = distformula(m_sections[0].x, m_sections[1].y, it->second->m_sections[i].x, it->second->m_sections[i].y);

					if (dist < radius + it->second->radius) {
						hysnake current = get();
						hysnake other = it->second->get();
						if (_manager->gamemode_loaded) {
							is_dead = _manager->gamemode->on_collide(&current, &other);
							set(&current);
							it->second->set(&other);
						} else {
							is_dead = true;
						}
						if (_manager->gamemode_loaded && is_dead) {
							_manager->gamemode->on_kill(&current,&other);
							set(&current);
							it->second->set(&other);
						}
						break;
					}

				}

			}

			if (is_dead) {
				kill(_m,_manager,_serv,_conn);
			}
		}

		void calc_property() {

			double_t radius_init = calc_radius(10);

			radius = calc_radius(length);
			size = 10 + (int)(sqrt(length)*2);
			current_size = m_sections.size();
			m_zoom = radius / radius_init;

		}

		double_t calc_radius(double_t _length) {
			return 10 + sqrt(sqrt(_length*log(100 + _length)*log(100 + _length)));
		}

		void resize() {
			int c_size = m_sections.size();
			if (size > c_size) {
				initSections(size - c_size);
			} else if (size < c_size) {
				for (int i = 0; i < c_size - size; i++) {
					m_sections.pop_back();
					for (int j = 0; j < precision; j++) {
						m_headPath.pop_back();
					}
				}
			}
		}

		void respawn() {
			m_sections.clear();
			m_headPath.clear();
			m_is_boost = 0;
			rang = -1;
			length = configuration.HebyStartLength;
			Rand_int rndX{ 100, (int)configuration.mapSizeWidth - 100 };
			Rand_int rndY{ 100, (int)configuration.mapSizeHeight - 100 };
			section head(rndX(), rndY());
			m_sections.push_back(head);
			m_lastHeadPosition = point(m_sections[0].x, m_sections[0].y);
			calc_property();
			initSections(size);
			play();
		}

		config configuration;

		connection_hdl m_hdl;

		point m_lastHeadPosition;

		double_t m_angle;
		double_t m_zoom;

		int m_speed;
		int m_is_boost;

		bool m_is_bot;
		int m_bot_tick;
		int botname_index;

		box m_box;
		box m_viewbox;

		std::map<std::string, bool> m_sectorslist;
		std::map<std::string, bool> m_testsector;
		std::vector<point> m_headPath;

		typedef std::map<std::string, snake*> snakelistpointer;
		snakelistpointer view_snake;
	};

	typedef std::map<std::string, snake> snakelist;

} // namespace heby



#endif //HEBY_SNAKE_HPP
