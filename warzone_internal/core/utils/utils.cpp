#include "utils.h"
#include <windows.h>
#include <vector>

namespace utils {
	uintptr_t rva(uintptr_t instruction_address, int instruction_size) {
		return instruction_address + instruction_size + (uintptr_t) * (int*)(instruction_address + (instruction_size - sizeof(int)));
	}

	/*uint8_t* pattern_scan(void* module, const char* signature) {
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		auto dos_headers = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
		auto nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>((std::uint8_t*)module + dos_headers->e_lfanew);

		auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
		auto pattern_bytes = pattern_to_byte(signature);
		auto scan_bytes = reinterpret_cast<std::uint8_t*>(module);

		auto s = pattern_bytes.size();
		auto d = pattern_bytes.data();

		for (auto i = 0ul; i < size_of_image - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scan_bytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scan_bytes[i];
			}
		}
		return nullptr;
	}*/

	bool is_bad_ptr(uintptr_t* ptr) {
		if (!ptr || (uint64_t)ptr > 0xffffffffffff)
			return true;

		return false;
	}
}