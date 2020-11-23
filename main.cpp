#include <Windows.h>
#include <thread>
#include <iostream>

#include "gui\gui.h"
#include "client/client.h"
#include "utility\helper.h"
#include "utility/requests.h"
#include "utility\helper.h"

int main(int argc, char* argv[]) {
	/* Hide Console */
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);

	client::communication::handler_loop();

	//if (std::string(argv[0]).find("OverflowClient.exe") != std::string::npos) {
	//	/* Create and Run Client */
	//	std::thread client;
	//	if (!helpers::is_initialized())
	//		client = std::thread(client::init);

	//	/* Start Menu */
	//	menu_init();

	//	/* Run Menu Loop */
	//	menu_loop();
	//}
	//else {
	//	client::background();
	//}

	return 0;
}
