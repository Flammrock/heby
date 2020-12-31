#ifndef HEBY_GAME_HPP
#define HEBY_GAME_HPP

#include <heby/common/manager.hpp>
#include <heby/common/server.hpp>
#include <heby/entities/snake.hpp>
#include <heby/common/helper.hpp>
#include <heby/common/skin_manager.hpp>

#include <chrono>

namespace heby {

	typedef std::map<connection_hdl, std::string, std::owner_less<connection_hdl>> playerlist;

	class game {
	public:
		void loop(server *s) {

			m_server = s;
			std::chrono::time_point<std::chrono::steady_clock> lastFrame = std::chrono::steady_clock::now();

			typedef std::set<connection_hdl, std::owner_less<connection_hdl> > con_list;
			manager *manager = m_server->getManager();
			map *map = m_server->getMap();
			websocketppserver *serv = m_server->getWebSocketServer();
			con_list *connections = m_server->getConnection();

			Sleep(1000);

			int FPS = 1000 / manager->configuration.FPS; // FPS

			// Add bot(s)
			for (int i = 0; i < manager->configuration.botAmount; i++) {
				std::string id = random_string(20);
				(*manager->snakes)[id] = snake(id, manager, -1);
				(*manager->snakes)[id].play();
			}

			while (true) {

				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastFrame);
				if (elapsed.count() < FPS) {
					Sleep(FPS - elapsed.count());
				}
				lastFrame = std::chrono::steady_clock::now();

				bool update_leaderboard = false;
				std::map<std::string, double_t> temp_leaderboard;
				std::string rawleaderboard;
				auto elapsed_leaderboard = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - manager->leaderboard_time);
				if (elapsed_leaderboard.count() > manager->configuration.leaderBoardUpdate) {
					manager->leaderboard_time = std::chrono::steady_clock::now();
					update_leaderboard = true;
					snakelist::iterator it2;
					for (it2 = manager->snakes->begin(); it2 != manager->snakes->end(); ++it2) {
						if (it2->second.state == STATE::PLAYING) {
							temp_leaderboard[it2->second.id] = it2->second.length;
						}
					}
					std::multimap<double_t, std::string> reverseTest = flip_map(temp_leaderboard);
					int index_leaderboard = 0;
					for (std::multimap<double_t, std::string>::const_reverse_iterator it = reverseTest.rbegin(); it != reverseTest.rend(); ++it) {
						manager->snakes->at(it->second).rang = index_leaderboard + 1;
						if (index_leaderboard < manager->configuration.leaderBoardSize) {
							rawleaderboard += it->second + "\r" + manager->snakes->at(it->second).name + "\r" + std::to_string(it->first) + "\n";
						}
						index_leaderboard++;
					}
					if (rawleaderboard.length() > 0) {
						rawleaderboard = rawleaderboard.substr(0, rawleaderboard.length() - 1);
					}
				}
				
				snakelist::iterator it;
				std::vector<std::string> snake_dead;
				for (it = manager->snakes->begin(); it != manager->snakes->end(); ++it) {

					if (it->second.state == STATE::PLAYING || it->second.state == STATE::DYING) {

						try {

							it->second.update_position(map, manager, serv, connections);

							if (update_leaderboard) {
								it->second.send_leaderboard(serv, rawleaderboard, manager->configuration.leaderBoardShowCurrentPlayer);
							}
							it->second.send_data(serv);

						} catch (...) {}

					} else if (it->second.state == STATE::DEAD) {

						snake_dead.push_back(it->second.id);

					}

				}

				for (unsigned i = 0; i < snake_dead.size(); i++) {
					manager->snakes->erase(snake_dead[i]);
				}

			}

		}
	private:
		server *m_server;
	};


} // namespace heby


#endif //HEBY_GAME_HPP