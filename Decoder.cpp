#include "Decoder.h"
#include <fstream>
#include <format>

namespace mk {
	void decode(std::string input_filename)
	{
		std::cout << "Decoding..." << std::endl;

		std::string output_filename = "decoded.txt";
		if (auto input = std::fstream(input_filename, std::ios::in | std::ios::binary))
		{
			int count = 0;
			char tmp = '\0';
			auto output = std::fstream(output_filename, std::ios::out | std::ios::binary);

			while (true) {
				input.read(reinterpret_cast<char*>(&count), sizeof(int));
				input.read(reinterpret_cast<char*>(&tmp), sizeof(char));

				if (input.eof()) {
					break;
				}

				for (int j = 0; j < count; ++j) {
					output.write(reinterpret_cast<char*>(&tmp), sizeof(char));
				}
			}
			std::cout << "Decoded file: " << output_filename << std::endl;
		}
		else
		{
			throw std::runtime_error{
				std::format("Failed to open \"{}\".\n", input_filename)
			};
		}
	}
}