#ifndef HEBY_COMMAND_HPP
#define HEBY_COMMAND_HPP

#include <heby/logger/log.hpp>
#include <heby/config/config.hpp>
#include <heby/common/server.hpp>
#include <heby/entities/snake.hpp>
#include <heby/entities/player.hpp>

#include <iostream>
#include <chrono>
#include <future>
#include <string>
#include <thread>

namespace heby {


	std::string GetLineFromCin() {
		std::string line;
		std::getline(std::cin, line);
		return line;
	}

	class command {
	public:
		command() {
			
		}
		void loop(server *s) {

			m_server = s;

			auto future = std::async(std::launch::async, GetLineFromCin);

			while (true) {
				if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
					auto line = future.get();

					// Set a new line. Subtle race condition between the previous line
					// and this. Some lines could be missed. To aleviate, you need an
					// io-only thread. I'll give an example of that as well.
					future = std::async(std::launch::async, GetLineFromCin);

					analyse(line);
				}

				//std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
		void analyse(std::string data) {
			trim(data);

			std::string command = data;
			std::string args = "";
			if (data.find_first_of(" ") > 0 && data.find_first_of(" ") < data.length()) {
				command = data.substr(0, data.find_first_of(" "));
				args = data.substr(data.find_first_of(" ") + 1);
			}

			if (command == "echo") {
				std::cout << args << std::endl;
				return;
			}
			if (command == "list_connection") {
				typedef std::map<connection_hdl, std::string, std::owner_less<connection_hdl>> playerlist;
				playerlist::iterator it;
				int index = 0;

				std::cout << "==== ================ ========================= =========== " << std::endl;
				std::cout << " ID   IP ENDPOINT      PLAYER_NAME               LENGTH" << std::endl;

				websocketppserver* serv = m_server->getWebSocketServer();

				for (it = m_server->getManager()->players.begin(); it != m_server->getManager()->players.end(); ++it) {
					snake &current_snake = m_server->getManager()->snakes->at(it->second);
					websocketppserver::connection_ptr connection = serv->get_con_from_hdl(it->first);
					auto local_endpoint = connection->get_raw_socket().local_endpoint();
					std::string local_address = local_endpoint.address().to_string();
					std::cout << "  " << index << "    " << local_address << "     angle: " << current_snake.get_angle() << std::endl;
					index++;
				}
				return;
			}

			m_console.log(std::cout) << "Unknown command. Type 'help' for help." << std::endl;
		}
	private:
		console m_console;
		server *m_server;
	};


} // namespace heby


#endif //HEBY_COMMAND_HPP