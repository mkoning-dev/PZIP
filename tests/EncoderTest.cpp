#include <gtest/gtest.h>
#include "Encoder.h"

namespace mk
{

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

} // namespace mk
