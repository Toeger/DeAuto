#include "utility.h"

#include <fstream>

bool Utility::copy_file(boost::filesystem::path from, boost::filesystem::path to) {
	//TODO: Once C++17 is available switch to std::copy_file
	std::ifstream input{from.c_str()};
	std::ofstream output{to.c_str()};
	output << input.rdbuf();
	return static_cast<bool>(output);
}
