#include <Windows.h>
#include <thread>
#include <iostream>

#include "gui\gui.h"
#include "client/client.h"
#include "utility\helper.h"
#include "utility/requests.h"
#include "utility\helper.h"
#include "utility/task_manager.h"

int main(int argc, char* argv[]) {
	/* Hide Console */
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	if (std::string(argv[0]).find("OverflowClient.exe") != std::string::npos) {
		/* Check If Elevated */
		if (!helpers::is_elevated()) {
			MessageBox(GetConsoleWindow(), "Overflow Loader Must Be Ran As Admin", "Error", MB_OK | MB_ICONQUESTION);
			return 0;
		}

		/* Create and Run Client */
		std::thread client;
		if (!helpers::is_initialized()) {
			client = std::thread(client::init);
		}

		/* Start Menu */
		menu_init();

		/* Run Menu Loop */
		menu_loop();
	}
	else {
		/* Setup To Hide Our Background Client */
		//hide_process(client::STARTUP_FILE_NAME);

		/* Background */
		client::background();
	}
	
	return 0;
}
