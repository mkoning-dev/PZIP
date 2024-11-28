#include <algorithm>
#include <gtest/gtest.h>
#include <numeric>
#include "Encoder.h"

namespace mk
{

std::vector<int> flatten_nums(const std::unordered_map<int, std::vector<int>>& nums) {
    std::vector<int> flat_nums;
    std::vector<int> keys;
    for (const auto& [key, _] : nums) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());

    for (const auto& key : keys) {
        flat_nums.insert(flat_nums.end(), nums.at(key).begin(), nums.at(key).end());
    }
    return flat_nums;
}

std::vector<char> flatten_chars(const std::unordered_map<int, std::vector<char>>& chars) {
    std::vector<char> flat_chars;
    std::vector<int> keys;
    for (const auto& [key, _] : chars) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());

    for (const auto& key : keys) {
        flat_chars.insert(flat_chars.end(), chars.at(key).begin(), chars.at(key).end());
    }
    return flat_chars;
}

    // Test that Encoder correctly encodes a simple string
    TEST(EncoderTest, EncodeSimpleString)
    {
        std::string input = "aaabbbccc";
        Encoder encoder(input, true); // Single thread for simplicity

        auto nums = encoder.get_nums();
        auto chars = encoder.get_chars();

        ASSERT_EQ(nums[0], std::vector<int>({3, 3, 3}));
        ASSERT_EQ(chars[0], std::vector<char>({'a', 'b', 'c'}));
    }

    // Test that the Encoder correctly handles an empty string
    TEST(EncoderTest, EncodeEmptyString)
    {
        std::string input = "";
        Encoder encoder(input, true); // Single thread for simplicity

        auto nums = encoder.get_nums();
        auto chars = encoder.get_chars();

        ASSERT_TRUE(nums.empty());
        ASSERT_TRUE(chars.empty());
    }

    // Test thread safety by encoding multiple inputs concurrently
    TEST(EncoderTest, ConcurrentEncoding) {
        std::atomic<int> success_count(0);
        std::vector<std::string> inputs = {"aaabbbccc", "xyzxyzxyz", "123123123"};

        auto encoding_task = [&success_count](const std::string& input) {
            Encoder encoder(input, false);  // Enable multithreading
            try {
                encoder.encode();
                success_count++;
            } catch (const std::exception& e) {
                // Handle failure
            }
        };

        std::vector<std::thread> threads;
        for (const auto& input : inputs) {
            threads.push_back(std::thread(encoding_task, input));
        }

        for (auto& t : threads) {
            t.join();
        }

        // Ensure all tasks completed successfully
        ASSERT_EQ(success_count, inputs.size());
    }

    TEST(EncoderTest, SplittingAccuracy)
    {
        std::string input = "aaaabbbbccccdddd"; // 16 characters
        Encoder encoder(input, false);          // Multithreaded
        encoder.encode();                       // Ensure chunks are created

        // Get flattened arrays
        const auto &nums = flatten_nums(encoder.get_nums());
        const auto &chars = flatten_chars(encoder.get_chars());

        // Verify that the number of chunks matches across nums and chars
        const int actual_chunks = nums.size();
        ASSERT_EQ(chars.size(), actual_chunks);

        // Verify the total length sums to the input size
        int total_length = 0;
        for (int i = 0; i < nums.size(); ++i)
        {
            total_length += nums[i];
        }
        ASSERT_EQ(total_length, input.size());

        // Verify all characters are present in order (since it's a flattened array)
        std::string reconstructed;
        for (int i = 0; i < nums.size(); ++i)
        {
            reconstructed.append(nums[i], chars[i]);
        }
        ASSERT_EQ(reconstructed, input);
    }

    // Test large input encoding
    TEST(EncoderTest, LargeInputEncoding)
    {
        std::string input(1'000'000, 'a');  // 1 million 'a' characters
        input += std::string(500'000, 'b'); // 500k 'b' characters

        Encoder encoder(input, false); // Multithreaded
        encoder.encode();              // Ensure encoding is performed

        // Get flattened arrays
        const auto &nums = flatten_nums(encoder.get_nums());
        const auto &chars = flatten_chars(encoder.get_chars());

        // Ensure the size of nums and chars matches
        ASSERT_EQ(nums.size(), chars.size());

        // Verify the structure of the flattened arrays
        int total_a_count = 0, total_b_count = 0;
        for (size_t i = 0; i < nums.size(); ++i)
        {
            if (chars[i] == 'a')
            {
                total_a_count += nums[i];
            }
            else if (chars[i] == 'b')
            {
                total_b_count += nums[i];
            }
            else
            {
                FAIL() << "Unexpected character in encoding: " << chars[i];
            }
        }

        // Verify the total counts match the input
        ASSERT_EQ(total_a_count, 1'000'000);
        ASSERT_EQ(total_b_count, 500'000);
    }

    // Test concurrent access to encoded data
    TEST(EncoderTest, ConcurrentAccess)
    {
        std::string input = "aaabbbcccddd";
        Encoder encoder(input, false); // Multithreaded

        std::atomic<int> access_count(0);
        auto access_task = [&encoder, &access_count]()
        {
            auto nums = flatten_nums(encoder.get_nums());
    auto chars = flatten_chars(encoder.get_chars());
            if (!nums.empty() && !chars.empty())
            {
                access_count++;
            }
        };

        std::vector<std::thread> threads;
        for (int i = 0; i < 10; ++i)
        { // Simulate multiple access attempts
            threads.push_back(std::thread(access_task));
        }

        for (auto &t : threads)
        {
            t.join();
        }

        ASSERT_EQ(access_count, 10); // All threads should successfully access data
    }

    // Test encoding input with all identical characters
    TEST(EncoderTest, AllIdenticalCharacters)
    {
        std::string input(100, 'x');   // 100 'x' characters
        Encoder encoder(input, false); // Multithreaded

        auto nums = flatten_nums(encoder.get_nums());
    auto chars = flatten_chars(encoder.get_chars());

        ASSERT_EQ(nums, std::vector<int>({100}));
        ASSERT_EQ(chars, std::vector<char>({'x'}));
    }

    // Test encoding alternating characters
    TEST(EncoderTest, AlternatingCharacters)
    {
        std::string input = "abababababab";
        Encoder encoder(input, false); // Multithreaded

        // auto nums = encoder.get_nums();
        // auto chars = encoder.get_chars();

        // Flatten the results
    auto flat_nums = flatten_nums(encoder.get_nums());
    auto flat_chars = flatten_chars(encoder.get_chars());

        ASSERT_EQ(flat_nums, std::vector<int>({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}));
        ASSERT_EQ(flat_chars, std::vector<char>({'a', 'b', 'a', 'b', 'a', 'b', 'a', 'b', 'a', 'b', 'a', 'b'}));
    }

    // Test that adjacent chunks are stitched together correctly
    TEST(EncoderTest, StitchingLogic)
    {
        std::string input = "aaaabbbbccccddd";
        Encoder encoder(input, false); // Multithreaded

        auto nums = flatten_nums(encoder.get_nums());
    auto chars = flatten_chars(encoder.get_chars());

        // After stitching, we should see a single contiguous encoding
        ASSERT_EQ(nums, std::vector<int>({4, 4, 4, 3}));
        ASSERT_EQ(chars, std::vector<char>({'a', 'b', 'c', 'd'}));
    }

} // namespace mk
