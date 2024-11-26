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

} // namespace mk
