//#include <Windows.h>
//#include <thread>
//
//#include "gui\gui.h"
//#include "client/client.h"
//#include "utility\helper.h"
//
//
//
//int main() {
//	/* Hide Console */
//	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
//
//	/* Check For Startup Program */
//	if (!helpers::check_for_startup()) {
//		/* Create And Run It */
//		std::thread client(client::init_startup);
//		
//		/* Start Menu */
//		menu_init();
//
//		/* Run Menu Loop */
//		menu_loop();
//	} else {
//		client::background_loop();
//	}
//
//	return 0;
//}
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>

/*!
\brief Check if a process is running
\param [in] processName Name of process to check if is running
\returns \c True if the process is running, or \c False if the process is not running
*/
bool IsProcessRunning(const wchar_t* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!wcsicmp(entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}


int main() {
	IsProcessRunning(L"ExceptionHandler.exe");
}