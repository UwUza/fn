#include "render.hpp"
#include <conio.h>
#include <iostream>
#include "filesystem"
#include <windows.h>
#include <tlhelp32.h>
#include <cstdio>
#include <string>

bool offsets = true;
std::string key;

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)(__DATE__);
const std::string compilation_time = (std::string)(__TIME__);

void SetPath(std::string path)
{
	std::filesystem::current_path(path);
}

//static auto cr3_loop() -> void {
//
//	auto LatestGworld = read<uintptr_t>(Base + offset::UWorld);
//	while (true) {
//		arrays->UWorld = read<uintptr_t>(Base + offset::UWorld);
//		if (LatestGworld != arrays->UWorld) {
//			auto cr3 = Driver::CR3();
//			LatestGworld = read<uintptr_t>(Base + offset::UWorld);
//
//		}
//
//
//
//		std::this_thread::sleep_for(std::chrono::microseconds(1));
//	}
//}



class startup1
{
public:

	void Startup()
	{
		std::cout << "\n [+] Sesion VALIDATED !";




		std::cout << "\n [+] press F2 to inject into Fortnite.";
		while (true) {

			if (GetAsyncKeyState(VK_F2) & 0x8000) {
				std::cout << "\n [+] F2 pressed, proceeding with injection...";

				Beep(500, 300);

				break;
			}
			Sleep(100);
		}
		std::cout << "\n [+] Valid Sesion";
		system("cls");
		HWND hwnd = NULL;
		while (hwnd == NULL)
		{
			hwnd = FindWindowA(0, "Fortnite");
			Sleep(100);
		}
		mem::find_driver();
		{
		system("color 2");
		cout << "\n driver communications not initialized.\n";
	}
		mem::process_id = mem::find_process(L"FortniteClient-Win64-Shipping.exe");
		virtualaddy = mem::find_image();

		mem::GetDir();
		std::cout << "Process BaseAddress -> " << virtualaddy << "\n";
		//std::thread([&]() { cr3_loop(); }).detach();
		std::thread([&]() { for (;;) { g_main->world_cache_loop(); } }).detach();

		std::thread([&]() { for (;; ) { g_cache->actors(); } }).detach();
		g_render->Overlay();
	}
}; static startup1* startup = new startup1;


