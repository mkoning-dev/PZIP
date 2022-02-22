#pragma once

#include <unordered_map>

namespace mk {
	class File_IO {
	public:
		std::string read_file(std::string input_filename);
		void write_file(std::string filename, std::unordered_map<int, std::vector<char>> chars,
			std::unordered_map<int, std::vector<int>> nums);
	};
}