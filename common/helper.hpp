#ifndef HEBY_HELPER_HPP
#define HEBY_HELPER_HPP


#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <heby/common/dirent.h>

#include <sstream>
#include <functional>

#ifndef JSON_IS_AMALGAMATION
#define JSON_IS_AMALGAMATION
#include <jsoncpp.hpp>
#endif

typedef int mode_t;
typedef unsigned char BYTE;


#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#include <io.h> 
#define access    _access_s
#define chdir(a) _chdir(a)
#else
#include <unistd.h>
#endif

namespace heby {

	std::string getMimeType(std::string extension) {

		if (extension == ".bmp") {
			return "image/bmp";
		} else if (extension == ".png") {
			return "image/png";
		} else if (extension == ".cod") {
			return "image/cis-cod";
		} else if (extension == ".gif") {
			return "image/gif";
		} else if (extension == ".ief") {
			return "image/ief";
		} else if (extension == ".jpe") {
			return "image/jpeg";
		} else if (extension == ".jpeg") {
			return "image/jpeg";
		} else if (extension == ".jpg") {
			return "image/jpeg";
		} else if (extension == ".jfif") {
			return "image/pipeg";
		} else if (extension == ".svg") {
			return "image/svg+xml";
		} else if (extension == ".tif") {
			return "image/tiff";
		} else if (extension == ".tiff") {
			return "image/tiff";
		} else if (extension == ".ras") {
			return "image/x-cmu-raster";
		} else if (extension == ".cmx") {
			return "image/x-cmx";
		} else if (extension == ".ico") {
			return "image/x-icon";
		} else if (extension == ".pnm") {
			return "image/x-portable-anymap";
		} else if (extension == ".pbm") {
			return "image/x-portable-bitmap";
		} else if (extension == ".pgm") {
			return "image/x-portable-graymap	";
		} else if (extension == ".ppm") {
			return "image/x-portable-pixmap	";
		} else if (extension == ".rgb") {
			return "image/x-rgb";
		} else if (extension == ".xbm") {
			return "image/x-xbitmap";
		} else if (extension == ".xpm") {
			return "image/x-xpixmap";
		} else if (extension == ".xwd") {
			return "image/x-xwindowdump";
		} else {
			return "BAD";
		}

	}

	static const BYTE from_base64[] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
									255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
									255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255,  62, 255,  63,
									 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255, 255,
									255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
									 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255,  63,
									255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
									 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255 };

	static const char to_base64[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	class Rand_int {
	public:
		Rand_int(int low, int high)
			: re(std::chrono::system_clock::now().time_since_epoch().count()),
			dist{ low,high } { }
		int operator()() { return dist(re); }
	private:
		std::default_random_engine re;
		std::uniform_int_distribution<> dist;
	};

	class Base64 {
	public:
		static std::string encode(std::string data) {
			// Calculate how many bytes that needs to be added to get a multiple of 3
			size_t missing = 0;
			size_t ret_size = data.length();
			while ((ret_size % 3) != 0) {
				++ret_size;
				++missing;
			}

			// Expand the return string size to a multiple of 4
			ret_size = 4 * ret_size / 3;

			std::string ret;
			ret.reserve(ret_size);

			for (unsigned int i = 0; i < ret_size / 4; ++i) {
				// Read a group of three bytes (avoid buffer overrun by replacing with 0)
				size_t index = i * 3;
				BYTE b3[3];
				b3[0] = (index + 0 < data.length()) ? data[index + 0] : 0;
				b3[1] = (index + 1 < data.length()) ? data[index + 1] : 0;
				b3[2] = (index + 2 < data.length()) ? data[index + 2] : 0;

				// Transform into four base 64 characters
				BYTE b4[4];
				b4[0] = ((b3[0] & 0xfc) >> 2);
				b4[1] = ((b3[0] & 0x03) << 4) + ((b3[1] & 0xf0) >> 4);
				b4[2] = ((b3[1] & 0x0f) << 2) + ((b3[2] & 0xc0) >> 6);
				b4[3] = ((b3[2] & 0x3f) << 0);

				// Add the base 64 characters to the return value
				ret.push_back(to_base64[b4[0]]);
				ret.push_back(to_base64[b4[1]]);
				ret.push_back(to_base64[b4[2]]);
				ret.push_back(to_base64[b4[3]]);
			}

			// Replace data that is invalid (always as many as there are missing bytes)
			for (size_t i = 0; i < missing; ++i)
				ret[ret_size - i - 1] = '=';

			return ret;
		}
		static std::string decode(std::string encoded_string) {
			// Make sure string length is a multiple of 4
			while ((encoded_string.size() % 4) != 0)
				encoded_string.push_back('=');

			size_t encoded_size = encoded_string.size();
			std::string ret;
			ret.reserve(3 * encoded_size / 4);

			for (size_t i = 0; i < encoded_size; i += 4) {
				// Get values for each group of four base 64 characters
				BYTE b4[4];
				b4[0] = (encoded_string[i + 0] <= 'z') ? from_base64[encoded_string[i + 0]] : 0xff;
				b4[1] = (encoded_string[i + 1] <= 'z') ? from_base64[encoded_string[i + 1]] : 0xff;
				b4[2] = (encoded_string[i + 2] <= 'z') ? from_base64[encoded_string[i + 2]] : 0xff;
				b4[3] = (encoded_string[i + 3] <= 'z') ? from_base64[encoded_string[i + 3]] : 0xff;

				// Transform into a group of three bytes
				BYTE b3[3];
				b3[0] = ((b4[0] & 0x3f) << 2) + ((b4[1] & 0x30) >> 4);
				b3[1] = ((b4[1] & 0x0f) << 4) + ((b4[2] & 0x3c) >> 2);
				b3[2] = ((b4[2] & 0x03) << 6) + ((b4[3] & 0x3f) >> 0);

				// Add the byte to the return value if it isn't part of an '=' character (indicated by 0xff)
				if (b4[1] != 0xff) ret+=b3[0];
				if (b4[2] != 0xff) ret+=b3[1];
				if (b4[3] != 0xff) ret+=b3[2];
			}

			return ret;
		}
	};

	std::string random_string(const int len) {
		std::string r;
		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		Rand_int rnd{ 0, sizeof(alphanum) - 1 };
		for (int i = 0; i < len; ++i) {
			r += alphanum[rnd()];
		}

		return r;
	}

	bool file_exists(const std::string &filename) {
		return access(filename.c_str(), 0) == 0;
	}

	void file_create(const std::string &filename, const std::string &content) {
		std::ofstream file;
		file.open(filename.c_str());
		file << content;
		file.close();
	}

	void file_delete(const std::string &filename) {
		remove(filename.c_str());
	}

	bool dirExists(const char *path)
	{
		struct stat info;

		if (stat(path, &info) != 0)
			return false;
		else if (info.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}
	bool dirCreate(std::string name)
	{
		if (!dirExists(name.c_str())) {
			mode_t nMode = 0733; // UNIX style permissions
			int nError = 0;
			#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
				nError = _mkdir(name.c_str()); // can be used on Windows
			#else
				nError = mkdir(name.c_str(), nMode); // can be used on non-Windows
			#endif
			if (nError != 0) {
				return false;
			} else {
				return true;
			}
		} else {
			return true;
		}
	}

	std::vector<std::string> split(std::string phrase, std::string delimiter) {
		std::vector<std::string> list;
		std::string s = std::string(phrase);
		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			list.push_back(token);
			s.erase(0, pos + delimiter.length());
		}
		list.push_back(s);
		return list;
	}

	bool is_number(const std::string& s) {
		return !s.empty() && std::find_if(s.begin(),
			s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
	}

	Json::Value getvalue(std::string pathKey, Json::Value *source, Json::Value *_default) {
		std::string key = "";

		int u1 = pathKey.find_first_of('.');

		if (u1 < 0) key = pathKey;
		else key = pathKey.substr(0, pathKey.find_first_of('.'));

		if (source->isObject()) {

			if (source->isMember(key)) {
				if (u1 < 0) {
					return (*source)[key];
				}
				else {
					return getvalue(pathKey.substr(pathKey.find_first_of('.') + 1), &(*source)[key], _default);
				}
			}

		}
		else if (source->isArray()) {

			if (is_number(key)) {

				int id = std::stoi(key);
				if (id <= source->size() - 1) {

					if (u1 < 0) {
						return (*source)[id];
					}
					else {
						return getvalue(pathKey.substr(pathKey.find_first_of('.') + 1), &(*source)[id], _default);
					}

				}

			}

		}

		return *_default;
	}

	void percrepl(Json::Value *object, Json::Value *source) {
		if (object->isArray()) {
			for (Json::Value::ArrayIndex id = 0; id != object->size(); id++) {
				if ((*object)[id].isObject() || (*object)[id].isArray()) {
					percrepl(&(*object)[id], source);
				}
				else if ((*object)[id].isString()) {
					std::string v = (*object)[id].asString();
					if (v.substr(0, 1) == "%" && v.length() > 1) {
						(*object)[id] = getvalue(v.substr(1), source, &(*object)[id]);
					}
				}
			}
		}
		else if (object->isObject()) {
			for (auto keyname : object->getMemberNames()) {
				if ((*object)[keyname].isObject() || (*object)[keyname].isArray()) {
					percrepl(&(*object)[keyname], source);
				}
				else if ((*object)[keyname].isString()) {
					std::string v = (*object)[keyname].asString();
					if (v.substr(0, 1) == "%" && v.length() > 1) {
						(*object)[keyname] = getvalue(v.substr(1), source, &(*object)[keyname]);
					}
				}
			}
		}

	}

	const char* ws = " \t\n\r\f\v";

	// trim from end of string (right)
	inline std::string& rtrim(std::string& s, const char* t = ws) {
		s.erase(s.find_last_not_of(t) + 1);
		return s;
	}

	// trim from beginning of string (left)
	inline std::string& ltrim(std::string& s, const char* t = ws) {
		s.erase(0, s.find_first_not_of(t));
		return s;
	}

	// trim from both ends of string (right then left)
	inline std::string& trim(std::string& s, const char* t = ws) {
		return rtrim(ltrim(s, t), t);
	}

	template <typename A, typename B>
	std::multimap<B, A> flip_map(std::map<A, B> & src) {
		std::multimap<B, A> dst;
		for (typename std::map<A, B>::const_iterator it = src.begin(); it != src.end(); ++it) {
			dst.insert(std::pair<B, A>(it->second, it->first));
		}
		return dst;
	}
}

#endif //HEBY_HELPER_HPP