#pragma once
#include <Windows.h>
#include "../utils/vectors.h"

struct ImVec4;

namespace game {
	extern uintptr_t client_info;

	extern uintptr_t client_info_base;

	struct ref_def_view {
		vec2_t tan_half_fov;
		char pad[0xC];
		vec3_t axis[3];
	};

	struct ref_def_t {
		int x;
		int y;
		int width;
		int height;
		ref_def_view view;
	};

	extern struct ref_def_t ref_def;

	HWND get_window();

	bool in_game();

	int player_count();

	int local_index();

	enum stance {
		standing = 0,
		crouching = 1,
		crawling = 2,
		downed = 3,
	};

	class player_t {
	public:
		player_t(uintptr_t address) {
			this->address = address;
		}

		uintptr_t address{};

		bool is_valid();

		bool dead();

		int team_id();

		vec3_t get_origin_position();

		vec3_t get_head_position(vec3_t origin);

		void get_box(vec2_t screen_origin, vec2_t screen_head, vec2_t& top_left, vec2_t& bottom_right);

		int get_stance();

		uintptr_t get_bone_ptr(uintptr_t bone_base, uint64_t bone_index);

		vec3_t get_bone_position(uintptr_t bone_ptr, vec3_t& base_pos, int bone);
	};

	vec3_t get_bone_base_pos(uintptr_t client_info);

	player_t get_local();

	bool w2s(vec3_t world_position, vec2_t& screen_position);

	float units_to_m(float units);

	void log_stats();
}

namespace decryption {
	void update();

	uintptr_t get_client_info(uintptr_t peb);

	uintptr_t get_client_info_base(uintptr_t client_info, uintptr_t peb);

	uintptr_t get_bone_base(uintptr_t peb);

	uintptr_t get_bone_index(uint32_t bone_index);

	uintptr_t get_ref_def();
}