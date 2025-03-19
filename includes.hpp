#include "vectors.hpp"
#include "ImGui/imgui.h"
#include <d3d9.h>
#include "ImGui/imgui_impl_win32.h"
#include "Drivers.h"
#include "globals.hpp"
#include "import.hpp"
ImFont* MenuFont;
ImFont* GameFont;
ImFont* MenuFont2;
ImFont* MenuFont3;
ImFont* IconsFont;
ImFont* FortniteFont;
ImFont* NormalFont;

class entity {
public:
	uintptr_t
		entity,
		skeletal_mesh,
		root_component,
		player_state;
	int
		team_index;
	fvector
		relative_location;
	float
		lastrendertime;
	bool
		is_visible;
};
std::vector<entity> entity_list;
std::vector<entity> temporary_entity_list;

namespace o {
	class p
	{
	public:
		uintptr_t
			UWorld,
			game_instance,
			game_state,
			local_player,
			player_controller,
			acknowledged_pawn,
			aactor,
			skeletal_mesh,
			current_weapon,
			player_state,
			root_component,
			current_vehicle,
			player_array;
		int
			team_index,
			player_array_size;
		fvector
			relative_location,
			velocity;
	};
} static o::p* arrays = new o::p();



