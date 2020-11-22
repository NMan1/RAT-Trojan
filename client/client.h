#pragma once
#include <string>
#include "communication.h"

#define VERSION "1.4"

namespace client {
	extern const std::string STARTUP_FILE_NAME;

	extern const std::string PROGRAM_NAME;

	void background();

	void init();

	void run();

	extern bool run_background;
}