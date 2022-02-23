#include "Encoder.h"
#include <iostream>

namespace mk {

	Encoder::Encoder(std::string input, bool single)
		: m_input{ std::move(input) }
	{
		// In case the file is too small to make use of all threads.
		// (ex. a 1 byte file split by 4 cores)
		if ((m_size / m_encode_threads) < 1) {
			m_max_threads = static_cast<int>(m_size + 1);
			m_encode_threads = m_max_threads > 2 ? m_max_threads - 1 : 1;
		}

		if (single) {
			m_max_threads = 1;
			m_encode_threads = 1;
		}

		if (m_max_threads > 1) {
			m_threads.reserve(m_max_threads);
			// One thread splits and pushes to queue, while remaining threads encode.
			m_threads.push_back(std::jthread(&Encoder::split, this));
			for (unsigned int i = 0; i < m_encode_threads; ++i) {
				m_threads.push_back(std::jthread(&Encoder::encode, this));
			}
			for (int i = 0; i < m_threads.size(); ++i) {
				m_threads[i].join();
			}
			stitch();
		}
		else { // If a single thread is detected, skip thread creation.
			split();
			encode();
		}
	};

	void Encoder::split()
	{
		const auto chunk = m_size / m_encode_threads;
		const auto first_chunk = chunk + (m_size % m_encode_threads);
		auto chunk_count = 0;

		std::cout << "Chunk size: " << chunk << '\n';
		std::cout << "Reserving chunks: " << m_encode_threads << '\n';

		m_nums.reserve(m_encode_threads);
		m_chars.reserve(m_encode_threads);

		const auto size_kib = std::round(m_size / 1024.0);

		if (m_size <= 1024) {
			std::cout << m_size << " byte(s)\n";
		}
		else {
			std::cout << m_size / 1000 << " KB\n";
			std::cout << size_kib << " KiB (Windows Explorer)\n";
		}

		size_t i = 0;
		while (i < m_size) {
			const size_t length = i == 0 ? first_chunk : chunk;
			std::string str = m_input.substr(i, length);

			work.push(std::pair(chunk_count, str));
			m_work_available_flag.release();

			chunk_count++;
			i += i == 0 ? first_chunk : chunk;
		}
		std::cout << "Chunks pushed to queue: " << chunk_count << std::endl;
	}

	void Encoder::encode()
	{

		std::string s{ "" };
		auto index = 0;
		{
			std::lock_guard guard{ m_work_lock };
			m_work_available_flag.acquire();
			std::pair<int, std::string> pair = work.front();
			std::tie(index, s) = pair;
			// Structured binding: https://en.cppreference.com/w/cpp/language/structured_binding
			// const auto [index, s] = work.front();
			work.pop();
		}

		auto count = 0;
		auto tmp = s[0];
		std::vector<int> tmp_num_vec{};
		std::vector<char> tmp_char_vec{};
		tmp_num_vec.clear();
		tmp_char_vec.clear();

		const auto chunk_length = s.length();
		constexpr auto ratio = 0.25;
		const auto chunk_reserve = static_cast<std::size_t>(chunk_length * ratio);
		tmp_num_vec.reserve(chunk_reserve);
		tmp_char_vec.reserve(chunk_reserve);

		for (int i = 0; i < chunk_length; ++i) {
			if (tmp == s[i]) {
				count++;
			}
			else {
				tmp_num_vec.push_back(count);
				tmp_char_vec.push_back(tmp);
				tmp = s[i];
				count = 1;
			}
		}
		{
			tmp_num_vec.push_back(count);
			tmp_char_vec.push_back(tmp);
			count = 0;
		}
		{
			std::lock_guard guard{ m_work_lock };
			m_nums.insert({ index, tmp_num_vec });
			m_chars.insert({ index, tmp_char_vec });
		}

	}

	void Encoder::stitch()
	{
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
	}

	std::unordered_map<int, std::vector<char>> Encoder::get_chars()
	{
		return m_chars;
	}

	std::unordered_map<int, std::vector<int>> Encoder::get_nums()
	{
		return m_nums;
	}
}