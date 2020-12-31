#ifndef HEBY_HPP
#define HEBY_HPP

#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>
#include <set>

# define M_PI_           (double_t)3.14159265358979323846

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


#include <heby/common/dynLib.hpp>
#include <heby/common/mode.hpp>
#include <heby/entities/snake.hpp>
#include <heby/entities/player.hpp>
#include <heby/common/skin_manager.hpp>
#include <heby/common/manager.hpp>
#include <heby/common/server.hpp>
#include <heby/common/command.hpp>
#include <heby/common/game.hpp>

#endif //HEBY_HPP