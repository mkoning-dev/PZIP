#include "Encoder.h"
#include "Decoder.h"
#include "File_IO.h"
#include <chrono>

void print_duration(std::chrono::steady_clock::time_point start,
	std::chrono::steady_clock::time_point end)
{
	const std::chrono::duration<double, std::milli> elapsed_ms = end - start;
	std::cout << "Elapsed time: " << elapsed_ms.count() << " ms\n";
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

	if (operation == "-e" || operation == "--encode" || operation == "-e1") {
		mk::File_IO file{};

		std::string input = file.read_file(filename);
		if (input.empty()) {
			std::cout << "The provided file is empty. Nothing to encode." << std::endl;
			return 1;
		}

		bool single_thread = false;
		if (operation == "-e1") {
			single_thread = true;
		}

		std::cout << "Encoding..." << std::endl;

		const auto start = std::chrono::steady_clock::now();
		mk::Encoder encode{ input , filename, single_thread };
		const auto end = std::chrono::steady_clock::now();
		print_duration(start, end);

		std::unordered_map<int, std::vector<char>> chars = encode.get_chars();
		std::unordered_map<int, std::vector<int>> nums = encode.get_nums();
		file.write_file(filename, chars, nums);
	}
	else if (operation == "-d" || operation == "--decode") {
		mk::decode(filename);
	}
	else {
		print_help();
		return 1;
	}
}