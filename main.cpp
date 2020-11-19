#include <Windows.h>
#include <thread>

#include "gui\gui.h"
#include "client/client.h"

#pragma comment(lib, "wininet.lib")

int main() {
	/* Hide Console */
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	/* Start Menu */
	menu_init();
	
	/* Create Client Thread */
	//std::thread client(start_client);
	
	/* Run Menu Loop */
	menu_loop();
	
	return 0;
}