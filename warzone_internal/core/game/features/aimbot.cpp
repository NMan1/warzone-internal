#include "features.h"
#include "../game.h"
#include "../offsets.h"
#include "../../utils/globals.h"
#include "../../utils/utils.h"
#include "../../renderer/renderer.h"

vec2_t calc_relative_angles(vec3_t source, vec3_t dest, vec2_t angles) {
	auto delta = dest - source;
	vec2_t relative_angles{ utils::radians_to_deg(atan2(-delta.z, hypot(delta.x, delta.y))) - angles.x, 
							utils::radians_to_deg(atan2(delta.y, delta.x)) - angles.y };

	relative_angles.normalize();
	return relative_angles;
}

namespace features {
	void aimbot() {
		auto bone_base_pos = game::get_bone_base_pos(game::client_info);

		vec2_t target_angles{};
		game::player_t* best_player = nullptr;
		float best_delta = FLT_MAX;
		for (auto& player : game::valid_players) {
			auto target_aim_positon = player.get_bone(bone_base_pos, game::bone_pos_chest);
			auto angles = calc_relative_angles(game::get_camera_position(), target_aim_positon, game::get_camera_angles());
			auto delta = angles.length();

			vec2_t w2s{};
			if (game::w2s(player.get_bone(bone_base_pos, game::bone_pos_chest), w2s)) {
				char buf[64];
				sprintf(buf, "%f", delta);
				renderer::draw_text(buf, w2s, 8, { 0, 0, 0, 255 });
			}

			if (delta < best_delta && delta < globals::settings::fov) {
				best_player = &player;
				best_delta = delta;
				target_angles = angles;
			}
		}

		if (!best_player)
			return;

		vec2_t w2s{};
		if (game::w2s(best_player->get_bone(bone_base_pos, game::bone_pos_chest), w2s)) {
			renderer::draw_text("target", w2s, 18, { 255, 7, 58, 255 });
		}

		auto fov_multiplier = 120.0 / game::get_fov();
		auto tickrate_multiplier = 1.0;
		auto speed_multiplier = 2;
		auto scale = 1.0 / 2.5;

		auto multiplier = fov_multiplier * tickrate_multiplier * speed_multiplier * (powf(4, 1) / 4.0);

		auto scaled_delta = target_angles * (multiplier * scale);

		auto dx_f = -scaled_delta.y;
		auto dy_f = scaled_delta.x;

		dx_f = powf(abs(dx_f), 1) * (dx_f / abs(dx_f));
		dy_f = powf(abs(dy_f), 1) * (dy_f / abs(dy_f));

		INPUT input = {};
		input.type = INPUT_MOUSE;
		input.mi.dx = dx_f;
		input.mi.dy = dy_f;
		input.mi.dwFlags = MOUSEEVENTF_MOVE;
		utils::send_input(input);
	}
}