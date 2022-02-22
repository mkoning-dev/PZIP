#include <chrono>
#include "Encoder.h"
#include <filesystem>
#include <fstream>
#include <iostream>

void print_duration(std::chrono::steady_clock::time_point start,
	std::chrono::steady_clock::time_point end)
{
	const std::chrono::duration<double, std::milli> elapsed_ms = end - start;
	std::cout << "Elapsed time: " << elapsed_ms.count() << " ms\n";
}

std::string read_file(std::string input_filename)
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
		return 1;
	}
	std::string operation(argv[1]);
	std::string filename(argv[2]);

	std::string input = read_file(filename);
	if (input.empty()) {
		std::cout << "The provided file is empty." << std::endl;
		return 1;
	}

	if (operation == "-e" || operation == "--encode") {
		const auto start = std::chrono::steady_clock::now();

		mk::Encoder encode{ input , filename };

		const auto end = std::chrono::steady_clock::now();
		print_duration(start, end);
	}
	else if (operation == "-d" || operation == "--decode") {
		decode(filename);
	}
	else {
		print_help();
		return 1;
	}

}