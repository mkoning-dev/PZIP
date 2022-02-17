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
		Encoder(std::string f);
		void read_file();
		void encode();
		void stitch_and_write();

	private:
		// User provided filename.
		std::string m_input_filename{};

		// Store characters and integers to be written to encoded file.
		std::unordered_map<int, std::vector<char>> m_chars;
		std::unordered_map<int, std::vector<int>> m_nums;

		// Stores chunks to be processed by threads.
		std::queue<std::tuple<int, std::string>> work;

		// Thread related members.
		const unsigned int m_max_threads = std::thread::hardware_concurrency();
		const unsigned int m_encode_threads = (m_max_threads > 1 ? m_max_threads - 1 : 1);
		std::mutex work_lock{};
		std::counting_semaphore<LLONG_MAX> work_available_flag = std::counting_semaphore{ 0 };
		std::vector<std::jthread> m_threads;
	};
}