#include "File_IO.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace mk {

	std::string File_IO::read_file(std::string input_filename)
	{
		std::cout << "Reading file..." << std::endl;

		if (auto input = std::ifstream{ input_filename, std::ios::binary })
		{
			input >> std::noskipws; // Ensure no white space (space, newlines, etc.) is skipped.
			auto path = input_filename;
			auto size = std::filesystem::file_size(path);
			std::string str(size, '\0');

			input.read(&str[0], size);

			return str;
		}
		else
		{
			throw std::runtime_error{
				std::format("Failed to open \"{}\".\n", input_filename)
			};
		}
	}

	void File_IO::write_file(std::string filename, std::unordered_map<int, std::vector<char>> chars,
		std::unordered_map<int, std::vector<int>> nums)
	{
		std::cout << "\nWriting file..." << std::endl;

		auto output_filename = filename.append(".z");
		if (auto output = std::fstream(output_filename, std::ios::out | std::ios::binary)) {

			const auto outer_size = nums.size();
			for (int i = 0; i < outer_size; ++i) {

				std::vector<int> tmp_nums = { nums[i] };
				std::vector<char> tmp_chars = { chars[i] };
				const auto inner_size = tmp_nums.size();

				for (int j = 0; j < inner_size; ++j) {
					output.write(reinterpret_cast<char*>(&tmp_nums[j]), sizeof(int));
					output.write(reinterpret_cast<char*>(&tmp_chars[j]), sizeof(char));

				}
			}
			std::cout << "Encoded file: " << output_filename << std::endl;
		}
		else
		{
			throw std::runtime_error{
				std::format("Failed to open \"{}\".\n", output_filename)
			};
		}
	}
}