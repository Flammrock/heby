#ifndef HEBY_LOG_HPP
#define HEBY_LOG_HPP

#include <heby/logger/termcolor.hpp>

namespace heby {

	// level typdef info
	typedef int level;

	// level logger structure
	struct alevel {

		// Special aggregate value representing "no levels"
		static level const none = 0x0;
		// Low level debugging information (warning: very chatty)
		static level const devel = 0x1;
		// Information about unusual system states or other minor internal library
		// problems, less chatty than devel.
		static level const library = 0x2;
		// Information about minor configuration problems or additional information
		// about other warnings.
		static level const info = 0x4;
		// Information about important problems not severe enough to terminate
		// connections.
		static level const warn = 0x8;
		// Recoverable error. Recovery may mean cleanly closing the connection with
		// an appropriate error code to the remote endpoint.
		static level const rerror = 0x10;
		// Unrecoverable error. This error will trigger immediate unclean
		// termination of the connection or endpoint.
		static level const fatal = 0x20;
		// Special aggregate value representing "all levels"
		static level const all = 0xffffffff;

	};

	// LOG class
	class console {
	public:
		console() {
			m_level = alevel::all;
		}
		void set_channel(level l) {
			m_level = l;
		}
		level get_level() {
			return m_level;
		}

		inline std::ostream& log(std::ostream& stream) {
			stream << termcolor::reset << "[" << termcolor::green << "Console" << termcolor::reset << "] ";
			return stream;
		}
		inline std::ostream& warn(std::ostream& stream) {
			stream << termcolor::reset << "[" << termcolor::yellow << "Warn" << termcolor::reset << "] ";
			return stream;
		}
		inline std::ostream& info(std::ostream& stream) {
			stream << termcolor::reset << "[" << termcolor::cyan << "Info" << termcolor::reset << "] ";
			return stream;
		}
		inline std::ostream& error(std::ostream& stream) {
			stream << termcolor::reset << "[" << termcolor::red << "Error" << termcolor::reset << "] ";
			return stream;
		}
		inline std::ostream& game(std::ostream& stream) {
			stream << termcolor::reset << "[" << termcolor::on_lightgrey << termcolor::darkred << "Game" << termcolor::reset << "] ";
			return stream;
		}
		inline std::ostream& server(std::ostream& stream) {
			stream << termcolor::reset << "[" << termcolor::magenta << "Server" << termcolor::reset << "] ";
			return stream;
		}

	private:
		level m_level;
	};

} // namespace heby


#endif //HEBY_LOG_HPP