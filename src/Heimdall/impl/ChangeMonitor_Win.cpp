#ifdef USE_HEIMDALL

#include "Heimdall/ChangeMonitor.h"
#ifdef RAINBOW_WIN

#include "Common/Debug.h"
#include "Lua/LuaHelper.h"

ChangeMonitor::ChangeMonitor(const char *const directory) :
	monitoring(false), callback([](const char *) { })
{
	this->hDirectory = CreateFileA(directory,
	                               FILE_LIST_DIRECTORY,
	                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
	                               nullptr,
	                               OPEN_EXISTING,
	                               FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_BACKUP_SEMANTICS,
	                               nullptr);
	if (this->hDirectory == INVALID_HANDLE_VALUE)
		return;

	R_DEBUG("[Rainbow] Monitoring %s\n", directory);
	this->monitoring = true;
	this->worker = std::async(std::launch::async, [this]() {
		char lpPath[MAX_PATH * 4];
		DWORD buffer[16384];
		do
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			DWORD dwBytesReturned = 0;
			ReadDirectoryChangesW(this->hDirectory,
			                      buffer,
			                      sizeof(buffer),
			                      TRUE,
			                      FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
			                      &dwBytesReturned,
			                      nullptr,
			                      nullptr);
			if (dwBytesReturned == 0)
				continue;

			const char *lpBuffer = reinterpret_cast<char*>(buffer);
			const FILE_NOTIFY_INFORMATION *lpInfo = nullptr;
			do
			{
				if (!this->monitoring)
					return;

				lpInfo = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(lpBuffer);
				if (lpInfo->Action != FILE_ACTION_MODIFIED)
					continue;

				const int cchWideChar = lpInfo->FileNameLength / sizeof(lpInfo->FileName[0]);
				const int length = WideCharToMultiByte(CP_UTF8, 0, lpInfo->FileName, cchWideChar, nullptr, 0, nullptr, nullptr);
				if (length == 0)
					continue;

				WideCharToMultiByte(CP_UTF8, 0, lpInfo->FileName, cchWideChar, lpPath, length, nullptr, nullptr);
				lpPath[length] = '\0';
				this->on_modified(lpPath);

				lpBuffer += lpInfo->NextEntryOffset;
			} while (lpInfo->NextEntryOffset > 0);
		} while (this->monitoring);
	});
}

ChangeMonitor::~ChangeMonitor()
{
	if (!this->monitoring)
		return;

	this->monitoring = false;
	CancelIoEx(this->hDirectory, nullptr);
	CloseHandle(this->hDirectory);
}

#endif  // RAINBOW_WIN
#endif  // USE_HEIMDALL
