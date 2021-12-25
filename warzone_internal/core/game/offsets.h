#pragma once

namespace offsets {
    constexpr auto ref_def_ptr = 0x1F0D6320;
    constexpr auto name_array = 0x1F0E20B0;
    constexpr auto name_array_list = 0x4C70;
    constexpr auto camera_base = 0x1BA41910;
    constexpr auto camera_pos = 0x1D8;
    constexpr auto local_index = 0x520E0; 
    constexpr auto local_index_pos = 0x1FC;
    constexpr auto recoil = 0x7CEB4; // broken sig
    constexpr auto game_mode = 0x1EC510F8;
    constexpr auto weapon_definitions = 0xFFFF8009E2254927;
    constexpr auto distribute = 0x217AB2F8;
    constexpr auto visible_offset = 0xA83;
    constexpr auto visible = 0x680B830;
    namespace player {
        constexpr auto size = 0x5ED8;
        constexpr auto valid = 0x4CC;
        constexpr auto pos = 0x5D48;
        constexpr auto team = 0x9D0;
        constexpr auto stance = 0x5248;
        constexpr auto weapon_index = 0x0;// broken sig
        constexpr auto dead_1 = 0x5BA4;
        constexpr auto dead_2 = 0x5D64;
    }
    namespace bone {
        constexpr auto bone_base = 0x225EC;
        constexpr auto size = 0x150;
    }
}