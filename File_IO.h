#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace mk {
	class File_IO {
	public:
		static std::string read_file(std::string input_filename);
		static void write_file(std::string filename, std::unordered_map<int, std::vector<char>> chars,
			std::unordered_map<int, std::vector<int>> nums);
	};
}