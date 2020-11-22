#include "communication.h"
#include "..\utility\requests.h"

namespace client {
	namespace communication {
		void handler_loop() {
			while (true) {
				auto cmd = requests::get_request("https://overflow.red/post.php", "cmd=get_command");
				if (!cmd.empty()) {	
					char buff[256];
					sprintf_s(buff, "%s", cmd.c_str());
					MessageBox(GetConsoleWindow(), buff, "Sent Command", MB_OK | MB_ICONQUESTION);
				}
			}
		}
	}
}