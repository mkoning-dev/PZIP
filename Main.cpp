#include "Decoder.h"
#include "Encoder.h"
#include "File_IO.h"
#include <chrono>

double duration_ms(std::chrono::steady_clock::time_point start,
	std::chrono::steady_clock::time_point end) noexcept
{
	const std::chrono::duration<double, std::milli> elapsed_ms = end - start;
	return elapsed_ms.count();
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

		std::string input = mk::File_IO::read_file(filename);
		if (input.empty()) {
			std::cout << "The provided file is empty. Nothing to encode." << std::endl;
			return 1;
		}

		bool single_thread = false;
		if (operation == "-e1") {
			single_thread = true;
		}

		std::cout << "\nEncoding..." << std::endl;

		const auto start = std::chrono::steady_clock::now();
		mk::Encoder encode{ input , single_thread };
		const auto end = std::chrono::steady_clock::now();
		std::cout << "Encode time: " << duration_ms(start, end)
			<< " ms" << std::endl;

		std::unordered_map<int, std::vector<char>> chars = encode.get_chars();
		std::unordered_map<int, std::vector<int>> nums = encode.get_nums();
		mk::File_IO::write_file(filename, chars, nums);
	}
	else if (operation == "-d" || operation == "--decode") {
		mk::decode(filename);
	}
	else {
		print_help();
		return 1;
	}
}