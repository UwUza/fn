#include <list>
#include <string>
#include "offsets.hpp"
#include <string.h>
#include "functions.hpp"
#include <iostream>
#include "aimbot.cpp"
#include <unordered_map>
#include <vector>
#include <thread>
#include <future>
#include "andreu.h"
using namespace uee;

#define debug_cache true
std::mutex entity_list_mutex;
void draw_text(ImFont* font, float fontSize, ImVec2 position, ImColor color, const char* text)
{
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x - 1, position.y - 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x + 1, position.y - 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x - 1, position.y + 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x + 1, position.y + 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, position, color, text);
}

namespace g1
{
	class g2 {
	public:


		//void world_cache_loop()
		//{
		//	if (arrays->acknowledged_pawn && (globals::world::uncommon_loot || globals::world::common_loot || globals::world::rare_loot || globals::world::epic_loot || globals::world::legendary_loot || globals::world::mythic_loot || globals::world::chests || globals::world::llamas))
		//	{
		//		items_temp_list.clear();

		//		uintptr_t item_levels = read<uintptr_t>(arrays->UWorld + 0x198);
		//		for (int i = 0; i < read<DWORD>(arrays->UWorld + (0x198 + sizeof(PVOID))); i++)
		//		{
		//			uintptr_t item_level = read<uintptr_t>(item_levels + (i * sizeof(uintptr_t)));
		//			for (int i = 0; i < read<DWORD>(item_level + (0x210 + sizeof(PVOID))); i++)
		//			{
		//				uintptr_t item_pawns = read<uintptr_t>(item_level + 0x210);
		//				uintptr_t item_pawn = read<uintptr_t>(item_pawns + (i * sizeof(uintptr_t)));
		//				uintptr_t item_root_component = read<uintptr_t>(item_pawn + 0x1b0);
		//				Vector3 item_relative_location = read<Vector3>(item_root_component + 0x138);

		//				fvector item_relative_location_as_fvector = fvector(item_relative_location.x, item_relative_location.y, item_relative_location.z);

		//				// Calcula la distancia usando fvector
		//				float item_distance = camera_postion.location.distance(item_relative_location_as_fvector) * 0.01f;

		//				FName item_pawn_name = read<FName>(item_pawn + 0x18);
		//				auto item_pawn_name_string = item_pawn_name.ToString();

		//				bool should_update;
		//				bool b_is_pickup;

		//				if (globals::world::uncommon_loot || globals::world::common_loot || globals::world::rare_loot || globals::world::epic_loot || globals::world::legendary_loot || globals::world::mythic_loot || globals::world::chests || globals::world::llamas)
		//				{
		//					should_update = true;
		//					b_is_pickup = true;
		//				}

		//				if (should_update)
		//				{
		//					items cached_item{};
		//					cached_item.actor = item_pawn;
		//					cached_item.name = item_pawn_name_string;
		//					cached_item.is_pickup = b_is_pickup;
		//					cached_item.distance = item_distance;

		//					items_temp_list.push_back(cached_item);
		//				}
		//			}
		//		}

		//		items_list.clear();
		//		items_list = items_temp_list;
		//	}

		//	std::this_thread::sleep_for(std::chrono::milliseconds(500));
		//}

		//void world_loop()
		//{
		//	if (globals::world::uncommon_loot || globals::world::common_loot || globals::world::rare_loot || globals::world::epic_loot || globals::world::legendary_loot || globals::world::mythic_loot || globals::world::chests || globals::world::llamas)
		//	{
		//		for (items actor : items_list)
		//		{
		//			if (arrays->acknowledged_pawn && actor.actor)
		//			{
		//				ImGui::PushFont(MenuFont);
		//				if ((globals::world::uncommon_loot || globals::world::common_loot || globals::world::rare_loot || globals::world::epic_loot || globals::world::legendary_loot || globals::world::mythic_loot) && (strstr(actor.name.c_str(), xorstr_("FortPickupAthena")) || strstr(actor.name.c_str(), xorstr_("Fort_Pickup_Creative_C"))))
		//				{
		//					if (actor.distance <= 100.0f)
		//					{
		//						uintptr_t actor_root_component = read<uintptr_t>(actor.actor + 0x1b0);
		//						Vector3 actor_relative_location = read<Vector3>(actor_root_component + 0x138);

		//						// Convertir Vector3 a fvector
		//						fvector actor_location_as_fvector = fvector(actor_relative_location.x, actor_relative_location.y, actor_relative_location.z);

		//						fvector2d location = ue5->w2s(actor_location_as_fvector);

		//						if (!is_in_screen(location)) continue;

		//						uintptr_t definition = read<uintptr_t>(actor.actor + 0x370 + 0x18);
		//						EFortRarity tier = read<EFortRarity>(definition + 0xa2);
		//						uint64_t ItemName = read<uint64_t>(definition + 0x40);
		//						uint64_t FData = read<uint64_t>(ItemName + 0x18);
		//						int FLength = read<int>(ItemName + 0x20);
		//						if (FLength > 0 && FLength < 50)
		//						{
		//							wchar_t* WeaponBuffer = new wchar_t[FLength];
		//							mem::read_physical((void*)FData, (PVOID)WeaponBuffer, FLength * sizeof(wchar_t));
		//							std::wstring wstr_buf(WeaponBuffer);
		//							std::string weapon_name = std::string(wstr_buf.begin(), wstr_buf.end());
		//							delete[] WeaponBuffer;

		//							std::string text = weapon_name.c_str();
		//							ImVec2 text_size = ImGui::CalcTextSize(text.c_str());

		//							if (tier == EFortRarity::EFortRarity__Common && globals::world::common_loot)
		//							{
		//								draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
		//							}
		//							else if (tier == EFortRarity::EFortRarity__Uncommon && globals::world::uncommon_loot)
		//							{
		//								draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
		//							}
		//							else if (tier == EFortRarity::EFortRarity__Rare && globals::world::rare_loot)
		//							{
		//								draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
		//							}
		//							else if (tier == EFortRarity::EFortRarity__Epic && globals::world::epic_loot)
		//							{
		//								draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
		//							}
		//							else if (tier == EFortRarity::EFortRarity__Legendary && globals::world::legendary_loot)
		//							{
		//								draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
		//							}
		//							else if (tier == EFortRarity::EFortRarity__Mythic && globals::world::mythic_loot)
		//							{
		//								draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), get_loot_color(tier), text.c_str());
		//							}
		//						}
		//					}
		//				}

		//				if (globals::world::chests && (strstr(actor.name.c_str(), xorstr_("Tiered_Chest")) || strstr(actor.name.c_str(), xorstr_("AlwaysSpawn_NormalChest")) || strstr(actor.name.c_str(), xorstr_("AlwaysSpawn_RareChest"))))
		//				{
		//					if (actor.distance <= 100.0f)
		//					{
		//						auto is_searched = read<BYTE>(actor.actor + 0xe0a);
		//						if (is_searched >> 2 & 1) continue;

		//						uintptr_t actor_root_component = read<uintptr_t>(actor.actor + 0x1b0);
		//						Vector3 actor_relative_location = read<Vector3>(actor_root_component + 0x138);

		//						// Convertir Vector3 a fvector
		//						fvector actor_location_as_fvector = fvector(actor_relative_location.x, actor_relative_location.y, actor_relative_location.z);

		//						fvector2d location = ue5->w2s(actor_location_as_fvector);

		//						if (!is_in_screen(location)) continue;

		//						std::string text = xorstr_("Chest");
		//						ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
		//						draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), ImColor(255, 255, 0, 255), text.c_str());
		//					}
		//				}

		//				if (globals::world::llamas && strstr(actor.name.c_str(), xorstr_("AthenaSupplyDrop_Llama_C")))
		//				{
		//					if (actor.distance <= 100.0f)
		//					{
		//						auto is_searched = read<BYTE>(actor.actor + 0xe0a);
		//						if (is_searched >> 2 & 1) continue;

		//						uintptr_t actor_root_component = read<uintptr_t>(actor.actor + 0x1b0);
		//						Vector3 actor_relative_location = read<Vector3>(actor_root_component + 0x138);

		//						// Convertir Vector3 a fvector
		//						fvector actor_location_as_fvector = fvector(actor_relative_location.x, actor_relative_location.y, actor_relative_location.z);

		//						fvector2d location = ue5->w2s(actor_location_as_fvector);

		//						if (!is_in_screen(location)) continue;

		//						std::string text = "Llama";
		//						ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
		//						draw_text(ImGui::GetFont(), 14.0f, ImVec2(location.x - text_size.x / 2, location.y - 18), ImColor(163, 53, 238, 255), text.c_str());
		//					}
		//				}
		//				ImGui::PopFont();
		//			}
		//		}
		//	}
		//}


		static auto actors() -> void {
			temporary_entity_list.clear();

			/*if (debug_cache)
			{
				std::cout << "Uworld : " << arrays->uworld << std::endl;
			}*/

			arrays->UWorld = read<uintptr_t>(offset::UWorld + virtualaddy);
			arrays->game_instance = read<uintptr_t>(arrays->UWorld + offset::GameInstance);
			arrays->local_player = read<uintptr_t>(read<uintptr_t>(arrays->game_instance + offset::LocalPlayers));


			arrays->player_controller = read<uintptr_t>(arrays->local_player + offset::PlayerController);
			arrays->acknowledged_pawn = read<uintptr_t>(arrays->player_controller + offset::LocalPawn);
			arrays->skeletal_mesh = read<uintptr_t>(arrays->acknowledged_pawn + offset::Mesh);
			arrays->player_state = read<uintptr_t>(arrays->acknowledged_pawn + offset::PlayerState);
			arrays->root_component = read<uintptr_t>(arrays->acknowledged_pawn + offset::RootComponent);
			arrays->current_weapon = read<uintptr_t>(arrays->acknowledged_pawn + offset::CurrentWeapon);
			arrays->relative_location = read<fvector>(arrays->root_component + offset::RelativeLocation);
			arrays->team_index = read<int>(arrays->player_state + offset::TeamIndex);
			arrays->game_state = read<uintptr_t>(arrays->UWorld + offset::GameState);
			arrays->player_array = read<uintptr_t>(arrays->game_state + offset::PlayerArray);
			arrays->player_array_size = read<int>(arrays->game_state + (offset::PlayerArray + sizeof(uintptr_t)));
			//	arrays->current_vehicle = read<uintptr_t>(arrays->acknowledged_pawn + offset::curr);

			for (int i = 0; i < arrays->player_array_size; ++i) {

				auto player_state = read<uintptr_t>(arrays->player_array + (i * sizeof(uintptr_t)));
				auto current_actor = read<uintptr_t>(player_state + offset::PawnPrivate);

				if (current_actor == arrays->acknowledged_pawn) continue;
				auto skeletalmesh = read<uintptr_t>(current_actor + offset::Mesh);
				if (!skeletalmesh) continue;

				auto base_bone = ue5->Bone(skeletalmesh, bone::Root);
				if (base_bone.x == 0 && base_bone.y == 0 && base_bone.z == 0) continue;

				if (!ue5->in_screen(ue5->w2s(ue5->Bone(skeletalmesh, bone::Pelvis)))) continue;

				auto is_despawning = (read<char>(current_actor + 0x758) >> 3); //bIsKnockedback

				if (is_despawning) continue;

				auto root_component = read<uintptr_t>(current_actor + offset::RootComponent);

				auto relative_location1 = read<fvector>(root_component + offset::RelativeLocation);
				//auto team_index = read<int>(player_state + offset::TeamIndex);

				//if (arrays->acknowledged_pawn)
				//{
				//	auto team_index = read<int>(player_state + offset::TeamIndex);
				//	if (arrays->team_index == team_index) continue;
				//}

				entity cached_actors{ };
				cached_actors.entity = current_actor;
				cached_actors.skeletal_mesh = skeletalmesh;
				cached_actors.root_component = root_component;
				cached_actors.relative_location = relative_location1;
				cached_actors.player_state = player_state;
				cached_actors.team_index = TeamIndex;

				temporary_entity_list.push_back(cached_actors);




			}

			entity_list.clear();
			entity_list = temporary_entity_list;
			std::this_thread::sleep_for(std::chrono::milliseconds(450));
		}

	};
} static g1::g2* g_cache = new g1::g2();

