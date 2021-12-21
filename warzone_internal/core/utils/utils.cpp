#include "utils.h"
#include "../imports/lazy_importer.h"
#include <windows.h>
#include <vector>

namespace utils {
	std::ofstream* log_file;

	std::string log_path;

	void log_init(std::string file_path) {
		log_file = new std::ofstream();
		utils::log_path.assign(file_path);
	}

	bool is_bad_ptr(uintptr_t* ptr) {
		if (!ptr || (uint64_t)ptr > 0xffffffffffff)
			return true;

		return false;
	}

	uintptr_t pattern_scan(uintptr_t module_base, const char* signature) {
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector < int >{};
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

		auto dosHeader = (PIMAGE_DOS_HEADER)module_base;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((uint8_t*)module_base + dosHeader->e_lfanew);

		auto size_of_image = ntHeaders->OptionalHeader.SizeOfImage;
		auto pattern_bytes = pattern_to_byte(signature);
		auto module_bytes = reinterpret_cast<uint8_t*>(module_base);

		auto s = pattern_bytes.size();
		auto d = pattern_bytes.data();

		for (auto i = 0ul; i < size_of_image - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (module_bytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return (uintptr_t)&module_bytes[i];
			}
		}
		return NULL;
	}
}