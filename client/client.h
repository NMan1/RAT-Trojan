#pragma once

#define VERSION "1.1"

namespace client {
	void start_client();

	void background_loop();

	bool check_for_startup();

	extern bool run_background;
}