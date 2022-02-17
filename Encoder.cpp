module;

#include <filesystem>
#include <fstream>
#include <gsl/gsl-lite.hpp>
#include <iostream>
#include <mutex>

module Encoder;

namespace mk {
	Encoder::Encoder(std::string f) : m_input_filename{ f }
	{
		m_threads.reserve(m_max_threads);

		m_threads.push_back(std::jthread(&Encoder::read_file, this));
		for (unsigned int i = 0; i < m_encode_threads; ++i) {
			m_threads.push_back(std::jthread(&Encoder::encode, this));
		}
		for (int i = 0; i < m_threads.size(); ++i) {
			gsl::at(m_threads, i).join();
		}
		stitch_and_write();
	};

	void Encoder::read_file()
	{
		std::cout << "Reading file..." << std::endl;

		if (auto input = std::ifstream{ m_input_filename, std::ios::binary })
		{
			input >> std::noskipws; // Ensure no white space (space, newlines, etc.) is skipped.
			auto path = m_input_filename;
			auto size = std::filesystem::file_size(path);

			auto chunk = size / m_encode_threads;

			if (size < m_encode_threads) { // In case of a very small file.
				chunk = size;
			}

			const auto num_chunks = size % chunk == 0 ? size / chunk : (size / chunk) + 1;
			auto chunk_count = 0;
			std::cout << "Chunk size: " << chunk << std::endl;
			std::cout << "Reserving chunks: " << num_chunks << std::endl;

			m_nums.reserve(num_chunks);
			m_chars.reserve(num_chunks);

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
				std::format("Failed to open \"{}\".\n", m_input_filename)
			};
		}
	}

	void Encoder::encode()
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
				m_nums.insert({ index, tmp_num_vec });
				m_chars.insert({ index, tmp_char_vec });
			}
		}

	}

	void Encoder::stitch_and_write()
	{
		std::cout << "\nWriting file..." << std::endl;

		auto output_filename = m_input_filename.append(".z");
		if (auto output = std::fstream(output_filename, std::ios::out | std::ios::binary)) {

			// Stitch partial character runs back together.
			for (int i = 0; i < m_nums.size(); ++i) {
				if (i < m_nums.size() - 1) {

					if (m_chars[i].back() == m_chars[i + 1].front()) {
						auto num = m_nums[i].back();
						m_nums[i + 1].front() = m_nums[i + 1].front() + num;

						m_nums[i].pop_back();
						m_chars[i].pop_back();
					}
				}
			}

			// Write to file.
			for (int i = 0; i < m_nums.size(); ++i) {

				std::vector<int> tmp_nums = { m_nums[i] };
				std::vector<char> tmp_chars = { m_chars[i] };

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
}