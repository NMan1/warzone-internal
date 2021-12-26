#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
#include <stdarg.h>
#include <fstream>

#define M_PI 3.14159265358979323846

namespace utils {
	extern std::ofstream* log_file;

	extern std::string log_path;

	void log_init(std::string file_path);

	inline std::string get_time() {
		time_t now = time(0);
		struct tm  tstruct;
		char  buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%X", &tstruct);
		return std::string(buf);
	};

	template<typename T>
	void log(T const& v) {
		utils::log_file->open("D:\\overflow.log", std::ios_base::app);
		*utils::log_file << get_time() << " [-] " << v << "\n";
		utils::log_file->close();
	}
	
	template<typename Arg, typename ...Args>
	void log(Arg const& arg, Args const&... args) {
		log(arg);
		log(args...);
	}

	bool is_bad_ptr(uintptr_t* ptr);

	uintptr_t pattern_scan(uintptr_t module_base, const char* signature);

	bool get_cursor_pos(LPPOINT point);

	HCURSOR set_cursor(HCURSOR cursor);

	ULONG send_input(INPUT input);
}