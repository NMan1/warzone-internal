#pragma once
#include <Windows.h>
#include "../utils/vectors.h"
#include <vector>

struct ImVec4;

namespace game {
	extern uintptr_t client_info;

	extern uintptr_t client_info_base;

	extern uintptr_t bone_base;

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

	float get_fov();

	enum stance {
		standing = 0,
		crouching = 1,
		crawling = 2,
		downed = 3,
	};

	enum bones {
		bone_pos_helmet = 8,

		bone_pos_head = 7,
		bone_pos_neck = 6,
		bone_pos_chest = 5,
		bone_pos_mid = 4,
		bone_pos_tummy = 3,
		bone_pos_pelvis = 2,

		bone_pos_right_foot_1 = 21,
		bone_pos_right_foot_2 = 22,
		bone_pos_right_foot_3 = 23,
		bone_pos_right_foot_4 = 24,

		bone_pos_left_foot_1 = 17,
		bone_pos_left_foot_2 = 18,
		bone_pos_left_foot_3 = 19,
		bone_pos_left_foot_4 = 20,

		bone_pos_left_hand_1 = 13,
		bone_pos_left_hand_2 = 14,
		bone_pos_left_hand_3 = 15,
		bone_pos_left_hand_4 = 16,

		bone_pos_right_hand_1 = 9,
		bone_pos_right_hand_2 = 10,
		bone_pos_right_hand_3 = 11,
		bone_pos_right_hand_4 = 12
	};

	class player_t {
	public:
		player_t(uintptr_t address, int id) {
			this->address = address;
			this->id = id;
		}

		uintptr_t address{};

		int id{};

		bool is_valid();

		bool dead();

		int team_id();

		vec3_t get_origin_position();

		vec3_t get_head_position(vec3_t origin);

		void get_box(vec2_t screen_origin, vec2_t screen_head, vec2_t& top_left, vec2_t& bottom_right);

		int get_stance();

		vec3_t get_bone(vec3_t bone_base_pos, int id);
	};

	vec3_t get_bone_base_pos(uintptr_t client_info);

	vec3_t get_camera_position();

	vec2_t get_camera_angles();

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