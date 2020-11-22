#pragma once
#include <string>

#define VERSION "1.2"

namespace client {
	extern const std::string STARTUP_FILE_NAME;

	extern const std::string PROGRAM_NAME;

	void init_startup();

	void start_client();

	void background_loop();

	extern bool run_background;
}