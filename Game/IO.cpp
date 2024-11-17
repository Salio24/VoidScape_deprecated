#include "IO.hpp"

std::string ReadFileAsString(const std::string& filePath) {
	std::ifstream file(filePath.c_str());

	if (!file.is_open()) {
		std::cout << "Failed to open file" << (filePath) << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}