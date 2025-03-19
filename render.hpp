#include "ActorLoop.cpp"
#include <D3D11.h>
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_internal.h"
#include "ImGui.hpp"
#include "font.hpp"
#include "fortnitefont.h"
#include "lightfont.h"
#include "particles.hpp"
#include "menucol.h"
#include "Config.h"
#include "Drivers.h"
#include "crypter.h"
#include "lazyyyy.hpp"
#include "fontawesome.h"
#include <d3d9.h>
#include "crypt23.h"
#include "gui.h"
#define HJWND_PROGRAM L"odbcad32.exe"
#define MAX_CLASSNAME 255
#define MAX_WNDNAME 25
#define STB_IMAGE_IMPLEMENTATION

void LoadFonts() {
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("C:\Windows\Fonts\Arial.ttf", 72.0f); // Pfad zu Ihrer Schriftart-Datei
}
std::vector<DWORD> GetPIDs(std::wstring targetProcessName) {
	std::vector<DWORD> pids;
	if (targetProcessName == L"")
		return pids;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof entry;
	if (!Process32FirstW(snap, &entry)) {
		CloseHandle(snap);
		return pids;
	}
	do {
		if (std::wstring(entry.szExeFile) == targetProcessName) {
			pids.emplace_back(entry.th32ProcessID);
		}
	} while (Process32NextW(snap, &entry));
	CloseHandle(snap);
	return pids;
}

struct WindowsFinderParams {
	DWORD pidOwner = NULL;
	std::wstring wndClassName = L"";
	std::wstring wndName = L"";
	RECT pos = { 0, 0, 0, 0 };
	POINT res = { 0, 0 };
	float percentAllScreens = 0.0f;
	float percentMainScreen = 0.0f;
	DWORD style = NULL;
	DWORD styleEx = NULL;
	bool satisfyAllCriteria = false;
	std::vector<HWND> hwnds;
};

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
	WindowsFinderParams& params = *(WindowsFinderParams*)lParam;

	unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;

	// If looking for windows of a specific PID
	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);
	if (params.pidOwner != NULL)
		if (params.pidOwner == pid)
			++satisfiedCriteria; // Doesn't belong to the process targeted
		else
			++unSatisfiedCriteria;

	// If looking for windows of a specific class
	wchar_t className[MAX_CLASSNAME] = L"";
	GetClassName(hwnd, className, MAX_CLASSNAME);
	std::wstring classNameWstr = className;
	if (params.wndClassName != L"")
		if (params.wndClassName == classNameWstr)
			++satisfiedCriteria; // Not the class targeted
		else
			++unSatisfiedCriteria;

	// If looking for windows with a specific name
	wchar_t windowName[MAX_WNDNAME] = L"";
	GetWindowText(hwnd, windowName, MAX_CLASSNAME);
	std::wstring windowNameWstr = windowName;
	if (params.wndName != L"")
		if (params.wndName == windowNameWstr)
			++satisfiedCriteria; // Not the class targeted
		else
			++unSatisfiedCriteria;

	// If looking for window at a specific position
	RECT pos;
	GetWindowRect(hwnd, &pos);
	if (params.pos.left || params.pos.top || params.pos.right || params.pos.bottom)
		if (params.pos.left == pos.left && params.pos.top == pos.top && params.pos.right == pos.right && params.pos.bottom == pos.bottom)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// If looking for window of a specific size
	POINT res = { pos.right - pos.left, pos.bottom - pos.top };
	if (params.res.x || params.res.y)
		if (res.x == params.res.x && res.y == params.res.y)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// If looking for windows taking more than a specific percentage of all the screens
	LONG ratioAllScreensX = res.x / GetSystemMetrics(SM_CXSCREEN);
	LONG ratioAllScreensY = res.y / GetSystemMetrics(SM_CYSCREEN);
	float percentAllScreens = (float)ratioAllScreensX * (float)ratioAllScreensY * 100;
	if (params.percentAllScreens != 0.0f)
		if (percentAllScreens >= params.percentAllScreens)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// If looking for windows taking more than a specific percentage or the main screen
	RECT desktopRect;
	GetWindowRect(GetDesktopWindow(), &desktopRect);
	POINT desktopRes = { desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top };
	LONG ratioMainScreenX = res.x / desktopRes.x;
	LONG ratioMainScreenY = res.y / desktopRes.y;
	float percentMainScreen = (float)ratioMainScreenX * (float)ratioMainScreenY * 100;
	if (params.percentMainScreen != 0.0f)
		if (percentAllScreens >= params.percentMainScreen)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
	if (params.style)
		if (params.style & style)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	LONG_PTR styleEx = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	if (params.styleEx)
		if (params.styleEx & styleEx)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	if (satisfiedCriteria == 0)
	{
		return TRUE;

	}

	if (params.satisfyAllCriteria == 1 && unSatisfiedCriteria > 0)
	{
		return TRUE;
	}

	params.hwnds.push_back(hwnd);
	return TRUE;
}
HWND window_handle;

std::vector<HWND> WindowsFinder(WindowsFinderParams params) {
	EnumWindows(EnumWindowsCallback, (LPARAM)&params);
	return params.hwnds;
}


inline HWND HiJackNotepadWindow() {

	HWND hwnd = NULL;

	std::vector<DWORD> existingNotepads = GetPIDs((L"winver.exe"));
	if (!existingNotepads.empty()) {
		for (int i(0); i < existingNotepads.size(); ++i) {
			// Terminating processes
			HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, existingNotepads[i]);
			TerminateProcess(hOldProcess, 0);
			CloseHandle(hOldProcess);
		}
	}

	system(("start winver.exe")); 

	std::vector<DWORD> notepads = GetPIDs((L"winver.exe"));
	if (notepads.empty() || notepads.size() > 1) 
		return hwnd;
	WindowsFinderParams params;
	params.pidOwner = notepads[0];
	params.style = WS_VISIBLE;
	params.satisfyAllCriteria = true;
	std::vector<HWND> hwnds;
	int attempt = 0; 
	while (hwnd == NULL || attempt > 50) {
		Sleep(50);
		hwnds = WindowsFinder(params);
		if (hwnds.size() > 1)
			return hwnd;
		hwnd = hwnds[0];
		++attempt;
	}
	if (!hwnd)
		return hwnd;

	SetMenu(hwnd, NULL);
	SetWindowLongPtr(hwnd, GWL_STYLE, WS_VISIBLE);
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW); // WS_EX_NOACTIVATE  and WS_EX_TOOLWINDOW removes it from taskbar

	SetWindowPos(hwnd, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
	screen_width = GetSystemMetrics(SM_CXSCREEN);
	screen_height = GetSystemMetrics(SM_CYSCREEN);
	return hwnd;
}


ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);
HWND windows = NULL;
HWND hwnd = NULL;
RECT GameRect = { NULL };

IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD ScreenCenterX;
DWORD ScreenCenterY;

ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_device_ctx;
IDXGISwapChain* d3d_swap_chain;
ID3D11RenderTargetView* d3d_render_target;
D3DPRESENT_PARAMETERS d3d_present_params;

void SetWindowToTarget()
{
	while (true)
	{
		if (windows)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(windows, &GameRect);
			screen_width = GameRect.right - GameRect.left;
			screen_height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(windows, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				screen_height -= 39;
			}
			ScreenCenterX = screen_width / 2;
			ScreenCenterY = screen_height / 2;
			MoveWindow(window_handle, GameRect.left, GameRect.top, screen_width, screen_height, true);
		}
		else
		{
			exit(0);
		}
	}
}

namespace n_render {
	class c_render {
	public:

		//static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
		auto imgui() -> bool {
			DXGI_SWAP_CHAIN_DESC swap_chain_description;
			ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
			swap_chain_description.BufferCount = 2;
			swap_chain_description.BufferDesc.Width = 0;
			swap_chain_description.BufferDesc.Height = 0;
			swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
			swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
			swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_description.OutputWindow = window_handle;
			swap_chain_description.SampleDesc.Count = 1;
			swap_chain_description.SampleDesc.Quality = 0;
			swap_chain_description.Windowed = 1;
			swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			D3D_FEATURE_LEVEL d3d_feature_lvl;

			const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

			D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, d3d_feature_array, 2, D3D11_SDK_VERSION, &swap_chain_description, &d3d_swap_chain, &d3d_device, &d3d_feature_lvl, &d3d_device_ctx);

			ID3D11Texture2D* pBackBuffer;
		

			d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

			d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &d3d_render_target);

			pBackBuffer->Release();

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
			ImFontConfig icons_config;

			ImFontConfig CustomFont;
			CustomFont.FontDataOwnedByAtlas = false;

			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			icons_config.OversampleH = 3;
			icons_config.OversampleV = 3;

			ImGui_ImplWin32_Init(window_handle);
			ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

			ImFontConfig font_config;
			font_config.OversampleH = 1;
			font_config.OversampleV = 1;
			font_config.PixelSnapH = 1;

			static const ImWchar ranges[] =
			{
				0x0020, 0x00FF,
				0x0400, 0x044F,
				0,
			};

			MenuFont = io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Verdana.ttf"), 18.f);
			GameFont = io.Fonts->AddFontFromMemoryTTF(font, sizeof(font), 15.f);
			MenuFont2 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 13.f); //11 before
			MenuFont3 = io.Fonts->AddFontFromMemoryTTF(&LightFont, sizeof LightFont, 15.f);
			io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 20.1f, &icons_config, icons_ranges);
			io.Fonts->AddFontDefault();
			FortniteFont = io.Fonts->AddFontFromMemoryTTF(FnFont, sizeof(FnFont), 17.f);

			ImGui_ImplWin32_Init(window_handle);

			ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

			d3d_device->Release();

			return true;
		}

		auto hijack() -> bool {

			while (!window_handle) {
				window_handle = HiJackNotepadWindow();
				Sleep(10);
			}
			MARGINS margin = { -1 };
			DwmExtendFrameIntoClientArea(window_handle, &margin);
			SetMenu(window_handle, NULL);
			SetWindowLongPtr(window_handle, GWL_STYLE, WS_VISIBLE);
			SetWindowLongPtr(window_handle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

			ShowWindow(window_handle, SW_SHOW);
			UpdateWindow(window_handle);

			SetWindowLong(window_handle, GWL_EXSTYLE, GetWindowLong(window_handle, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);

			return true;
		}
		void AlignForWidth(float width, float alignment = 0.5f)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			float avail = ImGui::GetContentRegionAvail().x;
			float off = (avail - width) * alignment;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
		}
	
		int tabs = 2;

		void RenderTabButtons(const char* label1, const char* label2, const char* label3, const char* label4, const char* label5) {
			ImGui::SetWindowSize(ImVec2(780, 425));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 10));

			ImGui::BeginChild("##Tab", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::PushFont(FortniteFont);
			ImVec2 text_pos = ImGui::GetCursorScreenPos();
			ImVec2 text_pos2 = ImGui::GetCursorScreenPos();


			text_pos.x += 27; 
			text_pos.y += 13; 
			text_pos2.x += 22; 
			text_pos2.y += 40; 

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddText(NULL, 30.0f, text_pos, IM_COL32(255, 255, 255, 255), "Frozen"); 
			draw_list->AddText(NULL, 30.0f, text_pos2, IM_COL32(0, 0, 255, 255), "Slotted"); 


			ImGui::PopFont();

			ImGui::SetCursorPosY(120);
			if (ImGui::Button(label1, ImVec2(138, 30))) tabs = 1;
			if (ImGui::Button(label2, ImVec2(138, 30))) tabs = 2;
			if (ImGui::Button(label3, ImVec2(138, 30))) tabs = 3;
			if (ImGui::Button(label4, ImVec2(138, 30))) tabs = 4;
			if (ImGui::Button(label5, ImVec2(138, 30))) tabs = 5;

			ImGui::EndChild();
			ImGui::PopStyleColor();

		}

		void RenderCombatTab() {
			ImGui::BeginChild("##Combat", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_NoResize);

			ImGui::Text(E("Aimbot"));
			ImGui::Separator();

			ImGui::Checkbox(E("Enable Aimbot"), &globals::g_aimbot);
			ImGui::Checkbox(E("Enable Prediction"), &globals::prediction);
			ImGui::Checkbox(E("Visible Check"), &globals::g_visible_check);
			ImGui::Checkbox(E("Enable Target Text"), &globals::g_target_text);
			ImGui::Checkbox(E("Normal Fov"), &globals::g_render_fov);
			ImGui::Checkbox(E("Crosshair"), &globals::crosshair);

			ImGui::Spacing();

			ImGui::Text("Misc");
			ImGui::Separator();

			ImGui::Text("Aimbot Key");
			HotkeyButton(aimkey, ChangeKey, keystatus);
			float MaxSmooth = globals::vsync ? 20 : 100;
			ImGui::SliderFloat(E("Smooth"), &globals::g_smooth, 1, 29);
			ImGui::SliderFloat(E("FovSize"), &globals::g_aimfov, 10, 300);
			ImGui::Combo("Hitbox", &globals::g_hitbox, "Head\0Neck\0Chest\0Random\0");

			ImGui::EndChild();
		}

		void RenderVisualsTab() {
			ImGui::BeginChild("##Visuals", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_NoResize);

			ImGui::Text("Visuals");
			ImGui::Separator();

			ImGui::Checkbox(E("Box"), &globals::g_box_esp);
			ImGui::Checkbox(E("Skeleton"), &globals::g_skeleton);
			ImGui::Checkbox(E("Name"), &globals::g_username);
			ImGui::Checkbox(E("Platform "), &globals::g_platform);
			ImGui::Checkbox(E("Distance "), &globals::g_distance);
			ImGui::Checkbox(E("Head"), &globals::headesp);
			ImGui::Checkbox(E("Ranked"), &globals::g_ranked);
			ImGui::Checkbox(E("Radar"), &globals::radaresp);
			ImGui::Checkbox(E("Snaplines"), &globals::g_line);
			ImGui::Checkbox(E("Outline"), &globals::g_outline);

			ImGui::Spacing();

			ImGui::Text("Misc");
			ImGui::Separator();
			ImGui::SliderFloat("Render Range", &globals::g_render_distance, 10.f, 275.f, "%.1f");
			ImGui::SliderFloat("Box Thickness", &globals::g_box_thick, 1, 5);
			//ImGui::SliderFloat("Box Width", &, 1, 5);

			//box_width
			ImGui::SliderInt("Line Thickness", &globals::g_line_thick, 1, 5);
			ImGui::SliderInt("Skel Thickness", &globals::g_skeletonthickness, 1, 5);
			ImGui::Combo("Line", &globals::g_line_type, "Top\0Middle\0Bottom\0");		
			ImGui::Combo("Watermark", &globals::g_watermark_type, "Fov\0Fps\0Mikz\0");
			ImGui::Combo("Distance", &globals::g_distance_type, "Top\0Bottom\0");		
			ImGui::Combo("Box", &globals::g_box_type, "2D\0Rounded\0Cornered\0Bounding\0Filled\0");

			ImGui::Text("");
			ImGui::Text("Radar Misc");
			ImGui::Separator();
			ImGui::SliderFloat(E("Radar Pos X"), &radar_position_x, 10.f, 2000.f);
			ImGui::SliderFloat(E("Radar Pos Y"), &radar_position_y, 10.f, 2000.f);
			ImGui::SliderFloat(E("Radar Distance"), &RadarDistance, 10.f, 700.f);
			ImGui::SliderFloat(E("Radar Size"), &radar_size, 100.f, 500.f);
			
			ImGui::EndChild();
		}

		void RenderWorldTab() {
			ImGui::BeginChild("##Configs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_NoResize);

			ImGui::Text("Configs");
			ImGui::Separator();
			ImGui::Checkbox("Enable Chest Esp ", &globals::world::chests);
			ImGui::Checkbox("Enable Comon Loot ", &globals::world::common_loot);
			ImGui::Checkbox("Enable Uncommon Loot ", &globals::world::uncommon_loot);
			ImGui::Checkbox("Enable Epic Loot ", &globals::world::epic_loot);
			ImGui::Checkbox("Enable Legendray Loot", &globals::world::legendary_loot);
			ImGui::Checkbox("Enable Rare Loot", &globals::world::rare_loot);
			ImGui::Checkbox("Enable mythic loot", &globals::world::mythic_loot);
			ImGui::Checkbox("Enable SuplyDrop", &globals::world::SupplyDrop);
			ImGui::SliderFloat("###distance", &globals::world::render_distance, 1, 2000, "distance: %.2f");
			ImGui::EndChild();
		}

		void RenderSettingsTab() {
			ImGui::BeginChild("##Colors", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_NoResize);

			ImGui::Text("Colors");
			ImGui::Separator();
			ImGui::ColorEdit3("Visible Color", globals::g_color_Visible);
			ImGui::ColorEdit3("Not Visible", globals::g_color_invisible);
			ImGui::ColorEdit3("Normal Color", globals::g_color);
			ImGui::ColorEdit3("Text Color", globals::FontColor);


           

			ImGui::Separator();


			ImGui::Spacing();

			ImGui::Text("Misc");
			ImGui::Separator();
			ImGui::Checkbox("VSync", &globals::vsync);
			ImGui::Checkbox("Stream Proof", &globals::vsync);
			ImGui::SliderFloat(E("Font Size"), &globals::g_font_size, 15, 30);


			ImGui::EndChild();
		}

		void RenderConfigTab() {

			ImGui::BeginChild("##Configs", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_NoResize);

			ImGui::Text("Configs");
			ImGui::Separator();

			if (ImGui::Button("Legit Config")) {
				globals::g_smooth = 8;
				globals::g_aimfov = 60;
				globals::g_aimbot = true;
				globals::g_render_fov = true;
				globals::g_platform = true;
				globals::g_username = true;
				globals::g_distance = true;
				globals::g_snapline = true;
				globals::g_visible_check = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Tournament Config")) {
				globals::g_render_fov = true;
				globals::g_platform = false;
				globals::g_username = true;
				globals::g_distance = true;
				globals::g_snapline = false;
				globals::g_visible_check = true;
				globals::g_smooth = 12;
				globals::g_aimfov = 40;
			}
			if (ImGui::Button("Mikz Config")) {
				globals::g_aimbot = true;
				globals::g_render_fov = true;
				globals::g_platform = true;
				globals::g_username = true;
				globals::g_distance = true;
				globals::g_snapline = true;
				globals::g_visible_check = true;
				globals::g_smooth = 11;
				globals::g_aimfov = 65;
			}

			ImGui::Spacing();

			ImGui::EndChild();
		}

		void RenderMenu() {
			static bool previous_insert_state = false;

			bool current_insert_state = GetAsyncKeyState(VK_INSERT) & 0x8000;

			if (current_insert_state && !previous_insert_state) {
				globals::g_render_menu = !globals::g_render_menu;
			}

			previous_insert_state = current_insert_state;

			if (globals::g_render_menu) {
				ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
				{

				
					// Define columns for the menu layout
					ImGui::Columns(2, "");
					ImGui::SetColumnWidth(0, 150); // Set the width of the tab column
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f); // Adjust vertical positioning

					// Render the tab buttons
					RenderTabButtons("Aim", "Visuals", "World", "Settings", "Config");

					ImGui::NextColumn();

					// Render the content based on the selected tab
					switch (tabs) {
					case 1: RenderCombatTab(); break;
					case 2: RenderVisualsTab(); break;
					case 3: RenderWorldTab(); break;
					case 4: RenderSettingsTab(); break;
					case 5: RenderConfigTab(); break;
					}

					ImGui::Columns(1); // Reset column layout
				}
				ImGui::End();
			}
		}



		auto menu() -> void {
		

			
			struct tab
			{
				const char* tab_name;
				int tab_id;
			};

			static int tabs = 0;
			static int settingtab = 0;
			static int current_tab = 0;

			

		
		}

		auto draw() -> void {
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);

			ImGui::StyleColorsDark();
			SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			g_main->actor_loop();
			g_main->world_loop(); // world esp!

			RenderMenu();
			//menu();
			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			d3d_device_ctx->OMSetRenderTargets(1, &d3d_render_target, nullptr);
			d3d_device_ctx->ClearRenderTargetView(d3d_render_target, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			if (globals::vsync)
			{
				d3d_swap_chain->Present(1, 0);
			}
			else
			{
				d3d_swap_chain->Present(0, 0);
			}
		}

		auto render() -> bool {
			static IDXGISwapChain* pSwapChain;
			IDXGISwapChain* pSwaChain = nullptr;
			static RECT rect_og;
			MSG msg = { NULL };
			ZeroMemory(&msg, sizeof(MSG));

			while (msg.message != WM_QUIT)
			{
				UpdateWindow(window_handle);
				ShowWindow(window_handle, SW_SHOW);

				if (PeekMessageA(&msg, window_handle, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				ImGuiIO& io = ImGui::GetIO();
				io.ImeWindowHandle = window_handle;
				io.DeltaTime = 1.0f / 60.0f;

				POINT p_cursor;
				GetCursorPos(&p_cursor);
				io.MousePos.x = p_cursor.x;
				io.MousePos.y = p_cursor.y;

				if (GetAsyncKeyState(VK_LBUTTON)) {
					io.MouseDown[0] = true;
					io.MouseClicked[0] = true;
					io.MouseClickedPos[0].x = io.MousePos.x;
					io.MouseClickedPos[0].x = io.MousePos.y;
				}
				else
					io.MouseDown[0] = false;

				draw();
			}
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			DestroyWindow(window_handle);

			return true;
		}

		void Overlay()
		{
		    hijack();

			imgui();

			render();
		};
	};
} static n_render::c_render* g_render = new n_render::c_render();