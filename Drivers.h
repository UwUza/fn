#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>

uintptr_t virtualaddy;
uintptr_t cr3;

#define base_adress_code CTL_CODE ( FILE_DEVICE_UNKNOWN, 0x6B1, METHOD_BUFFERED, FILE_SPECIAL_ACCESS )
#define read_memory_code CTL_CODE ( FILE_DEVICE_UNKNOWN, 0x6B2, METHOD_BUFFERED, FILE_SPECIAL_ACCESS )
#define dir_adress_code CTL_CODE ( FILE_DEVICE_UNKNOWN, 0x6B3, METHOD_BUFFERED, FILE_SPECIAL_ACCESS )



typedef struct _BASE_STRUC {
	INT32 security;
	INT32 process_id;
	ULONGLONG* address;
} base, * process_base;

typedef struct _RW_STRUC {
	INT32 security;
	INT32 process_id;
	ULONGLONG address, buffer, size;
	BOOLEAN write;
} readm, * process_read;

typedef struct _DIR_STRUC {
	INT32 process_id;
	bool* operation;
} dir, * process_dir;

namespace mem {
	HANDLE driver_handle;
	INT32 process_id;

	bool find_driver() {
		driver_handle = CreateFileW(L"\\\\.\\\{8ce632b2-d595-4987-b6a8-11d93ecec136}", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); //old name staydetected

		if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
			return false;

		return true;
	}

	void read_physical(PVOID address, PVOID buffer, DWORD size) {
		_RW_STRUC arguments = { 0 };

		arguments.security = 0;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = FALSE;

		DeviceIoControl(driver_handle, read_memory_code, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	void write_physical(PVOID address, PVOID buffer, DWORD size) {
		_RW_STRUC arguments = { 0 };

		arguments.security = 0;
		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = process_id;
		arguments.write = TRUE;

		DeviceIoControl(driver_handle, read_memory_code, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	inline bool GetDir() {

		bool ret = false;
		_DIR_STRUC arguments = { 0 };
		arguments.process_id = process_id;
		arguments.operation = (bool*)&ret;
		DeviceIoControl(driver_handle, dir_adress_code, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
		return ret;
	}

	uintptr_t find_image() {
		uintptr_t image_address = { NULL };
		_BASE_STRUC arguments = { NULL };

		arguments.security = 0;
		arguments.process_id = process_id;
		arguments.address = (ULONGLONG*)&image_address;

		DeviceIoControl(driver_handle, base_adress_code, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return image_address;
	}


	INT32 find_process(LPCTSTR process_name) {
		PROCESSENTRY32 pt;
		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt)) {
			do {
				if (!lstrcmpi(pt.szExeFile, process_name)) {
					CloseHandle(hsnap);
					process_id = pt.th32ProcessID;
					return pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		}
		CloseHandle(hsnap);

		return { NULL };
	}
}

template <typename T>
T read(uint64_t address) {
	T buffer{ };
	mem::read_physical((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

template <typename T>
T write(uint64_t address, T buffer) {

	mem::write_physical((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

bool is_valid(const uint64_t adress)
{
	if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress >
		0x7FFFFFFFFFFFFFFF) {
		return false;
	}
	return true;
}

template <typename T>
std::vector<T> batch_read(const std::vector<uint64_t>& addresses) {
	size_t num_addresses = addresses.size();
	std::vector<T> results(num_addresses);
	std::vector<uint8_t> buffer(num_addresses * sizeof(T));

	for (size_t i = 0; i < num_addresses; ++i) {
		mem::read_physical((PVOID)addresses[i], buffer.data() + i * sizeof(T), sizeof(T));
	}

	for (size_t i = 0; i < num_addresses; ++i) {
		results[i] = *reinterpret_cast<T*>(buffer.data() + i * sizeof(T));
	}

	return results;
}
