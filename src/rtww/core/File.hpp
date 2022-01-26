#pragma once

#include "core/Core.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::experimental::filesystem;

class RTWWFile {
public:
	static bool Write2File(const char* filePath, const char* buffer, int length);
};

bool RTWWFile::Write2File(const char* filePath, const char* buffer, int length) {
	std::ofstream out(filePath,std::ios::app);
	if (!out) {
		std::cerr << "Fail to open " << filePath  << '\n' << std::flush;
		return false;
	}
	for (int i = 0; i < length; ++i)
		out << buffer[i];
	out.close();
	return true;
}