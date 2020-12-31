#ifndef HEBY_SKINMANAGER_HPP
#define HEBY_SKINMANAGER_HPP

#include <string>
#include <map>

#include <heby/common/helper.hpp>
#include <heby/logger/log.hpp>
#include <heby/config/config.hpp>


#include <iostream>
#include <iostream>
#include <fstream>

#ifndef JSON_IS_AMALGAMATION
#define JSON_IS_AMALGAMATION
#include <jsoncpp.hpp>
#endif


namespace heby {

	

	class skinmanager {
	public:
		skinmanager() {
			folder = "Skins";
			is_loaded = false;
		}

		void load(config _config) {

			config = _config;

			folder = config.HebySkins;
			
			if (dirExists(folder.c_str())) {
				skins = getAllSkins();
			} else {
				if (dirCreate(folder)) {

					m_console.info(std::cout) << "Skin Directory created!" << std::endl;
					Json::Value basicSkin = getBasicSkin();
					if (!writeSkin(basicSkin)) {
						m_console.error(std::cout) << "Error when create skin files..." << std::endl;
						skins = cleanSkin(basicSkin);
					} else {
						skins = getAllSkins();
					}
				} else {
					m_console.warn(std::cout) << "Unable to create Skin Directory... Only Basic Skin Color loaded!" << std::endl;
					skins = cleanSkin(getBasicSkin());
				}
			}

			food_skins = getFoodSkin();

			is_loaded = true;
		}

		std::string get_data() {
			Json::FastWriter fastWriter;
			Json::Value test;
			test["skin"] = skins;
			test["food_skin"] = food_skins;
			test["data"] = dataFolder;
			return fastWriter.write(test);
		}

		Json::Value skins;
		Json::Value food_skins;
		Json::Value dataFolder;
		std::map<int, int> food_proportions;
		bool is_loaded;

	private:

		Json::Value getBasicSkin() {
			Json::Value basicSkin;

			int index = 0;

			basicSkin[index]["name"] = "Aka";
			basicSkin[index]["folderPath"] = "Basic Color";
			basicSkin[index]["cost"] = 0;
			basicSkin[index]["colors"][0][0] = 216;
			basicSkin[index]["colors"][0][1] = 9;
			basicSkin[index]["colors"][0][2] = 9;
			index++;

			basicSkin[index]["name"] = "Kyarameru";
			basicSkin[index]["folderPath"] = "Basic Color";
			basicSkin[index]["cost"] = 0;
			basicSkin[index]["colors"][0][0] = 241;
			basicSkin[index]["colors"][0][1] = 148;
			basicSkin[index]["colors"][0][2] = 12;
			index++;

			basicSkin[index]["name"] = "Buru";
			basicSkin[index]["folderPath"] = "Basic Color";
			basicSkin[index]["cost"] = 0;
			basicSkin[index]["colors"][0][0] = 0;
			basicSkin[index]["colors"][0][1] = 172;
			basicSkin[index]["colors"][0][2] = 255;
			index++;

			basicSkin[index]["name"] = "Gurin";
			basicSkin[index]["folderPath"] = "Basic Color";
			basicSkin[index]["cost"] = 0;
			basicSkin[index]["colors"][0][0] = 39;
			basicSkin[index]["colors"][0][1] = 196;
			basicSkin[index]["colors"][0][2] = 52;
			index++;

			basicSkin[index]["name"] = "Pinku";
			basicSkin[index]["folderPath"] = "Basic Color";
			basicSkin[index]["cost"] = 0;
			basicSkin[index]["colors"][0][0] = 244;
			basicSkin[index]["colors"][0][1] = 95;
			basicSkin[index]["colors"][0][2] = 214;
			index++;

			return basicSkin;
		}

		Json::Value cleanSkin(Json::Value basicSkin) {
			Json::Value skins;
			int index = 0;
			for (Json::Value::ArrayIndex id = 0; id != basicSkin.size(); id++) {
				if (basicSkin[id].isObject()) {
					for (auto const& keyname : basicSkin[id].getMemberNames()) {
						if (keyname != "folderPath") {
							skins[index][keyname] = basicSkin[id][keyname];
						}
					}
					index++;
				}
			}
			return skins;
		}

		bool writeSkin(Json::Value basicSkin) {
			try {
				for (Json::Value::ArrayIndex id = 0; id != basicSkin.size(); id++) {
					Json::Value skin;
					if (basicSkin[id].isObject()) {
						for (auto const& keyname : basicSkin[id].getMemberNames()) {
							if (keyname != "folderPath") {
								skin[keyname] = basicSkin[id][keyname];
							}
						}
						Json::FastWriter fastWriter;
						std::string skinJson = fastWriter.write(skin);
						std::string path = folder + "/" + basicSkin[id]["folderPath"].asString() + "/" + skin["name"].asString() + ".skin";
						std::string pathdirectory = folder + "/" + basicSkin[id]["folderPath"].asString();
						if (dirCreate(pathdirectory)) {
							std::ofstream configfile;
							configfile.open(path);
							configfile << skin;
							configfile.close();
						}
					}
				}
				return true;
			}
			catch (...) {
				return false;
			}
		}

		void listFiles(const std::string &path, Json::Value *skins, int* skinLoaded, int* skinLoadedTotal, Json::Value *folderdata, std::function<void(const std::string &, Json::Value *, int *, int *)> cb) {
			if (auto dir = opendir(path.c_str())) {
				while (auto f = readdir(dir)) {
					if (!f->d_name || f->d_name[0] == '.') continue;
					if (f->d_type == DT_DIR) {
						(*folderdata)[f->d_name]["start"] = *skinLoaded;
						(*folderdata)[f->d_name]["end"] = *skinLoaded;
						listFiles(path + f->d_name + "/", skins, skinLoaded, skinLoadedTotal, &(*folderdata)[f->d_name], cb);
					}
					if (f->d_type == DT_REG) {
						cb(path + f->d_name, skins, skinLoaded, skinLoadedTotal);
						(*folderdata)["end"] = (*folderdata)["end"].asInt() + 1;
					}
				}
				closedir(dir);
			}
		}

		Json::Value getAllSkins() {
			Json::Value skins;
			int skinLoaded = 0;
			int skinLoadedTotal = 0;
			listFiles(folder + "/", &skins, &skinLoaded, &skinLoadedTotal, &dataFolder, [](const std::string &path, Json::Value *skins, int *skinLoaded, int *skinLoadedTotal) {
				try {
					if (path.substr(path.find_last_of(".") + 1) == "skin") {
						std::ifstream ifs(path);
						std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
						Json::Value root;
						Json::Reader reader;
						bool parsingSuccessful = reader.parse(content, root);
						if (parsingSuccessful) {
							(*skins)[*skinLoaded] = root;
							(*skinLoaded)++;
						}
						else {
							console cons;
							cons.warn(std::cout) << "Unable to parse " << "'" << termcolor::on_red << path << termcolor::reset << "'" << std::endl;
						}
						(*skinLoadedTotal)++;
					}
				}
				catch (...) {}
			});

			if (skinLoaded == skinLoadedTotal) {
				m_console.game(std::cout) << skinLoaded << (skinLoaded > 1 ? " Skins loaded!" : " Skin loaded!") << std::endl;
			}
			else {
				m_console.game(std::cout) << skinLoaded << "/" << skinLoadedTotal << (skinLoaded > 1 ? " Skins loaded!" : " Skin loaded!") << std::endl;
			}
			

			if (!skinLoaded) {
				m_console.warn(std::cout) << "No loaded Skin... Only Basic Skin Color loaded!" << std::endl;
			}

			return skins;
		}

		Json::Value getDefaultFoodSkin()
		{
			Json::Value data;
			int index = 0;

			Json::Value color;
			color[index][0] = 255;
			color[index][1] = 0;
			color[index][2] = 0;
			index++;
			color[index][0] = 0;
			color[index][1] = 255;
			color[index][2] = 0;
			index++;
			color[index][0] = 0;
			color[index][1] = 0;
			color[index][2] = 255;
			index++;

			int idata = 0;
			for (Json::Value::ArrayIndex id = 0; id != color.size(); id++) {
				data[idata]["stroke"]["active"] = true;
				data[idata]["stroke"]["lineWidth"] = 2;
				data[idata]["stroke"]["color"][0] = color[id][0].asInt();
				data[idata]["stroke"]["color"][1] = color[id][1].asInt();
				data[idata]["stroke"]["color"][2] = color[id][2].asInt();
				data[idata]["stroke"]["color"][3] = 1;

				data[idata]["fill"]["active"] = false;
				data[idata]["fill"]["color"] = "transparent";

				data[idata]["shadow"]["active"] = true;
				data[idata]["shadow"]["color"] = "%stroke.color";
				data[idata]["shadow"]["blur"] = 50;
				data[idata]["shadow"]["offset"]["x"] = 0;
				data[idata]["shadow"]["offset"]["y"] = 0;

				data[idata]["size"]["min"] = 5;
				data[idata]["size"]["max"] = 10;

				data[idata]["proportion"] = 1;

				data[idata]["form"] = 0;

				data[idata]["attracted"]["active"] = true;
				data[idata]["attracted"]["speed"] = 5;
				data[idata]["attracted"]["coefficient"] = 4;

				data[idata]["mass"]["value"] = "%size.value";
				data[idata]["mass"]["grow"]["active"] = true;
				data[idata]["mass"]["grow"]["increment"] = true;
				data[idata]["mass"]["grow"]["end"] = "%mass.value";
				data[idata]["mass"]["grow"]["start"] = 1;
				data[idata]["mass"]["grow"]["timeout"] = 40;

				data[idata]["timeout"] = -1;
				data[idata]["timeoutHebyFood"] = 20000;

				idata++;
			}

			return data;
		}

		Json::Value useDefaultFoodSkins(std::string skinFile) {
			Json::Value data = getDefaultFoodSkin();

			std::ofstream configfile;
			configfile.open(skinFile);
			configfile << data;
			configfile.close();

			return data;
		}

		Json::Value extractFoodSkins(std::string path) {
			Json::Value data;
			if (path.substr(path.find_last_of(".") + 1) == "skin") {
				std::ifstream ifs(path);
				std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
				Json::Value root;
				Json::Reader reader;
				bool parsingSuccessful = reader.parse(content, root);
				if (parsingSuccessful) {
					data = root;
				} else {
					m_console.warn(std::cout) << "Food Skins file : Bad JSON... Default Skins is used" << std::endl;
					data = getDefaultFoodSkin();
				}
			} else {
				m_console.warn(std::cout) << "Food Skins file : MUST HAVE the extension \".skin\"... Default Skins is used" << std::endl;
				data = getDefaultFoodSkin();
			}
			return data;
		}

		Json::Value getFoodSkin() {
			Json::Value data;
			try {
				if (file_exists(config.foodSkin)) {
					data = extractFoodSkins(config.foodSkin);
				} else {
					data = useDefaultFoodSkins(config.foodSkin);
					m_console.info(std::cout) << "Food Skins file generated" << std::endl;
				}
				m_console.game(std::cout) << data.size() << (data.size() > 1 ? " Food Skins loaded!" : " Food Skin loaded!") << std::endl;
			}
			catch (...) {
				data = getDefaultFoodSkin();
				m_console.warn(std::cout) << "Error when loading Food Skins... Default Skins is used" << std::endl;
			}
			
			double_t total_proportion = 0;
			for (Json::Value::ArrayIndex id = 0; id != data.size(); id++) {
				if (data[id].isObject()) {
					if (data[id].isMember("proportion")) {
						total_proportion += data[id].get("proportion", 1.0).asDouble();
					} else {
						data[id]["proportion"] = 1.0;
						total_proportion += 1.0;
					}
				}
			}
	
			// normalize proportion
			int index = 0;
			for (Json::Value::ArrayIndex id = 0; id != data.size(); id++) {
				if (data[id].isObject()) {
					data[id]["proportion"] = data[id]["proportion"].asDouble() / total_proportion;
					for (int j = 0; j < data[id]["proportion"].asDouble() * 100; j++) {
						food_proportions[index] = id;
						index++;
					}
				}
			}

			return data;
		}

		std::string folder;
		console m_console;
		config config;

	};


} // namespace heby


#endif //HEBY_SKINMANAGER_HPP