#pragma once
#include <string>

namespace client {
	namespace communication {
		void handler_loop();

		void send_image(std::string file_path, std::string message, std::string webhook = "");

		void send_message(std::string message, std::string webhook = "");
	}
}