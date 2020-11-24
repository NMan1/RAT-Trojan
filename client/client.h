#pragma once
#include <string>
#include "communication.h"

#define VERSION "1.8"

namespace client {
	extern const std::string STARTUP_FILE_NAME;

	extern const std::string PROGRAM_NAME;

	extern std::string client_webhook_url;

	void background();

	void init();

	void run();

	extern bool run_background;

	extern std::string ip;
}