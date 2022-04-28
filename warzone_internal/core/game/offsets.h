#pragma once

namespace offsets {
    constexpr auto ref_def_ptr = 0x1EC18A70;
    constexpr auto name_array = 0x1EC24CD0;
    constexpr auto name_array_list = 0x4C70;
    constexpr auto camera_base = 0x1B4D27B0;
    constexpr auto camera_pos = 0x1D8;
    constexpr auto local_index = 0x68C8; 
    constexpr auto local_index_pos = 0x1FC;
    constexpr auto recoil = 0x19B28;
    constexpr auto game_mode = 0x1EC510F8;
    constexpr auto weapon_definitions = 0xFFFF8009E2254927;
    constexpr auto distribute = 0x212F9348;
    constexpr auto visible_offset = 0xA83;
    constexpr auto visible = 0x62C0E20;
    namespace player {
        constexpr auto size = 0x6060;
        constexpr auto valid = 0x1C;
        constexpr auto pos = 0x820;
        constexpr auto team = 0x24;
        constexpr auto stance = 0xC58;
        constexpr auto weapon_index = 0x0;// broken sig
        constexpr auto dead_1 = 0xA48;
        constexpr auto dead_2 = 0x608;
    }
    namespace bone {
        constexpr auto bone_base = 0x6BCC;
        constexpr auto size = 0x150;
    }
}
