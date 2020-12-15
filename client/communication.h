#pragma once
#include <string>

namespace client {
	namespace communication {
		void handler_loop();

		std::string send_message(std::string title, std::string desc = "", std::string file_path = "", std::string type = "");

		void set_profile(std::string key, std::string value);

		std::string get_profile(std::string key);

		std::string get_command();
	}
}