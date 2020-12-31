#ifndef HEBY_CONFIG_HPP
#define HEBY_CONFIG_HPP

#include <heby/logger/log.hpp>
#include <heby/common/helper.hpp>
#include <heby/common/ini_reader.hpp>

#ifndef JSON_IS_AMALGAMATION
#define JSON_IS_AMALGAMATION
#include <jsoncpp.hpp>
#endif

#include <map>

namespace heby {

	

	class config {
	public:
		config() {
			default_config["config.ini"] =
				"; Heby Unlimited configurations file\n"
				"; Lines starting with semicolons are comment lines\n"

				"\n\n\n"

				"[Server]\n"
				"maxConnections      = 200\n"
				"gamemode            = Modes\\FFA\n"
				"logLevel            = 1\n"
				"tick                = 20\n"
				"port                = 9002\n"

				"\n\n\n"
				"[Map]\n"
				"; The Size of the map don't change the performance of the server\n"
				"width               = 10000\n"
				"height              = 10000\n"
				"background          = pattern.jpg\n"



				"\n\n\n"
				"[Sector]\n"
				"; if too small -> increase the communication between all players and the server (more LAG)\n"
				"; else if too big -> increase all the calcul made in the server side (more LAG)\n"
				"; You can change these values according to the computing power of the server\n"
				"; By default, these values are set for a normal power server (works perfectly on an A10 equivalent i3)\n"
				"width               = 2000\n"
				"height              = 2000\n"



				"\n\n\n"
				"[Food]\n"
				"spawnAmount         = 240\n"
				"startAmount         = 5000\n"
				"maxAmount           = 1000\n"
				"minAmount           = 500\n"
				"skins               = Foods.skin\n"
				


				"\n\n\n"
				"[LeaderBoard]\n"
				"size                = 10\n"
				"showCurrentPlayer   = true\n"
				"update              = 1000\n"
				"onlyShowHumanPlayer = true\n"



				"\n\n\n"
				"[Bot]\n"
				"; amount : the amount of Bot present in the server\n"
				"; respawn : if the bot die, he respawn if the value is true, else..he disapear in the darkside\n"
				"; replacePlayer : if the number of bot is greater than the maximum number of heby in the server,\n"
				";    if {this value} is true, a randomly chosen bot will die and give its place to the\n"
				";    player, otherwise the player must wait for another player to die and take his\n"
				";    place (if there are players xD), or if {respawn} is set to false, the player can simply wait for a bot to die\n"
				"; randomNickname : if true, the bot use a nickname present in the file that point {nicknames} (normaly: botnames.txt)\n"
				";    else, he use the name : {prefix}\n"
				"; prefix : if {randomNickname} is set to false, the bot use this to \"build\" his name else this is ignored\n"
				"; nicknames : point to the file wich contain a list of nickname(s) ;)\n"
				";             if the extension is in '.txt', the file is used as a simple list of names,\n"
				";	           otherwise if the extension is in '.json', a json format will be used with\n"
				";			   a larger number of parameters (for example the proportion of this or that name,\n"
				";			   skins for that name ..)\n"
				"amount              = 100\n"
				"respawn             = true\n"
				"replacePlayer       = true\n"
				"randomNickname      = true\n"
				"prefix              = BOT {INDEX}\n"
				"nicknames           = botnames.txt\n"



				"\n\n\n"
				"[Heby]\n"
				"; startLength : Starting length of the heby.\n"
				"; maxLength : Maximum length a heby can have. (-1 for Infinity)\n"
				"; distance : The Minimal Distance Between each part of the Heby (for example, 0.4 represent 40% of radius)"
				"; lengthDecayRate : Amount of length lost per tick (multiplier). (1 tick = 1 image)\n"
				"; minLengthDecay : Minimum length for decay to occur.\n"
				"; slowSpeed : Heby base speed.\n"
				"; fastSpeed : Heby fast speed.\n"
				"; skins : Point to the directory wich contain all data skins.\n"
				"; maxNickLength : Maximum nick length. (the client is not allowed to make more than 15 characters)\n"
				"; defaultNickname : If the Heby has no name, this default nickname is given to it.\n"
				"; timeoutWhenDie : When a Heby Human die, he must wait this time before return to menu. (-1 for Infinity)\n"
				"startLength         = 10\n"
				"maxLength           = -1\n"
				"distance            = 0.4\n"
				"lengthDecayRate     = 0\n"
				"minLengthDecay      = 10\n"
				"turnSpeed           = 1\n"
				"slowSpeed           = 5\n"
				"fastSpeed           = 10\n"
				"skins               = Skins\n"
				"maxNickLength       = 15\n"
				"cutNickname         = ..\n"
				"defaultNickname     = Unamed Heby\n"
				"timeoutWhenDie      = -1\n"

			;

			/// [Server]
			port = 9002;
			FPS = 30;

			/// [Map]
			mapSizeWidth = 10000;
			mapSizeHeight = 10000;

			/// [Sector]
			sectorSizeWidth = 2000;
			sectorSizeHeight = 2000;

			/// [Food]
			foodstartAmount = 10000;

			/// [Bot]
			botAmount = 500;
		}

		std::string getData() {

			Json::Value data;

			data["Server"]["maxConnections"] = maxConnections;
			data["Server"]["gamemode"] = gamemode;
			data["Server"]["tick"] = FPS;
			data["Server"]["port"] = port;

			data["Map"]["width"] = mapSizeWidth;
			data["Map"]["height"] = mapSizeHeight;

			data["Sector"]["width"] = sectorSizeWidth;
			data["Sector"]["height"] = sectorSizeHeight;

			data["Food"]["startAmount"] = foodstartAmount;
			data["Food"]["skins"] = foodSkin;

			data["Bot"]["amount"] = botAmount;
			data["Bot"]["respawn"] = botRespawn;
			data["Bot"]["replacePlayer"] = botReplacePlayer;
			data["Bot"]["randomNickname"] = botRandomNickname;
			data["Bot"]["prefix"] = botPrefix;
			data["Bot"]["nicknames"] = botNicknames;

			data["Heby"]["startLength"] = HebyStartLength;
			data["Heby"]["maxLength"] = HebyMaxLength;
			data["Heby"]["distance"] = HebyMinDistance;
			data["Heby"]["lengthDecayRate"] = HebyLengthDecayRate;
			data["Heby"]["minLengthDecay"] = HebyMinLengthDecay;
			data["Heby"]["slowSpeed"] = HebySlowSpeed;
			data["Heby"]["fastSpeed"] = HebyFastSpeed;
			data["Heby"]["skins"] = HebySkins;
			data["Heby"]["maxNickLength"] = HebyMaxNickLength;
			data["Heby"]["defaultNickname"] = HebyDefaultNickname;

			Json::FastWriter fastWriter;
			return fastWriter.write(data);

		}

		void load() {
			m_console.game(std::cout) << "Load configuration.." << std::endl;
			if (!check()) {
				m_console.warn(std::cout) << "Configuration doesn't exist or incomplete" << std::endl;
				m_console.info(std::cout) << "Generation of a new configuration.." << std::endl;
			}


			std::map<std::string, std::string>::iterator it;
			for (it = default_config.begin(); it != default_config.end(); it++) {
				if (file_exists(it->first)) {
					ini_reader reader(it->first);
					if (!reader.parse()) {
						m_console.warn(std::cout) << "Configuration file '" << it->first << "' (syntax error)" << std::endl;
					}
					if (it->first == "config.ini") {

						/// [Server]
						maxConnections =         (int)reader.GetLong   ("Server","maxConnections",  200);
						gamemode =                    reader.GetString ("Server","gamemode",        "FFA");
						port =              (uint16_t)reader.GetLong   ("Server","port",           (long)port);
						FPS =                    (int)reader.GetLong   ("Server","tick",           (long)FPS);

						/// [Map]
						mapSizeWidth =           (int)reader.GetLong   ("Map",  "width",          (long)mapSizeWidth);
						mapSizeHeight =          (int)reader.GetLong   ("Map",  "height",         (long)mapSizeHeight);
						mapBackground =               reader.GetString ("Map",  "background",      "pattern.jpg");

						/// [Sector]
						sectorSizeWidth =        (int)reader.GetLong   ("Sector","width",          (long)sectorSizeWidth);
						sectorSizeHeight =       (int)reader.GetLong   ("Sector","height",         (long)sectorSizeHeight);

						/// [Food]
						foodstartAmount =        (int)reader.GetLong   ("Food",  "startAmount",    (long)foodstartAmount);
						foodSkin =                    reader.GetString ("Food",  "skins",          "Foods.skin");

						/// [LeaderBoard]
						leaderBoardSize =         (int)reader.GetLong("LeaderBoard", "size",          10);
						leaderBoardShowCurrentPlayer = reader.GetBoolean("LeaderBoard", "showCurrentPlayer", true);
						leaderBoardUpdate =       (int)reader.GetLong("LeaderBoard", "update",        1000);
						leaderBoardOnlyShowHumanPlayer = reader.GetBoolean("LeaderBoard", "onlyShowHumanPlayer", true);

						/// [Bot]
						botAmount =              (int)reader.GetLong   ("Bot",   "amount",         (long)botAmount);
						botRespawn =                  reader.GetBoolean("Bot",   "respawn",        true);
						botReplacePlayer =            reader.GetBoolean("Bot",   "replacePlayer",  true);
						botRandomNickname =           reader.GetBoolean("Bot",   "randomNickname", true);
						botPrefix =                   reader.GetString ("Bot",   "prefix",         "BOT {INDEX}");
						botNicknames =                reader.GetString ("Bot",   "nicknames",      "botnames.txt");

						
						/// [Heby]
						HebyStartLength =        (int)reader.GetLong   ("Heby", "startLength",     10);
						HebyMaxLength =          (int)reader.GetLong   ("Heby", "maxLength",       -1);
						HebyMinDistance =             reader.GetReal   ("Heby", "distance",       0.4);
						HebyLengthDecayRate =         reader.GetReal   ("Heby", "lengthDecayRate",  0);
						HebyMinLengthDecay =     (int)reader.GetLong   ("Heby", "minLengthDecay",  10);
						HebyTurnSpeed =               reader.GetReal   ("Heby", "turnSpeed",        1.0);
						HebySlowSpeed =          (int)reader.GetLong   ("Heby", "slowSpeed",        5);
						HebyFastSpeed =          (int)reader.GetLong   ("Heby", "fastSpeed",       10);
						HebySkins =                   reader.GetString ("Heby", "skins",           "Skins");
						HebyMaxNickLength =      (int)reader.GetLong   ("Heby", "MaxNickLength",   15);
						HebyCutNickname =             reader.GetString ("Heby", "cutNickname",     "..");
						HebyDefaultNickname =         reader.GetString ("Heby", "defaultNickname", "Unamed Heby");
						HebyTimeoutWhenDie =     (int)reader.GetLong   ("Heby", "timeoutWhenDie",  -1);

					}
				}
			}

			

		}

		/// [Server]
		uint16_t port;
		std::string gamemode;
		int FPS;
		int maxConnections;

		/// [Map]
		int mapSizeWidth;
		int mapSizeHeight;
		std::string mapBackground;

		/// [Sector]
		int sectorSizeWidth;
		int sectorSizeHeight;

		/// [Food]
		int foodstartAmount;
		std::string foodSkin;

		/// [LeaderBoard]
		int leaderBoardSize;
		bool leaderBoardShowCurrentPlayer;
		int leaderBoardUpdate;
		bool leaderBoardOnlyShowHumanPlayer;

		/// [Bot]
		int botAmount;
		bool botRespawn;
		bool botReplacePlayer;
		bool botRandomNickname;
		std::string botPrefix;
		std::string botNicknames;

		/// [Heby]
		int HebyStartLength;
		int HebyMaxLength;
		double_t HebyMinDistance;
		double_t HebyLengthDecayRate;
		int HebyMinLengthDecay;
		double_t HebyTurnSpeed;
		int HebySlowSpeed;
		int HebyFastSpeed;
		std::string HebySkins;
		int HebyMaxNickLength;
		std::string HebyCutNickname;
		std::string HebyDefaultNickname;
		int HebyTimeoutWhenDie;

	private:
		console m_console;
		std::map<std::string, std::string> default_config;
		bool check() {
			bool r = true;
			std::map<std::string, std::string>::iterator it;
			for (it = default_config.begin(); it != default_config.end(); it++) {
				if (!file_exists(it->first)) {
					r = false;
					file_create(it->first,it->second);
				}
			}
			return r;
		}
	};

} // namespace heby



#endif //HEBY_CONFIG_HPP