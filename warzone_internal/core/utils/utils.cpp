#include "utils.h"
#include "../imports/lazy_importer.h"
#include <vector>
#include "../imports/syscall/syscall.h"

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

	// mutable thx, for sys call ids

	std::string get_version() {
		DWORD dwType = REG_SZ;
		HKEY hKey = 0;
		char value[1024];
		DWORD value_length = 1024;
		const char* subkey = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";
		RegOpenKeyA(HKEY_LOCAL_MACHINE, subkey, &hKey);
		const auto status = RegQueryValueExA(hKey, "DisplayVersion", NULL, &dwType, (LPBYTE)&value, &value_length);
		RegCloseKey(hKey);
		return std::string(value);
	}

	bool get_cursor_pos(LPPOINT point) {
		static auto version = get_version();
		int id = 4138;

		if (version == "1507") { 
			id = 4141; 
		}
		else if (version == "1511") { 
			id = 4141; 
		}
		else if (version == "1607") { 
			id = 4141; 
		}
		else if (version == "1703") {
			id = 4141; }
		else if (version == "1709") { 
			id = 4141;
		}
		else if (version == "1803") { 
			id = 4141; 
		}
		else if (version == "1809") { 
			id = 4141;
		}
		else if (version == "1903") { 
			id = 4141; 
		}
		else if (version == "1909") { 
			id = 4141; 
		}
		else if (version == "2004") {
			id = 4138; 
		}
		else if (version == "20H2") { 
			id = 4138; }
		else if (version == "21H2") { 
			id = 5149; 
		}

		return syscall<bool>(id, point, 1, 127);
	}

	HCURSOR set_cursor(HCURSOR cursor) {
		static auto version = get_version();
		int id = 4122;

		if (version == "1507") {
			id = 4125; 
		}
		else if (version == "1511") { 
			id = 4125; 
		}
		else if (version == "1607") { 
			id = 4125; 
		}
		else if (version == "1703") { 
			id = 4125; 
		}
		else if (version == "1709") { 
			id = 4125; 
		}
		else if (version == "1803") { 
			id = 4125;
		}
		else if (version == "1809") { 
			id = 4125; 
		}
		else if (version == "1903") {
			id = 4125;
		}
		else if (version == "1909") { 
			id = 4125; 
		}
		else if (version == "2004") {
			id = 4122;
		}
		else if (version == "20H2") { 
			id = 4122;
		}
		else if (version == "21H2") {
			id = 4120;
		}

		return syscall<HCURSOR>(id, cursor);
	}

	ULONG send_input(INPUT input) {
		static auto version = get_version();
		int id = 4223;

		if (version == "1507") {
			id = 4228;
		}
		else if (version == "1511") {
			id = 4228;
		}
		else if (version == "1607") {
			id = 4228;
		}
		else if (version == "1703") {
			id = 4226;
		}
		else if (version == "1709") {
			id = 4226;
		}
		else if (version == "1803") {
			id = 4226;
		}
		else if (version == "1809") {
			id = 4226;
		}
		else if (version == "1903") {
			id = 4226;
		}
		else if (version == "1909") {
			id = 4226;
		}
		else if (version == "2004") {
			id = 4223;
		}
		else if (version == "20H2") {
			id = 4223;
		}

		return syscall<ULONG>(id, 1, &input, sizeof(INPUT));
	}
}