#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <gsl/gsl-lite.hpp>
#include <iostream>
#include <locale>
#include <mutex>
#include <queue>
#include <semaphore>
#include <thread>
#include <unordered_map>
#include <vector>

// Global
auto work_lock = std::mutex{};
auto work = std::queue<std::tuple<int, std::string>>{};
auto work_available_flag = std::counting_semaphore{ 0 };
std::unordered_map<int, std::vector<char>> chars;
std::unordered_map<int, std::vector<int>> nums;

void print_duration(std::chrono::steady_clock::time_point start,
	std::chrono::steady_clock::time_point end)
{
	const std::chrono::duration<double, std::milli> elapsed_ms = end - start;
	std::cout << "Elapsed time: " << elapsed_ms.count() << " ms\n";
}

void read_file(unsigned int threads, std::string& input_filename)
{
	std::cout << "Reading file..." << std::endl;

	//auto input_filename = "6.txt";

	if (auto input = std::ifstream{ input_filename, std::ios::binary })
	{
		input >> std::noskipws; // Ensure no white space (space, newlines, etc.) is skipped.
		auto path = input_filename;
		auto size = std::filesystem::file_size(path);

		auto chunk = size / threads;

		if (size < threads) { // In case of a very small file.
			chunk = size;
		}

		const auto num_chunks = size % chunk == 0 ? size / chunk : (size / chunk) + 1;
		auto chunk_count = 0;
		std::cout << "Chunk size: " << chunk << std::endl;
		std::cout << "Reserving chunks: " << num_chunks << std::endl;

		nums.reserve(num_chunks);
		chars.reserve(num_chunks);

		const auto size_kib = std::round(size / 1024.0);

		if (size <= 1024) {
			std::cout << size << " byte(s)" << std::endl;
		}
		else {
			std::cout << size / 1000 << " KB" << std::endl;
			std::cout << size_kib << " KiB (Windows Explorer)" << std::endl;
		}

		while (input) {
			if (size < chunk) {
				chunk = size;
			}

			std::string str(chunk, '\0'); // construct string to stream size
			if (input.read(&str[0], chunk)) {
				if (!str.empty()) {
					work.push(std::tuple(chunk_count, str));
					work_available_flag.release();
					chunk_count++;
				}
			}

			if (chunk == 0) {
				input.read(&str[0], 2);
			}
			size -= chunk;
		}
		std::cout << "Chunks pushed to queue: " << chunk_count << "\n";
	}
	else
	{
		throw std::runtime_error{
			std::format("Failed to open \"{}\".\n", input_filename)
		};
	}
}

void encode()
{
	work_available_flag.acquire();
	while (!work.empty()) {

		thread_local std::string s{ "" };
		thread_local auto index = 0;
		{
			std::lock_guard guard{ work_lock };
			if (!work.empty()) {
				std::tuple<int, std::string> tuple = work.front();
				std::tie(index, s) = tuple;
				work.pop();
			}
			else {
				continue;
			}
		}

		thread_local auto count = 0;
		thread_local auto tmp = gsl::at(s, 0);
		thread_local std::vector<int> tmp_num_vec{};
		thread_local std::vector<char> tmp_char_vec{};
		tmp_num_vec.clear();
		tmp_char_vec.clear();

		for (int i = 0; i < s.length(); ++i) {
			if (tmp == gsl::at(s, i)) {
				count++;
			}
			else {
				tmp_num_vec.push_back(count);
				tmp_char_vec.push_back(tmp);
				tmp = gsl::at(s, i);
				count = 1;
			}
		}
		{
			tmp_num_vec.push_back(count);
			tmp_char_vec.push_back(tmp);
			count = 0;
		}
		{
			std::lock_guard guard{ work_lock };
			nums.insert({ index, tmp_num_vec });
			chars.insert({ index, tmp_char_vec });
		}
	}

}

void stitch_and_write(std::string& filename)
{
	std::cout << "\nWriting file..." << std::endl;

	auto output_filename = filename.append(".z");
	if (auto output = std::fstream(output_filename, std::ios::out | std::ios::binary)) {

		// Stitch pieces back together.
		for (int i = 0; i < nums.size(); ++i) {
			if (i < nums.size() - 1) {

				if (chars[i].back() == chars[i + 1].front()) {
					auto num = nums[i].back();
					nums[i + 1].front() = nums[i + 1].front() + num;

					nums[i].pop_back();
					chars[i].pop_back();
				}
			}
		}

		// Write to file.
		for (int i = 0; i < nums.size(); ++i) {

			std::vector<int> tmp_nums = { nums[i] };
			std::vector<char> tmp_chars = { chars[i] };

			for (int j = 0; j < tmp_nums.size(); ++j) {
				output.write(reinterpret_cast<char*>(&gsl::at(tmp_nums, j)), sizeof(int));
				output.write(reinterpret_cast<char*>(&gsl::at(tmp_chars, j)), sizeof(char));

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
				//std::cout << tmp;
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
		<< "    -e to encode the specified file.\n"
		<< "    -d to decode the specified file." << std::endl;
}

int main(int argc, char** argv)
{

	if (argc != 3) {
		print_help();
		exit(1);
	}
	std::string operation(argv[1]);
	std::string filename(argv[2]);

	if (operation == "-e") {
		// Encode
		const auto start = std::chrono::steady_clock::now();
		{
			const auto max_threads = std::thread::hardware_concurrency();
			const auto encode_threads = (max_threads > 1 ? max_threads - 1 : 1);

			std::vector<std::jthread> threads;
			threads.reserve(max_threads);

			threads.push_back(std::jthread(read_file, encode_threads, std::ref(filename)));
			for (unsigned int i = 0; i < encode_threads; ++i) {
				threads.push_back(std::jthread(encode));
			}
		}
		const auto end = std::chrono::steady_clock::now();
		print_duration(start, end);

		stitch_and_write(filename);
	}
	else if (operation == "-d") {
		// Decode
		decode(filename);
	}
	else {
		// Return error.
		print_help();
		exit(1);
	}

}