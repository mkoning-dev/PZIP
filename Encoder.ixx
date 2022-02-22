module;

#include <mutex>
#include <queue>
#include <semaphore>
#include <thread>
#include <unordered_map>
#include <vector>

export module Encoder;

namespace mk {
	export class Encoder {
	public:
		Encoder(std::string, std::string);
		void read_file();
		void split();
		void encode();
		void stitch();
		void write_file();

	private:
		// User provided filename.
		std::string m_input_filename{};
		std::string m_input{};
		const size_t m_size = m_input.size();

		// Store characters and integers to be written to encoded file.
		std::unordered_map<int, std::vector<char>> m_chars;
		std::unordered_map<int, std::vector<int>> m_nums;

		// Stores chunks to be processed by threads.
		std::queue<std::tuple<int, std::string>> work;

		// Thread related members.
		unsigned int m_max_threads = std::thread::hardware_concurrency();
		unsigned int m_encode_threads = m_max_threads > 2 ? m_max_threads - 1 : 1;
		std::mutex m_work_lock{};
		std::counting_semaphore<LLONG_MAX> m_work_available_flag = std::counting_semaphore{ 0 };
		std::vector<std::jthread> m_threads;
	};
}