#include "features.h"
#include "../game.h"
#include "../offsets.h"
#include "../../utils/globals.h"
#include "../../utils/utils.h"
#include "../../renderer/renderer.h"

vec2_t calc_angle(vec3_t source, vec3_t dest, vec2_t angles) {
	auto delta = source - dest;
	vec2_t relative_angles{};
	auto radians_to_degrees = [](float radians) { return radians * 180 / static_cast<float>(M_PI); };
	relative_angles.x = radians_to_degrees(atanf(delta.z / hypotf(delta.x, delta.y))) - angles.x;
	relative_angles.y = radians_to_degrees(atanf(delta.y / delta.x)) - angles.y;

	if (delta.x >= 0.0)
		angles.y += 180.0f;

	relative_angles.normalize();
	return relative_angles;
}

void get_closest_player(game::player_t& target_player, vec2_t& target_angles) {
	auto local = game::get_local();
	auto local_pos = local.get_origin_position();
	auto local_team = local.team_id();
	auto bone_base_pos = game::get_bone_base_pos(game::client_info);

	float best_delta = FLT_MAX;
	for (int i = 0; i < game::player_count(); i++) {

		game::player_t player(game::client_info_base + (i * offsets::player::size), i);
		if (!player.is_valid() || player.dead()) {
			continue;
		}

		if (player.team_id() == local_team) {
			continue;
		}

		vec3_t origin_pos = player.get_origin_position();
		auto bone = player.get_bone(bone_base_pos, game::bone_pos_neck);
		if (game::units_to_m(local_pos.distance_to(bone)) < 200) {
			auto target_aim_positon = bone;
			auto angles = calc_angle(game::get_camera_position(), target_aim_positon, game::get_camera_angles());
			auto delta = angles.length();

			if (delta < best_delta && delta < globals::settings::fov) {
				target_player = player;
				best_delta = delta;
				target_angles = angles;
			}
		}

	}
}

namespace features {
	void aimbot() {
		game::bone_base = decryption::get_bone_base(__readgsqword(0x60));

		game::player_t target_player{0, 0};
		vec2_t target_angles{};
		get_closest_player(target_player, target_angles);

		if (!target_player.address)
			return;

		vec2_t screen_chest{};
		if (game::w2s(target_player.get_bone(game::get_bone_base_pos(game::client_info), game::bone_pos_neck), screen_chest)) {
			renderer::draw_circle(screen_chest, 6, { 0, 0, 0, 255 }, 1, 32);
		}

		auto fov_multiplier = 120.0 / game::get_fov();
		auto tickrate_multiplier = 1.0;
		auto speed_multiplier = globals::settings::aimbot_speed;
		auto scale = 1.0 / 2.5;
// credits mutavblew
		auto multiplier = fov_multiplier * tickrate_multiplier * speed_multiplier;
		auto scaled_delta = target_angles * (multiplier * scale);

		auto dx_f = -scaled_delta.y;
		auto dy_f = scaled_delta.x;

		if (dx_f < 1.0f && dx_f > -1.0f) {
			dx_f = 0;
		}

		if (dy_f < 1.0f && dy_f > -1.0f) {
			dy_f = 0;
		}

		INPUT input = {};
		input.type = INPUT_MOUSE;
		input.mi.dx = dx_f;
		input.mi.dy = dy_f;
		input.mi.dwFlags = MOUSEEVENTF_MOVE;
		utils::send_input(input);
	}
}
