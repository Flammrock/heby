#ifndef HEBY_SKIN_HPP
#define HEBY_SKIN_HPP

#include <string>
#include <map>

#include <heby/common/helper.hpp>
#include <heby/logger/log.hpp>

#include <iostream>
#include <iostream>
#include <fstream>


#ifndef JSON_IS_AMALGAMATION
#define JSON_IS_AMALGAMATION
#include <jsoncpp.hpp>
#endif

namespace heby {

	
	class dataskin {
	public:
		dataskin(std::string _filename, std::string _content, Json::Value _data) {
			filename = _filename;
			content = _content;
			data = _data;
		}
		int length() {
			return data.size();
		}
		std::string getByIndex(int skinindex) {
			
			std::string key;

			if (skinindex < 0) {
				skinindex = 0;
			}
			if (skinindex > length() - 1) {
				skinindex = length() - 1;
			}

			int i = 0;
			for (Json::ValueIterator it = data.begin(); it != data.end(); it++) {
				if (i == skinindex) {
					key = it.key().asCString();
				}
				i++;
			}

			return key;
		}
		std::string content;
		std::string filename;
		Json::Value data;
	private:
	};
	

	class skin {
	public:
		skin() {

			
			files["skins.json"] = "{\r\n  \"0\":[\"#000\"],\r\n  \"aka\":[\r\n    \"#E50000\"\r\n  ],\r\n  \"nanatsu\":[\r\n    \"#B30101\",\r\n    \"#000D7D\",\r\n    \"#04A80E\",\r\n    \"#E9DB03\",\r\n    \"#70108D\",\r\n    \"#E45A05\",\r\n    \"#D70383\"\r\n  ],\r\n  \"kumo\":[\r\n    \"#242424\"\r\n  ],\r\n  \"buru\":[\r\n    \"#00A4FF\"\r\n  ],\r\n  \"orenji\":[\r\n    \"#F55E09\"\r\n  ],\r\n  \"kiiro\":[\r\n    \"#FFC800\"\r\n  ],\r\n  \"pinku\":[\r\n    \"#FC91F3\"\r\n  ],\r\n  \"gure\":[\r\n    \"#787171\"\r\n  ],\r\n  \"hakujin\":[\r\n    \"#EBEBEB\"\r\n  ],\r\n  \"kaiyo\":[\r\n    \"#051597\",\r\n    \"#051FFF\",\r\n    \"#051FFF\",\r\n    \"#3F8FFF\",\r\n    \"#0BB0F4\"\r\n  ],\r\n  \"hono\":[\r\n    \"#B00202\",\r\n    \"#DB0606\",\r\n    \"#FF0000\",\r\n    \"#FF3300\",\r\n    \"#FF6100\"\r\n  ],\r\n  \"inron\":[\r\n    \"#EBEBEB\",\r\n    \"#EBEBEB\",\r\n    \"#EBEBEB\",\r\n    \"#EBEBEB\",\r\n    \"#EBEBEB\",\r\n    \"#EBEBEB\",\r\n    \"#242424\",\r\n    \"#242424\",\r\n    \"#242424\",\r\n    \"#242424\",\r\n    \"#242424\",\r\n    \"#242424\"\r\n  ],\r\n  \"papuru\":[\r\n    \"#9600FF\"\r\n  ],\r\n  \"furansu\":[\r\n    \"#0015FF\",\r\n    \"#FFFFFF\",\r\n    \"#E00202\"\r\n\r\n  ],\r\n  \"d1\":[\r\n    \"#000000\",\r\n    \"#FCCC04\",\r\n    \"#FC3D3D\"\r\n  ],\r\n  \"d2\":[\r\n    \"#FF2F2F\",\r\n    \"#FFFFFF\",\r\n    \"#FF2F2F\"\r\n  ],\r\n  \"d3\":[\r\n    \"#0709BA\",\r\n    \"#0709BA\",\r\n    \"#0709BA\",\r\n    \"#FED303\",\r\n    \"#FED303\",\r\n    \"#FED303\"\r\n  ],\r\n  \"d4\":[\r\n    \"#0763CE\",\r\n    \"#0763CE\",\r\n    \"#000000\",\r\n    \"#000000\",\r\n    \"#FFFFFF\",\r\n    \"#FFFFFF\"\r\n  ],\r\n  \"d5\":[\r\n    \"#FFFFFF\",\r\n    \"#FFFFFF\",\r\n    \"#FFFFFF\",\r\n    \"#FFFFFF\",\r\n    \"#074894\",\r\n    \"#074894\",\r\n    \"#074894\",\r\n    \"#074894\"\r\n\r\n  ],\r\n  \"d6\":[\r\n    \"#009109\",\r\n    \"#009109\",\r\n    \"#FFFFFF\",\r\n    \"#FFFFFF\",\r\n    \"#E80000\",\r\n    \"#E80000\"\r\n\r\n  ],\r\n  \"gurin\":[\r\n    \"#06D600\"\r\n  ],\r\n  \"akaao\":[\r\n    \"#000990\",\r\n    \"#0412F7\",\r\n    \"#AB0404\",\r\n    \"#E80606\"\r\n  ]\r\n\r\n\r\n\r\n\r\n}\r\n";
			

		}
		void load(std::string file) {

			if (!file_exists(file)) {
				file_create(file, files[file]);
			}

			std::string line;
			std::string section = "";
			std::ifstream infile(file);
			std::string content = "";
			while (std::getline(infile, line))
			{
				content += line;
			}

			Json::Value d;
			Json::Reader reader;
			bool isParse = reader.parse(content.c_str(), d);

			if (!isParse) {
				m_console.warn(std::cout) << "Skins file '" << file << "' (syntax error)" << std::endl;
				content = files[file];
				reader.parse(content.c_str(), d);
			}

			tempdata.push_back(dataskin(file, content, d));
			//data[file] = &tempdata.at(tempdata.size()-1);
			data[file] = &tempdata.at(tempdata.size() - 1);

			//m_console.game(std::cout) << d.size() << " Skins loaded!" << std::endl;
		}
		std::map<std::string, dataskin*> data;
	private:
		console m_console;
		std::vector<dataskin> tempdata;
		std::map<std::string, std::string> files;
	};

} // namespace heby


#endif //HEBY_SKIN_HPP