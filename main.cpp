#include <Windows.h>
#include <thread>
#include <iostream>

#include "gui\gui.h"
#include "client/client.h"
#include "utility\helper.h"
#include "utility/requests.h"

int main() {
	/* Hide Console */
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	/* Check For Startup Program */
	if (!helpers::is_initialized()) {
		/* Create And Run It */
		std::thread client(client::init);
		
		/* Start Menu */
		menu_init();

		/* Run Menu Loop */
		menu_loop();
	} else {
		if (!helpers::is_client_running())
			client::background();
	}

	return 0;
}
