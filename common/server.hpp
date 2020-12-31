#ifndef HEBY_SERVER_HPP
#define HEBY_SERVER_HPP

/* on_open insert connection_hdl into channel
 * on_close remove connection_hdl from channel
 * on_message queue send to all channels
 */

#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <algorithm>

#include <heby/logger/log.hpp>
#include <heby/common/manager.hpp>
#include <heby/entities/map.hpp>
#include <heby/common/skin_manager.hpp>
#include <heby/common/helper.hpp>
#include <heby/common/mode.hpp>

#include <websocketpp/common/thread.hpp>

typedef websocketpp::server<websocketpp::config::asio> websocketppserver;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

namespace heby {

	enum action_type {
		SUBSCRIBE,
		UNSUBSCRIBE,
		MESSAGE
	};

	struct action {
		action(action_type t, connection_hdl h) : type(t), hdl(h) {}
		action(action_type t, connection_hdl h, websocketppserver::message_ptr m)
			: type(t), hdl(h), msg(m) {}
		action_type type;
		websocketpp::connection_hdl hdl;
		websocketppserver::message_ptr msg;
	};

	class server {
	public:
		server() {
			// Initialize Asio Transport
			m_server.init_asio();

			// Register handler callbacks
			m_server.set_open_handler(bind(&server::on_open, this, ::_1));
			m_server.set_close_handler(bind(&server::on_close, this, ::_1));
			m_server.set_message_handler(bind(&server::on_message, this, ::_1, ::_2));

			m_server.clear_access_channels(websocketpp::log::alevel::all);
			m_server.clear_error_channels(websocketpp::log::alevel::all);
		}

		void run(uint16_t port, manager m) {
			// add manager
			m_manager = m;

			// listen on specified port
			m_server.listen(port);

			// Start the server accept loop
			m_server.start_accept();

			// create the map
			m_map = map(&m_manager);
			m_map.generate();

			// Start the ASIO io_service run loop
			try {
				m_manager.console.server(std::cout) << "listening on port " << termcolor::blue << port << termcolor::reset << std::endl;
				std::cout << std::endl;
				m_manager.console.log(std::cout) << "Type 'help' for help." << std::endl << std::endl;
				m_server.run();
			}
			catch (const std::exception & e) {
				std::cout << e.what() << std::endl;
			}
		}

		void on_open(connection_hdl hdl) {
			{
				lock_guard<mutex> guard(m_action_lock);
				//std::cout << "on_open" << std::endl;
				m_actions.push(action(SUBSCRIBE, hdl));
			}
			m_action_cond.notify_one();
		}

		void on_close(connection_hdl hdl) {
			{
				lock_guard<mutex> guard(m_action_lock);
				//std::cout << "on_close" << std::endl;
				m_actions.push(action(UNSUBSCRIBE, hdl));
			}
			m_action_cond.notify_one();
		}

		void on_message(connection_hdl hdl, websocketppserver::message_ptr msg) {
			// queue message up for sending by processing thread
			{
				lock_guard<mutex> guard(m_action_lock);
				//std::cout << "on_message" << std::endl;
				m_actions.push(action(MESSAGE, hdl, msg));
			}
			m_action_cond.notify_one();
		}

		void process_messages() {
			while (1) {
				unique_lock<mutex> lock(m_action_lock);

				while (m_actions.empty()) {
					m_action_cond.wait(lock);
				}

				action a = m_actions.front();
				m_actions.pop();

				lock.unlock();

				if (a.type == SUBSCRIBE) {
					lock_guard<mutex> guard(m_connection_lock);
					m_connections.insert(a.hdl);

					if (m_manager.players.size() >= m_manager.configuration.maxConnections) {
						websocketppserver::connection_ptr con = m_server.get_con_from_hdl(a.hdl);
						std::string resp("12\nSorry, Server is full..Please try later..");
						con->send(resp, websocketpp::frame::opcode::text);
						con->close(websocketpp::close::status::normal, "Server is Full");
					} else {
						std::string id = random_string(20);

						m_manager.players.insert({ a.hdl , id });
						m_manager.snakes->insert({ id, snake(id,a.hdl,&m_manager,-1) });

						snake &current_snake = m_manager.snakes->at(id);

						if (m_manager.gamemode_loaded) {
							hysnake s = current_snake.get();
							m_manager.gamemode->on_join(&s);
							current_snake.set(&s);
						}

						websocketppserver::connection_ptr con = m_server.get_con_from_hdl(a.hdl);

						std::string pre = m_manager.skinmanager->get_data();
						pre.erase(std::remove(pre.begin(), pre.end(), '\n'), pre.end());
						if (m_manager.gamemode_loaded) {
							std::string r = m_manager.gamemode->on_join_data_send(pre);
							if (!r.empty()) {
								pre = r;
							}
						}


						
						
						std::string pre2 = m_manager.configuration.getData();
						pre2.erase(std::remove(pre2.begin(), pre2.end(), '\n'), pre2.end());
						std::string resp("9\n" + pre + "\n" + m_manager.pattern + "\n" + pre2);
						con->send(resp, websocketpp::frame::opcode::text);
					}

					

					//m_manager.console.info(std::cout) << "(" << id << ")" << "New Connection!" << std::endl;
				}
				else if (a.type == UNSUBSCRIBE) {
					lock_guard<mutex> guard(m_connection_lock);

					if (m_manager.players.find(a.hdl) != m_manager.players.end()) {
						std::string id = m_manager.players.at(a.hdl);

						//m_manager.console.info(std::cout) << "(" << id << ")" << "Connection Lost!" << std::endl;
						//m_manager.snakes->erase(id);

						snake &current_snake = m_manager.snakes->at(id);

						if (m_manager.gamemode_loaded) {
							hysnake s = current_snake.get();
							m_manager.gamemode->on_left(&s);
							current_snake.set(&s);
						}

						current_snake.state = STATE::DEAD;
						m_manager.players.erase(a.hdl);
						m_connections.erase(a.hdl);
					}

					
				}
				else if (a.type == MESSAGE) {
					lock_guard<mutex> guard(m_connection_lock);

					// analyse MESSAGE

					// use a simple protocol
					/*

					0 -> get entire snake
					1 -> set angle

					*/
					if (m_manager.players.find(a.hdl) != m_manager.players.end()) {
						std::string id = m_manager.players.at(a.hdl);
						if (m_manager.snakes->find(id) != m_manager.snakes->end()) {
							// snake is alive



							snake &current_snake = m_manager.snakes->at(id);

							std::string raw_data = a.msg->get_payload();

							int i = raw_data.find_first_of("\n");
							if (i > 0 && i < raw_data.length()) {
								std::string command = raw_data.substr(0, i);
								std::string data = raw_data.substr(i + 1);

								if (command == "0") {
									try {
										std::vector<std::string> player_data = split(data, "\n");

										Rand_int skin_rnd{ 0, (int)m_manager.skinmanager->skins.size() - 1 };

										if (player_data.size() > 0) {
											if (player_data.size() < 2) {
												try {
													current_snake.skin = std::stoi(player_data[0]);
												}
												catch (...) {
													current_snake.skin = skin_rnd();
												}
												current_snake.setName(m_manager.configuration.HebyDefaultNickname);
											}
											else {
												current_snake.setName(player_data[0]);
												try {
													current_snake.skin = std::stoi(player_data[1]);
												}
												catch (...) {
													current_snake.skin = skin_rnd();
												}
											}
										}
										else {

											current_snake.setName(m_manager.configuration.HebyDefaultNickname);
											current_snake.skin = skin_rnd();
										}

										if (m_manager.gamemode_loaded) {
											hysnake s = current_snake.get();
											m_manager.gamemode->on_play(&s);
											current_snake.set(&s);
										}

										current_snake.play();

										websocketppserver::connection_ptr con = m_server.get_con_from_hdl(a.hdl);
										std::string resp("0\n" + current_snake.id + "\n" + current_snake.name + "\n" + std::to_string(current_snake.precision) + "\n" + std::to_string(current_snake.length) + "\n" + std::to_string(current_snake.radius) + "\n" + std::to_string(current_snake.skin) + "\n" + current_snake.get_data());
										con->send(resp, websocketpp::frame::opcode::text); // For now, we accept directly the snake
									}
									catch (...) {}
								}

								if (command == "1") {
									double_t angle = std::stod(command);
									std::cout << "angle:" << angle << std::endl;
									current_snake.update_angle(angle);
									websocketppserver::connection_ptr con = m_server.get_con_from_hdl(a.hdl);
									std::string resp("1\n");
									con->send(resp, websocketpp::frame::opcode::text);
								}

								// LEFT TURN
								if (command == "2") {
									bool mouseDown = false;
									if (data == "1") {
										mouseDown = true;
									}
									current_snake.turn("LEFT", mouseDown, &m_server);
								}
								// RIGHT TURN
								if (command == "3") {
									bool mouseDown = false;
									if (data == "1") {
										mouseDown = true;
									}
									current_snake.turn("RIGHT", mouseDown, &m_server);
								}

								if (command == "4") {
									bool mouseDown = false;
									if (data == "1") {
										mouseDown = true;
									}
									current_snake.turn("NONE", mouseDown, &m_server);
								}
							}

						}
					}

					

					/*con_list::iterator it;
					for (it = m_connections.begin(); it != m_connections.end(); ++it) {
						m_server.send(*it, a.msg);
					}*/
				}
				else {
					// undefined.
				}
			}
		}
		manager* getManager() {
			return &m_manager;
		}
		websocketppserver* getWebSocketServer() {
			return &m_server;
		}
		map* getMap() {
			return &m_map;
		}
		typedef std::set<connection_hdl, std::owner_less<connection_hdl> > con_list;
		con_list* getConnection() {
			return &m_connections;
		}
	private:
		typedef std::set<connection_hdl, std::owner_less<connection_hdl> > con_list;

		websocketppserver m_server;
		manager m_manager;
		map m_map;
		con_list m_connections;
		std::queue<action> m_actions;

		mutex m_action_lock;
		mutex m_connection_lock;
		condition_variable m_action_cond;
	};

}

#endif //HEBY_SERVER_HPP