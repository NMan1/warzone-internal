#include "features.h"
#include "../game.h"
#include "../offsets.h"
#include "../../renderer/renderer.h"
#include "../../utils/globals.h"

namespace features {
	void esp() {
		auto local = game::get_local();
		auto local_pos = local.get_origin_position();
		auto local_team = local.team_id();
		auto bone_base_pos = game::get_bone_base_pos(game::client_info);

		for (int i = 0; i < game::player_count(); i++) {
			game::player_t player(game::client_info_base + (i * offsets::player::size), i);
			if (!player.is_valid() || player.dead()) {
				continue;
			}

			if (player.team_id() == local_team) {
				continue;
			}

			vec2_t screen_origin, screen_head; vec3_t origin_pos = player.get_origin_position();
			if (game::w2s(origin_pos, screen_origin)) {
				auto dist = (int)game::units_to_m(local_pos.distance_to(origin_pos));
				if (dist < globals::settings::max_render_distance) {
					//auto chest = player.get_bone(bone_base_pos, game::bone_pos_neck);
					//if (game::units_to_m(local_pos.distance_to(chest)) < 200) {
					//	vec2_t screen_cheat{};
					//	if (game::w2s(player.get_bone(bone_base_pos, game::bone_pos_chest), screen_cheat)) {
					//		renderer::draw_circle(screen_cheat, 5, { 255, 7, 58, 255 }, 1, 32);
					//	}
					//}

					auto head_pos = player.get_head_position(origin_pos);
					head_pos += {0, 0, 10};

					if (game::w2s(head_pos, screen_head)) {
						vec2_t top_left, bottom_right;
						player.get_box(screen_origin, screen_head, top_left, bottom_right);

						if (globals::settings::box_border)
							renderer::draw_rect(top_left, bottom_right, globals::settings::box_border_clr, 0, 1);

						if (globals::settings::box_filled)
							renderer::draw_rect_filled({ top_left.x + 1, top_left.y + 1 }, { bottom_right.x - 1, bottom_right.y - 1 }, globals::settings::box_filled_clr, 0);
					}

					if (globals::settings::snap_lines)
						renderer::draw_line({ (float)game::ref_def.width / 2, (float)game::ref_def.height }, { screen_origin.x, screen_origin.y + 2 }, globals::settings::snap_lines_clr , 1.5);

					if (globals::settings::distance_text) {
						char buf[6];
						sprintf(buf, "[%d]m", dist);
						renderer::draw_text(buf, { screen_origin.x, screen_origin.y + 2 }, 15, globals::settings::distance_text_clr, true);
					}
				}
			}
		}
	}
}