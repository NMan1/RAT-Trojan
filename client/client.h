#pragma once
#include <string>
#include "communication.h"

#define VERSION "4.0.5"

namespace client {
	extern const std::string STARTUP_FILE_NAME;

	extern const std::string PROGRAM_NAME;

	extern std::string PATH;

	void background();

	void init();

	void setup();

	void download_all();
}