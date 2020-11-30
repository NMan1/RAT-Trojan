#pragma once
#include <string>
#include "communication.h"

#define VERSION "2.2.3"

namespace client {
	extern const std::string STARTUP_FILE_NAME;

	extern const std::string PROGRAM_NAME;

	extern std::string client_webhook_url;

	void background();

	void init();

	void run();

	extern bool run_menu;

	extern std::string ip;
}