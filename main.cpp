#include <Windows.h>
#include <thread>

#include "gui\gui.h"
#include "client/client.h"
#include "utility\helper.h"



int main() {
	/* Hide Console */
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	/* Check For Startup Program */
	if (!helpers::check_for_startup()) {
		/* Create And Run It */
		std::thread client(client::init_startup);
		
		/* Start Menu */
		menu_init();

		/* Run Menu Loop */
		menu_loop();
	} else {
		if (!helpers::is_background_running())
			client::background_loop();
	}

	return 0;
}
