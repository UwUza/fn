#include "LoopHelper.hpp"
#include <iomanip>
#include "triggerbothelper.h"
#include "crypter.h"
#include "ImGui/imgui.h"
#include "crypt23.h"



struct WeaponInformation
{
	int32_t ammo_count;
	int32_t max_ammo;

	BYTE tier;
	std::string weapon_name;
	std::string buildplan;
};
std::string LocalPlayerWeapon;
WeaponInformation WeaponInfo;
int InFovEnemy = 0;
int visennemy = 0;

__int64 TargetedBuild;
bool bTargetedBuild;

bool ShowRadar = true;
bool rect_radar = true;
float radar_position_x{ 10.0f };
float radar_position_y{ 350.0f };
float radar_size{ 250.0f };
float RadarDistance = { 200.f };
float downed[1];
std::wstring string_to_wstring(const std::string& str) {
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
	return wstr;
}

std::string wstring_to_utf8(const std::wstring& wstr) {
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
	return str;
}
std::string get_rank(int32_t Tier)
{
	if (Tier == 0)
		return std::string("Bronze 1");
	else if (Tier == 1)
		return std::string("Bronze 2");
	else if (Tier == 2)
		return std::string("Bronze 3");
	else if (Tier == 3)
		return std::string("Silver 1");
	else if (Tier == 4)
		return std::string("Silver 2");
	else if (Tier == 5)
		return std::string("Silver 3");
	else if (Tier == 6)
		return std::string("Gold 1");
	else if (Tier == 7)
		return std::string("Gold 2");
	else if (Tier == 8)
		return std::string("Gold 3");
	else if (Tier == 9)
		return std::string("Platinum 1");
	else if (Tier == 10)
		return std::string("Platinum 2");
	else if (Tier == 11)
		return std::string("Platinum 3");
	else if (Tier == 12)
		return std::string("Diamond 1");
	else if (Tier == 13)
		return std::string("Diamond 2");
	else if (Tier == 14)
		return std::string("Diamond 3");
	else if (Tier == 15)
		return std::string("Elite");
	else if (Tier == 16)
		return std::string("Champion");
	else if (Tier == 17)
		return std::string("Unreal");
	else
		return std::string("Unranked");
}
ImVec4 get_rank_color(int32_t Tier)
{
	if (Tier == 0 || Tier == 1 || Tier == 2)
		return ImVec4(0.902f, 0.580f, 0.227f, 1.0f); // Bronze
	else if (Tier == 3 || Tier == 4 || Tier == 5)
		return ImVec4(0.843f, 0.843f, 0.843f, 1.0f); // Silver
	else if (Tier == 6 || Tier == 7 || Tier == 8)
		return ImVec4(1.0f, 0.871f, 0.0f, 1.0f); // Gold
	else if (Tier == 9 || Tier == 10 || Tier == 11)
		return ImVec4(0.0f, 0.7f, 0.7f, 1.0f); // Platinum
	else if (Tier == 12 || Tier == 13 || Tier == 14)
		return ImVec4(0.1686f, 0.3294f, 0.8235f, 1.0f); // Diamond
	else if (Tier == 15)
		return ImVec4(0.7f, 0.7f, 0.7f, 1.0f); // Elite
	else if (Tier == 16)
		return ImVec4(1.0f, 0.6f, 0.0f, 1.0f); // Champion
	else if (Tier == 17)
		return ImVec4(0.6f, 0.0f, 0.6f, 1.0f); // Unreal
	else
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Unranked
}

auto getranked(uintptr_t PlayerState) -> std::string
{
	if (is_valid(PlayerState))
	{
		uintptr_t HabaneroComponent = read<uintptr_t>(PlayerState + 0xab0);
		if (is_valid(HabaneroComponent))

		{
			uintptr_t rankedprogress = read<uintptr_t>(HabaneroComponent + 0xb8);
		}
		wchar_t ranked[64] = { 0 }; // Initialize the array to avoid potential issues
		std::wstring ranked_wstr(ranked);
		std::string ranked_str(ranked_wstr.begin(), ranked_wstr.end());
		return ranked_str;
	}
}

namespace Offset
{
	namespace AFortWeapon
	{
		auto bIsReloadingWeapon = 0x388;
		auto Ammo = 0xe54;
	}

	namespace UFortMovementComp_Character
	{
		auto bWantsToSwing = 0x14bc;
		auto SwingAttachLocation = 0x14c0;
		auto bSwingInstantVelocity = 0x14bf;
		auto SwingLaunch = 0x14d8;

		auto bWantsToSkate = 0x5511;
		auto acceleration = 0x58;
	}
}

void CalcRadarPoint(fvector vOrigin, int& screenx, int& screeny)
{
	ue5->get_camera();
	fvector vAngle = camera_postion.rotation;
	auto fYaw = vAngle.y * M_PI / 180.0f;
	float dx = vOrigin.x - camera_postion.location.x;
	float dy = vOrigin.y - camera_postion.location.y;

	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);

	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	x = -x;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float range = (float)RadarDistance * 1000.f;

	ue5->RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImVec2(radar_position_x, radar_position_y);
	ImVec2 DrawSize = ImVec2(radar_size, radar_size);


	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}

void fortnite_radar(float x, float y, float size, bool rect = false)
{
	if (ShowRadar)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Once);
		static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		ImGui::Begin(("##radar"), nullptr, flags);

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Draw transparent rectangle
		ImVec4 rectColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // Fully transparent
		drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + size, y + size), ImGui::GetColorU32(rectColor), 0.0f, 0);

		// Line thickness
		float thickness = 1.5f;

		// Draw white lines with black outline
		ImU32 white = ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 1.0f });
		ImU32 black = ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 1.0f });
		float outline_thickness = 0.5f;

		// Top line
		drawList->AddLine(ImVec2(x, y), ImVec2(x + size, y), black, thickness + outline_thickness);
		drawList->AddLine(ImVec2(x, y), ImVec2(x + size, y), white, thickness);

		// Right line
		drawList->AddLine(ImVec2(x + size, y), ImVec2(x + size, y + size), black, thickness + outline_thickness);
		drawList->AddLine(ImVec2(x + size, y), ImVec2(x + size, y + size), white, thickness);

		// Bottom line
		drawList->AddLine(ImVec2(x + size, y + size), ImVec2(x, y + size), black, thickness + outline_thickness);
		drawList->AddLine(ImVec2(x + size, y + size), ImVec2(x, y + size), white, thickness);

		// Left line
		drawList->AddLine(ImVec2(x, y + size), ImVec2(x, y), black, thickness + outline_thickness);
		drawList->AddLine(ImVec2(x, y + size), ImVec2(x, y), white, thickness);

		// Middle lines
		// Vertical middle line
		drawList->AddLine(ImVec2(x + size / 2, y), ImVec2(x + size / 2, y + size), black, thickness + outline_thickness);
		drawList->AddLine(ImVec2(x + size / 2, y), ImVec2(x + size / 2, y + size), white, thickness);

		// Horizontal middle line
		drawList->AddLine(ImVec2(x, y + size / 2), ImVec2(x + size, y + size / 2), black, thickness + outline_thickness);
		drawList->AddLine(ImVec2(x, y + size / 2), ImVec2(x + size, y + size / 2), white, thickness);

		// Optionally, draw a smaller circle at the center
		float render_size = 5;
		drawList->AddCircleFilled(ImVec2(x + size / 2, y + size / 2), render_size / 2, white, 100);

		ImGui::End();
	}
}




void add_players_radar(fvector WorldLocation)
{
	if (ShowRadar)
	{
		static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		ImGui::Begin(("##radar"), nullptr, flags);

		int ScreenX, ScreenY = 0;
		const auto& GetWindowDrawList = ImGui::GetWindowDrawList();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		CalcRadarPoint(WorldLocation, ScreenX, ScreenY);
		if (globals::g_visible_check) {
			drawList->AddTriangleFilled(
				ImVec2(ScreenX, ScreenY - 5),     // Punkt 1
				ImVec2(ScreenX - 5, ScreenY + 5), // Punkt 2
				ImVec2(ScreenX + 5, ScreenY + 5), // Punkt 3
				ImGui::GetColorU32({ globals::g_color_Visible[0], globals::g_color_Visible[1], globals::g_color_Visible[2], 1.0f })
			);
		}
		else {
			drawList->AddTriangleFilled(
				ImVec2(ScreenX, ScreenY - 5),     // Punkt 1
				ImVec2(ScreenX - 5, ScreenY + 5), // Punkt 2
				ImVec2(ScreenX + 5, ScreenY + 5), // Punkt 3
				ImGui::GetColorU32({ globals::g_color_invisible[0], globals::g_color_invisible[1], globals::g_color_invisible[2], 1.0f })
			);
		}

		ImGui::End();
	}
}

namespace g_loop {


	class g_fn {
	public:

		void world_cache_loop()
		{
			if (arrays->acknowledged_pawn && (globals::world::uncommon_loot || globals::world::common_loot || globals::world::rare_loot || globals::world::epic_loot || globals::world::legendary_loot || globals::world::mythic_loot || globals::world::chests || globals::world::llamas))
			{
				items_temp_list.clear();

				uintptr_t item_levels = read<uintptr_t>(arrays->UWorld + 0x198);
				for (int i = 0; i < read<DWORD>(arrays->UWorld + (0x198 + sizeof(PVOID))); i++)
				{
					uintptr_t item_level = read<uintptr_t>(item_levels + (i * sizeof(uintptr_t)));
					for (int i = 0; i < read<DWORD>(item_level + (0x210 + sizeof(PVOID))); i++)
					{
						uintptr_t item_pawns = read<uintptr_t>(item_level + 0x210);
						uintptr_t item_pawn = read<uintptr_t>(item_pawns + (i * sizeof(uintptr_t)));
						uintptr_t item_root_component = read<uintptr_t>(item_pawn + 0x1b0);
						Vector3 item_relative_location = read<Vector3>(item_root_component + 0x138);

						fvector item_relative_location_as_fvector = fvector(item_relative_location.x, item_relative_location.y, item_relative_location.z);

						// Calcula la distancia usando fvector
						float item_distance = camera_postion.location.distance(item_relative_location_as_fvector) * 0.01f;

						FName item_pawn_name = read<FName>(item_pawn + 0x18);
						auto item_pawn_name_string = item_pawn_name.ToString();

						bool should_update;
						bool b_is_pickup;

						if (globals::world::uncommon_loot || globals::world::common_loot || globals::world::rare_loot || globals::world::epic_loot || globals::world::legendary_loot || globals::world::mythic_loot || globals::world::chests || globals::world::llamas)
						{
							should_update = true;
							b_is_pickup = true;
						}

						if (should_update)
						{
							items cached_item{};
							cached_item.actor = item_pawn;
							cached_item.name = item_pawn_name_string;
							cached_item.is_pickup = b_is_pickup;
							cached_item.distance = item_distance;

							items_temp_list.push_back(cached_item);
						}
					}
				}

				items_list.clear();
				items_list = items_temp_list;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		void world_loop()
		{
			if (globals::world::uncommon_loot || globals::world::common_loot || globals::world::rare_loot || globals::world::epic_loot || globals::world::legendary_loot || globals::world::mythic_loot || globals::world::chests || globals::world::llamas)
			{
				for (items actor : items_list)
				{
					if (arrays->acknowledged_pawn && actor.actor)
					{
						ImGui::PushFont(FortniteFont);//FortniteFont
						if ((globals::world::uncommon_loot || globals::world::common_loot || globals::world::rare_loot || globals::world::epic_loot || globals::world::legendary_loot || globals::world::mythic_loot) && (strstr(actor.name.c_str(), xorstr_("FortPickupAthena")) || strstr(actor.name.c_str(), xorstr_("Fort_Pickup_Creative_C"))))
						{
							if (actor.distance <= 100.0f)
							{
								uintptr_t actor_root_component = read<uintptr_t>(actor.actor + 0x1b0);
								Vector3 actor_relative_location = read<Vector3>(actor_root_component + 0x138);

								// Convertir Vector3 a fvector
								fvector actor_location_as_fvector = fvector(actor_relative_location.x, actor_relative_location.y, actor_relative_location.z);

								fvector2d location = ue5->w2s(actor_location_as_fvector);

								if (!is_in_screen(location)) continue;

								uintptr_t definition = read<uintptr_t>(actor.actor + 0x370 + 0x18);
								EFortRarity tier = read<EFortRarity>(definition + 0xa2);
								uint64_t ItemName = read<uint64_t>(definition + 0x40);
								uint64_t FData = read<uint64_t>(ItemName + 0x18);
								int FLength = read<int>(ItemName + 0x20);
								if (FLength > 0 && FLength < 50)
								{
									wchar_t* WeaponBuffer = new wchar_t[FLength];
									mem::read_physical((void*)FData, (PVOID)WeaponBuffer, FLength * sizeof(wchar_t));
									std::wstring wstr_buf(WeaponBuffer);
									std::string weapon_name = std::string(wstr_buf.begin(), wstr_buf.end());
									delete[] WeaponBuffer;

									std::string text = weapon_name.c_str();
									ImVec2 text_size = ImGui::CalcTextSize(text.c_str());

									if (tier == EFortRarity::EFortRarity__Common && globals::world::common_loot)
									{
										draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
									}
									else if (tier == EFortRarity::EFortRarity__Uncommon && globals::world::uncommon_loot)
									{
										draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
									}
									else if (tier == EFortRarity::EFortRarity__Rare && globals::world::rare_loot)
									{
										draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
									}
									else if (tier == EFortRarity::EFortRarity__Epic && globals::world::epic_loot)
									{
										draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
									}
									else if (tier == EFortRarity::EFortRarity__Legendary && globals::world::legendary_loot)
									{
										draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
									}
									else if (tier == EFortRarity::EFortRarity__Mythic && globals::world::mythic_loot)
									{
										draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
									}
								}
							}
						}

						if (globals::world::chests && (strstr(actor.name.c_str(), xorstr_("Tiered_Chest")) || strstr(actor.name.c_str(), xorstr_("AlwaysSpawn_NormalChest")) || strstr(actor.name.c_str(), xorstr_("AlwaysSpawn_RareChest"))))
						{
							if (actor.distance <= 100.0f)
							{
								auto is_searched = read<BYTE>(actor.actor + 0xe0a);
								if (is_searched >> 2 & 1) continue;

								uintptr_t actor_root_component = read<uintptr_t>(actor.actor + 0x1b0);
								Vector3 actor_relative_location = read<Vector3>(actor_root_component + 0x138);

								// Convertir Vector3 a fvector
								fvector actor_location_as_fvector = fvector(actor_relative_location.x, actor_relative_location.y, actor_relative_location.z);

								fvector2d location = ue5->w2s(actor_location_as_fvector);

								if (!is_in_screen(location)) continue;

								std::string text = xorstr_("Chest");
								ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
								draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), ImColor(255, 255, 0, 255), text.c_str());
							}
						}

						if (globals::world::llamas && strstr(actor.name.c_str(), xorstr_("AthenaSupplyDrop_Llama_C")))
						{
							if (actor.distance <= 100.0f)
							{
								auto is_searched = read<BYTE>(actor.actor + 0xe0a);
								if (is_searched >> 2 & 1) continue;

								uintptr_t actor_root_component = read<uintptr_t>(actor.actor + 0x1b0);
								Vector3 actor_relative_location = read<Vector3>(actor_root_component + 0x138);

								// Convertir Vector3 a fvector
								fvector actor_location_as_fvector = fvector(actor_relative_location.x, actor_relative_location.y, actor_relative_location.z);

								fvector2d location = ue5->w2s(actor_location_as_fvector);

								if (!is_in_screen(location)) continue;

								std::string text = "Llama";
								ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
								draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), ImColor(163, 53, 238, 255), text.c_str());
							}
						}
						ImGui::PopFont();
					}
				}
			}
		}

		auto actor_loop() -> void {
			//ImGui::PushFont(GameFont);
			ImGui::PushFont(FortniteFont);

			ue5->get_camera();

			ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
			float target_dist = FLT_MAX;
			uintptr_t target_entity = 0;

			if (globals::g_watermark) {
				switch (globals::g_watermark_type)
				{
				case 0:
				{
					char aimfovText[64];
					sprintf_s(aimfovText, sizeof(aimfovText), "gg/frozenfn", globals::g_aimfov);
					DrawString(globals::g_font_size, 10, 30, ImColor(255, 255, 255, 255), false, true, aimfovText);
					break;
				}

				case 1: //Normal2
				{

					char fps[64];
					sprintf_s(fps, sizeof(fps), "FPS-> %.1f", ImGui::GetIO().Framerate);
					DrawString(globals::g_font_size, 110, 70, ImColor(255, 255, 255, 255), false, true, fps);
					break;
				}

				case 2: //Normal3
				{
					ImVec2 rectMin3 = ImVec2(10, 10);
					ImVec2 rectMax3 = ImVec2(120, 30);

					draw_list->AddRectFilled(rectMin3, rectMax3, ImColor(255, 192, 203, 255), 5.0f);
					draw_list->AddRect(rectMin3, rectMax3, ImColor(255, 192, 203, 255), 5.0f);

					ImVec2 textSize3 = ImGui::CalcTextSize("Mikz Private");

					ImVec2 textPos3 = ImVec2(rectMin3.x + (rectMax3.x - rectMin3.x - textSize3.x) * 0.5f, rectMin3.y + (rectMax3.y - rectMin3.y - textSize3.y) * 0.5f);

					draw_list->AddText(textPos3, ImColor(255, 255, 255), "Mikz Private");
					break;
				}

		
				}
			}


			const float centerWidth = screen_width / 2;
			const float centerHeight = screen_height / 2;

			for (auto& cached : entity_list) {
				auto root_bone = ue5->Bone(cached.skeletal_mesh, bone::Root);
				root = ue5->w2s(root_bone);

				root_box = ue5->w2s(fvector(root_bone.x, root_bone.y, root_bone.z + 30));

				root_box1 = ue5->w2s(fvector(root_bone.x, root_bone.y, root_bone.z - 15));
				float box_height = abs(head.y - root_box1.y);
				float box_width = box_height * 0.58f;
				auto head_bone = ue5->Bone(cached.skeletal_mesh, bone::Head);
				head = ue5->w2s(head_bone);
				head_box = ue5->w2s(fvector(head_bone.x, head_bone.y, head_bone.z + 15));

				float distance = arrays->relative_location.distance(root_bone) / 100;

				auto pawn_private1 = read <uintptr_t>(PlayerState + 0x308);
				if (arrays->acknowledged_pawn && !arrays->current_vehicle)
				if (distance > globals::g_render_distance && arrays->acknowledged_pawn) continue;

				if (globals::g_aimbot) {
					auto dx = head.x - (screen_width / 2);
					auto dy = head.y - (screen_height / 2);
					auto dist = sqrtf(dx * dx + dy * dy);

					if (globals::g_visible_check) {
						if (ue5->is_visible(cached.skeletal_mesh)) {
							if (dist < globals::g_aimfov && dist < target_dist) {
								target_dist = dist;
								target_entity = cached.entity;
							}
						}
					}
					else {
						if (dist < globals::g_aimfov && dist < target_dist) {
							target_dist = dist;
							target_entity = cached.entity;
						}
					}
				}
				
				if (globals::g_render_fov) {
					ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(screen_width / 2, screen_height / 2), globals::g_aimfov, ImColor(255, 255, 255), 64, 1);
				}
				if (globals::g_star_fov)
				{
					ImColor fov_col;

					float size = globals::g_aimfov;
					ImVec2 center(screen_width / 2, screen_height / 2);

					ImVec2 starPoints[10];
					float angleOff = 144.0f;

					for (int i = 0; i < 5; i++) {
						starPoints[i * 2] = ImVec2(center.x + size * sin(angleOff * i * 3.14159265359 / 180.0f),
							center.y - size * cos(angleOff * i * 3.14159265359 / 180.0f));
						starPoints[i * 2 + 1] = ImVec2(center.x + size / 2 * sin(angleOff * (i * 2 + 1) * 3.14159265359 / 180.0f),
							center.y - size / 2 * cos(angleOff * (i * 2 + 1) * 3.14159265359 / 180.0f));
					}

					for (int i = 0; i < 5; i++) {
						int next = (i + 1) % 5;
						ImGui::GetBackgroundDrawList()->AddLine(starPoints[i * 2], starPoints[next * 2], fov_col, 2);
					}
				}
				int visibleMeshes = 0;
				int nonVisibleMeshes = 0;
				ImVec2 countPosition = ImVec2(10, 10);
				std::string visibleCountStr = std::string(verucryptt("Visible Entities: ")) + std::to_string(visibleMeshes);
				ImVec2 visibleTextSize = ImGui::CalcTextSize(visibleCountStr.c_str());
				if (arrays->aactor != arrays->acknowledged_pawn)
				{
					visibleMeshes++;
				}
				int playersWithin50m = 0;

				if (globals::nearbyplayers)
				{
					if (distance <= globals::entitydistance)
					{
						playersWithin50m++;
					}

					ImVec2 playerCountPosition = ImVec2(countPosition.x, countPosition.y + visibleTextSize.y + 5);
					std::string nearbyPlayerCountStr = std::string(verucryptt("Nearby Players: ")) + std::to_string(playersWithin50m);

					ImVec2 distancePosition = ImVec2(playerCountPosition.x, playerCountPosition.y + visibleTextSize.y + 5);
					//std::string distanceText = std::string(verucryptt("Current Nearby Entities Distance: ")) + std::to_string(globals::entitydistance);
					ImVec2 textSize = ImGui::CalcTextSize(nearbyPlayerCountStr.c_str());

					// Textposition berechnen, um ihn zentriert oben auf dem Bildschirm zu platzieren
					ImVec2 textPosition(screen_width / 2.0f - textSize.x / 2.0f, 10.0f);

					// Text zum DrawList hinzufügen
					ImGui::GetBackgroundDrawList()->AddText(textPosition, ImColor(0, 255, 0, 255), nearbyPlayerCountStr.c_str());
					//ImGui::GetBackgroundDrawList()->AddText(distancePosition, ImColor(255, 255, 255, 255), distanceText.c_str());
				}

				if (globals::crosshair)
				{
					ImVec2 center(screen_width / 2, screen_height / 2);

					// Convert ImVec4 color to ImU32
					ImU32 crosshair_color_u32 = ImGui::ColorConvertFloat4ToU32(globals::crosshair_color);

					// Main crosshair lines
					draw_list->AddLine(ImVec2(center.x - globals::crosshair_size, center.y), ImVec2(center.x - globals::crosshair_length, center.y), crosshair_color_u32, globals::crosshair_thickness);
					draw_list->AddLine(ImVec2(center.x + globals::crosshair_size, center.y), ImVec2(center.x + globals::crosshair_length, center.y), crosshair_color_u32, globals::crosshair_thickness);
					draw_list->AddLine(ImVec2(center.x, center.y - globals::crosshair_size), ImVec2(center.x, center.y - globals::crosshair_length), crosshair_color_u32, globals::crosshair_thickness);
					draw_list->AddLine(ImVec2(center.x, center.y + globals::crosshair_size), ImVec2(center.x, center.y + globals::crosshair_length), crosshair_color_u32, globals::crosshair_thickness);

					// Secondary crosshair lines
					draw_list->AddLine(ImVec2(center.x - globals::crosshair_size - 1, center.y), ImVec2(center.x - globals::crosshair_length + 1, center.y), crosshair_color_u32, globals::crosshair_thickness - 1);
					draw_list->AddLine(ImVec2(center.x + globals::crosshair_size + 1, center.y), ImVec2(center.x + globals::crosshair_length - 1, center.y), crosshair_color_u32, globals::crosshair_thickness - 1);
					draw_list->AddLine(ImVec2(center.x, center.y - globals::crosshair_size - 1), ImVec2(center.x, center.y - globals::crosshair_length + 1), crosshair_color_u32, globals::crosshair_thickness - 1);
					draw_list->AddLine(ImVec2(center.x, center.y + globals::crosshair_size + 1), ImVec2(center.x, center.y + globals::crosshair_length - 1), crosshair_color_u32, globals::crosshair_thickness - 1);
				}

				ImColor vis;
				ImColor invis;
				ImColor normal;

				vis = ImVec4(globals::g_color_Visible[0], globals::g_color_Visible[1], globals::g_color_Visible[2], 1.0f);
				invis = ImVec4(globals::g_color_invisible[0], globals::g_color_invisible[1], globals::g_color_invisible[2], 1.0f);
				normal = ImVec4(globals::g_color[0], globals::g_color[1], globals::g_color[2], 1.0f);

				ImColor visibleColor;
				if (globals::g_visible_check && globals::g_enable_esp)
				{
					visibleColor = ue5->is_visible(cached.skeletal_mesh) ? ImColor(vis) : ImColor(invis);
				}
				else
				{
					visibleColor = ImColor(normal);
				}

				if (globals::g_line && globals::g_enable_esp)
				{
					switch (globals::g_line_type)
					{
					case 0: //top
						draw_list->AddLine(ImVec2(screen_width / 2, 0), ImVec2(head_box.x, head_box.y), visibleColor, globals::g_line_thick);
						break;
					case 1:	//middle
						draw_list->AddLine(ImVec2(screen_width / 2, screen_height / 2), ImVec2(head_box.x, head_box.y), visibleColor, globals::g_line_thick);
						break;
					case 2: //bottom
						draw_list->AddLine(ImVec2(screen_width / 2, screen_height), ImVec2(root.x, root.y), visibleColor, globals::g_line_thick);
						break;
					}
				}

				if (globals::g_box_esp && globals::g_enable_esp) {
					switch (globals::g_box_type)
					{
					case 0:  //2d
						if (globals::g_outline)
						{
							draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), head_box.y), ImVec2(root.x + (box_width / 2), head_box.y), ImColor(0, 0, 0), globals::g_box_thick + 2);
							draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), head_box.y), ImVec2(head_box.x - (box_width / 2), root.y), ImColor(0, 0, 0), globals::g_box_thick + 2);
							draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), root.y), ImVec2(root.x + (box_width / 2), root.y), ImColor(0, 0, 0), globals::g_box_thick + 2);
							draw_list->AddLine(ImVec2(root.x + (box_width / 2), head_box.y), ImVec2(root.x + (box_width / 2), root.y), ImColor(0, 0, 0), globals::g_box_thick + 2);
						}

						draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), head_box.y), ImVec2(root.x + (box_width / 2), head_box.y), visibleColor, globals::g_box_thick);
						draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), head_box.y), ImVec2(head_box.x - (box_width / 2), root.y), visibleColor, globals::g_box_thick);
						draw_list->AddLine(ImVec2(head_box.x - (box_width / 2), root.y), ImVec2(root.x + (box_width / 2), root.y), visibleColor, globals::g_box_thick);
						draw_list->AddLine(ImVec2(root.x + (box_width / 2), head_box.y), ImVec2(root.x + (box_width / 2), root.y), visibleColor, globals::g_box_thick);
						break;

					case 1: /*rounded*/
						if (globals::g_outline)
						{
							draw_list->AddRect(ImVec2(head_box.x - box_width / 2, head_box.y), ImVec2((head_box.x - box_width / 2) + box_width, head_box.y + box_height), ImColor(0, 0, 0), 10, ImDrawCornerFlags_All, globals::g_box_thick + 2.0);
						}

						draw_list->AddRect(ImVec2(head_box.x - box_width / 2, head_box.y), ImVec2((head_box.x - box_width / 2) + box_width, head_box.y + box_height), visibleColor, 10, ImDrawCornerFlags_All, globals::g_box_thick);

						break;

					case 2: /*cornered*/
						if (globals::g_outline)
						{
							DrawCorneredBox(root.x - (box_width / 2), head_box.y, box_width, box_height, ImColor(0, 0, 0), globals::g_box_thick + 2.0, 0, 0);
						}

						DrawCorneredBox(root.x - (box_width / 2), head_box.y, box_width, box_height, visibleColor, globals::g_box_thick, 0, 0);
						break;

					case 3: /*bounding*/
						if (globals::g_outline)
						{
							TwoD(head_box.x - (box_width / 2), head_box.y, box_width, box_height, ImColor(0, 0, 0), globals::g_box_thick + 2.5);
						}

						TwoD(head_box.x - (box_width / 2), head_box.y, box_width, box_height, visibleColor, globals::g_box_thick);
						break;

					case 4: /*filled*/
					{
						ImVec4 filledColorTransparent = ImVec4(1.0f, 0.0f, 0.0f, 0.5f); // Adjust the alpha (0.5f) for the desired transparency
						draw_list->AddRectFilled(ImVec2(head_box.x - (box_width / 2), head_box.y),
							ImVec2(root.x + (box_width / 2), root.y),
							IM_COL32(filledColorTransparent.x * 255, filledColorTransparent.y * 255, filledColorTransparent.z * 255, filledColorTransparent.w * 255));
					}

					// Make the second rectangle blue and transparent
					draw_list->AddRectFilled(ImVec2(head_box.x - (box_width / 2), head_box.y),
						ImVec2(root.x + (box_width / 2), root.y),
						IM_COL32(0, 0, 255, 128)); // Blue (0, 0
					
					}
				}

				//if (globals::g_username && globals::g_enable_esp) {
				//	switch (globals::g_username_type)
				//	{
				//	case 0: //top
				//	{
				//		std::string username_str = ue5->get_player_name(cached.player_state);
				//		ImVec2 text_size = ImGui::CalcTextSize(username_str.c_str());
				//		DrawString(globals::g_font_size, head_box.x - (text_size.x / 2), head_box.y - 20, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), false, true, username_str.c_str());
				//		break;
				//	}

				//	case 1: //bottom
				//	{
				//		std::string username_str = ue5->get_player_name(cached.player_state);
				//		ImVec2 text_size = ImGui::CalcTextSize(username_str.c_str());
				//		DrawString(globals::g_font_size, root.x - (text_size.x / 2), root.y + 20, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), false, true, username_str.c_str());
				//		break;
				//	}
				//	}
				//}

				if (globals::g_platform && globals::g_enable_esp) {
					DWORD_PTR test_platform = read<DWORD_PTR>((uintptr_t)cached.player_state + 0x430);

					wchar_t platform[64];

					mem::read_physical((PVOID)test_platform, (uint8_t*)platform, sizeof(platform));

					std::wstring platform_wstr(platform);

					std::string platform_str(platform_wstr.begin(), platform_wstr.end());

					ImVec2 textPosition(head_box.x, head_box.y - 35);

					draw_outlined_text( textPosition, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), platform_str.c_str());
				}

				if (globals::g_ranked)
				{
					ImVec2 text_size;
					uintptr_t habenero = read<uintptr_t>(cached.player_state + 0xa38);
					int32_t RankProgress = read<int32_t>(habenero + 0xd0 + 0x10);
					std::string ranktype = get_rank(RankProgress);
					std::wstring w_ranktype = string_to_wstring(ranktype);
					std::string final = wstring_to_utf8(w_ranktype);
					char ranked[64];
					sprintf_s(ranked, E("%s"), (final));

					ImVec4 rankedColor = get_rank_color(RankProgress);

					//DrawString(13, root_box1.x - 13, root_box1.y - 7, rankedColor, true, true, ranked);
					//DrawString(globals::g_font_size, head_box.x - (text_size.x / 2), head_box.y - 40, rankedColor, false, true, ranked);
					DrawString(globals::g_font_size, head_box.x - (text_size.x / 2) - 25, head_box.y - 50, rankedColor, false, true, ranked);

				}

				if (globals::headesp)
				{
					fvector2d head_2d = ue5->w2s(fvector(head_bone.x, head_bone.y, head_bone.z + 20));

					fvector delta = head_bone - camera_postion.location;
					float distance = delta.length();

					const float constant_circle_size = 10;

					float circle_radius = constant_circle_size * (screen_height / (2.0f * distance * tanf(camera_postion.fov * (float)M_PI / 360.f))) - 1.5;

					float y_offset = +70.0f;
					head_2d.y += y_offset;

					int segments = 50;
					float thickness = 2.0f;

					if (globals::g_outline)
					{
						ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, ImColor(0, 0, 0, 255), segments, globals::g_skeletonthickness + 2);
					}
					ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, visibleColor, segments, globals::g_skeletonthickness);
				}

				if (globals::radaresp)
				{
					float radarX = radar_position_x;
					float radarY = radar_position_y;
					float radarSize = radar_size;
					bool isRect = rect_radar;
					fortnite_radar(radarX, radarY, radarSize, isRect);
					add_players_radar(root_bone);

				}		

				if (globals::g_distance && globals::g_enable_esp)
				{
					switch (globals::g_distance_type)
					{
					case 0: //top
					{
						std::string Distance = "[" + std::to_string((int)distance) + (std::string)"m]";

						ImVec2 text_size = ImGui::CalcTextSize(Distance.c_str());

						DrawString(globals::g_font_size, head_box.x - (text_size.x / 2), head_box.y + 5, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), false, true, Distance.c_str());
						break;
					}

					case 1: //bottom
					{
						std::string Distance = "[" + std::to_string((int)distance) + (std::string)"m]";

						ImVec2 text_size = ImGui::CalcTextSize(Distance.c_str());

						DrawString(globals::g_font_size, root.x - (text_size.x / 2), root.y + 5, ImColor(globals::FontColor[0], globals::FontColor[1], globals::FontColor[2]), false, true, Distance.c_str());
						break;
					}
					}
				}

				if (globals::g_skeleton && globals::g_enable_esp) {
					ue5->skeleton(cached.skeletal_mesh);
				}
			}


			float ProjectileSpeed = 60000; // for the Reaper Sniper Rifle 
			fvector ComponentVelocity = read<fvector>(arrays->root_component + 0x168); // ComponentVelocity value
			auto test = read<uintptr_t>(target_entity + offset::RootComponent);



				if (globals::prediction && GetAsyncKeyState(aimkey)) {
						//float projectileSpeed = 27000.f;
						float projectileSpeed = 60000;
						float projectileGravityScale = 3.5f;
						auto closest_mesh2 = read <std::uint64_t>(target_entity + offset::Mesh);

						fvector hitbox = ue5->Bone(closest_mesh2, bone::Head);

						float distance = arrays->relative_location.distance(hitbox);

						auto test = read<uintptr_t>(target_entity + offset::RootComponent);

						fvector Velocity = read<fvector>(test + 0x168);

						fvector Predictor = ue5->Prediction(hitbox, Velocity, distance, ProjectileSpeed);

						fvector2d hitbox_screen_predict = ue5->w2s(Predictor);

						input->move(hitbox_screen_predict);

						// Assign hitbox_screen_predict to wherever you need it, not ue5->PredictLocation
			}

			if (globals::g_triggerbot) {
				{
					__int64 temp_targted = read<__int64>(arrays->player_controller + 0x1930);
					bool b = GetAsyncKeyState(triggerkey);
					if (TargetedBuild != 0 && TargetedBuild != temp_targted)
					{
						INPUT    Input = { 0 };
						// left down
						Input.type = INPUT_MOUSE;
						Input.mi.dwFlags = TYPES::left_down;
						::SendInput(1, &Input, sizeof(INPUT));

						// left up
						::ZeroMemory(&Input, sizeof(INPUT));
						Input.type = INPUT_MOUSE;
						Input.mi.dwFlags = TYPES::left_up;
						::SendInput(1, &Input, sizeof(INPUT));
						TargetedBuild = 0;
					}

					if (TargetedBuild != temp_targted && TargetedBuild != 0)
						Beep(500, 500);
				}
			}

			if (target_entity && globals::g_aimbot) {
				auto closest_mesh = read <std::uint64_t>(target_entity + offset::Mesh);

				float projectileSpeed = 0;
				float projectileGravityScale = 0;
				fvector hitbox;
				fvector2d hitbox_screen;

				switch (globals::g_hitbox) {
				case 0:
					hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Head));
					hitbox = ue5->Bone(closest_mesh, bone::Head);;
					break;
				case 1:
					hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Neck));
					hitbox = ue5->Bone(closest_mesh, bone::Neck);;
					break;
				case 2:
					hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Chest));
					hitbox = ue5->Bone(closest_mesh, bone::Chest);;
					break;
				case 3:
					std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
					std::uniform_int_distribution<int> distribution(0, 3);
					int randomHitbox = distribution(rng);

					switch (randomHitbox) {
					case 0:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Head));
						hitbox = ue5->Bone(closest_mesh, bone::Head);
						break;
					case 1:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Neck));
						hitbox = ue5->Bone(closest_mesh, bone::Neck);
						break;
					case 2:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Chest));
						hitbox = ue5->Bone(closest_mesh, bone::Chest);
						break;
					case 3:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::RShoulder));
						hitbox = ue5->Bone(closest_mesh, bone::RShoulder);
						break;
					case 4:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::LShoulder));
						hitbox = ue5->Bone(closest_mesh, bone::LShoulder);
						break;
					}
					break;
				}
				std::string LocalPlayerWeapon;

				fvector2d hitbox_screen2 = ue5->w2s(hitbox);

				if (hitbox.x != 0 || hitbox.y != 0 && (get_cross_distance(hitbox.x, hitbox.y, screen_width / 2, screen_height / 2) <= globals::g_aimfov))
				{
					if (ue5->is_visible(closest_mesh))
					{
					

						if (globals::g_target_text)
						{
							DrawString(20, hitbox_screen.x - 6, hitbox_screen.y - 80, ImColor(255, 0, 0), true, true, ("TARGET"));
						}

						if (GetAsyncKeyState(aimkey))
							input->move(hitbox_screen);
					}
				}
			}
			else {
				target_dist = FLT_MAX;
				target_entity = NULL;
			}

			ImGui::PopFont();
		}
	};
} static g_loop::g_fn* g_main = new g_loop::g_fn();