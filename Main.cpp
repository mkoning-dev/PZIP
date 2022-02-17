#include <chrono>
#include <fstream>
#include <iostream>

import Encoder;

void print_duration(std::chrono::steady_clock::time_point start,
	std::chrono::steady_clock::time_point end)
{
	const std::chrono::duration<double, std::milli> elapsed_ms = end - start;
	std::cout << "Elapsed time: " << elapsed_ms.count() << " ms\n";
}

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

void print_help()
{
	std::cout << "Usage: PZIP.exe <options> <filename>\n"
		<< "  options:\n"
		<< "    -e, --encode to encode the specified file.\n"
		<< "    -d, --decode to decode the specified file." << std::endl;
}

int main(int argc, char** argv)
{
	if (argc != 3) {
		print_help();
		exit(1);
	}
	std::string operation(argv[1]);
	std::string filename(argv[2]);

	if (operation == "-e" || operation == "--encode") {
		const auto start = std::chrono::steady_clock::now();

		mk::Encoder encode{ filename };

		const auto end = std::chrono::steady_clock::now();
		print_duration(start, end);
	}

	else if (operation == "-d" || operation == "--decode") {
		decode(filename);
	}
	else {
		print_help();
		exit(1);
	}
}