#ifndef HEBY_INI_READER_HPP
#define HEBY_INI_READER_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include <algorithm> 
#include <cctype>
#include <locale>

namespace heby {

	class ini_reader {
	public:
		ini_reader(std::string filename) {
			m_f = filename;
		}
		bool parse() {
			bool is_parse = true;
			std::string line;
			std::string section = "";
			std::ifstream infile(m_f);
			while (std::getline(infile, line))
			{
				trim(line);

				// data
				std::string data = line;
				if (line.find_first_of(';') >= 0 && line.find_first_of(';') < line.length()) {
					// No comment at the end
					data = line.substr(0, line.find_first_of(';'));
					trim(data);
				}

				// comment line
				if (data == "") {
					continue;
				}

				// section line
				if (data.substr(0,1) == "[" && data.substr(data.length() - 1) == "]") {
					section = data.substr(1, data.length() - 2);
					continue;
				}

				
				if (data.find_first_of('=') < 0 && data.find_first_of('=') < data.length()) {
					// Error Ignored
					is_parse = false;
					continue;
				}
				std::string key = data.substr(0, data.find_first_of('='));
				trim(key);
				std::string content = data.substr(data.find_first_of('=')+1);
				trim(content);
				if (!key.empty()) {
					m_ini[section + "_" + key] = std::string(content.c_str());
				}
				else {
					is_parse = false;
				}
			}
			return is_parse;
		}
		// Get a string value from INI file, returning default_value if not found.
		std::string Get(std::string section, std::string name, const std::string& default_value) {
			if (m_ini.find(section + "_" + name) != m_ini.end()) {
				return m_ini[section + "_" + name];
			}
			return default_value;
		}

		// Get a string value from INI file, returning default_value if not found,
		// empty, or contains only whitespace.
		std::string GetString(std::string section, std::string name, const std::string& default_value) {
			if (m_ini.find(section + "_" + name) != m_ini.end()) {
				return m_ini[section + "_" + name];
			}
			return default_value;
		}

		// Get an integer (long) value from INI file, returning default_value if
		// not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
		long GetLong(std::string section, std::string name, long default_value) {
			if (m_ini.find(section + "_" + name) != m_ini.end()) {
				return std::stol(m_ini[section + "_" + name]);
			}
			return default_value;
		}

		// Get an integer (int) value from INI file, returning default_value if
		// not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
		long GetInteger(std::string section, std::string name, long default_value) {
			if (m_ini.find(section + "_" + name) != m_ini.end()) {
				return std::stoi(m_ini[section + "_" + name]);
			}
			return default_value;
		}

		// Get a real (floating point double) value from INI file, returning
		// default_value if not found or not a valid floating point value
		// according to strtod().
		double GetReal(std::string section, std::string name, double default_value) {
			if (m_ini.find(section + "_" + name) != m_ini.end()) {
				return std::stod(m_ini[section + "_" + name]);
			}
			return default_value;
		}

		// Get a boolean value from INI file, returning default_value if not found or if
		// not a valid true/false value. Valid true values are "true", "yes", "on", "1",
		// and valid false values are "false", "no", "off", "0" (not case sensitive).
		bool GetBoolean(std::string section, std::string name, bool default_value) {
			if (m_ini.find(section + "_" + name) != m_ini.end()) {
				std::string d;
				std::string s = m_ini[section+"_"+name];
				d.resize(s.size());
				std::transform(s.begin(), s.end(), d.begin(), ::tolower);
				return d == "true" || d == "yes" || d == "1";

			}
			return default_value;
		}
	private:
		std::string m_f;
		std::map<std::string, std::string> m_ini;
	};

} // namespace heby



#endif //HEBY_INI_READER_HPP