#include <RandomWordGenerator/Generator.h>
#include <algorithm>
#include <gtest/gtest.h>
#include <set>

class GenerationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        generator = std::make_unique<RandomWordGenerator>();
        rng.seed(12345); // Fixed seed for reproducible tests

        // Add some sample words for testing
        generator->analyzeWord("hello");
        generator->analyzeWord("world");
        generator->analyzeWord("test");
        generator->analyzeWord("word");
        generator->analyzeWord("generator");
        generator->finalize();
    }

    std::unique_ptr<RandomWordGenerator> generator;
    std::minstd_rand                     rng;
};

// Test basic word generation
TEST_F(GenerationTest, GenerateWords)
{
    for (int i = 0; i < 10; ++i)
    {
        std::string word = (*generator)(rng);
        EXPECT_FALSE(word.empty());
        EXPECT_GE(word.length(), 1u); // Default minLength is 1

        // Check that all characters are lowercase letters
        for (char c : word)
        {
            EXPECT_TRUE(c >= 'a' && c <= 'z') << "Invalid character: " << c;
        }
    }
}

// Test word generation with min and max length
TEST_F(GenerationTest, GenerateWordsWithMinAndMaxLength)
{
    for (size_t minLen = 1; minLen <= 5; ++minLen)
    {
        for (size_t maxLen = minLen; maxLen <= minLen + 5; ++maxLen)
        {
            std::string word = (*generator)(rng, minLen, maxLen);
            EXPECT_GE(word.length(), minLen) << "Word too short: '" << word << "' (expected >= " << minLen << ")";
            EXPECT_LE(word.length(), maxLen) << "Word too long: '" << word << "' (expected <= " << maxLen << ")";
            EXPECT_FALSE(word.empty());

            // Check that all characters are valid
            for (char c : word)
            {
                EXPECT_TRUE(c >= 'a' && c <= 'z');
            }
        }
    }
}

// Test word generation with only minimum length specified
TEST_F(GenerationTest, GenerateWordsWithMinLengthOnly)
{
    for (size_t minLen = 1; minLen <= 10; ++minLen)
    {
        std::string word = (*generator)(rng, minLen, 0); // 0 means unlimited max
        EXPECT_GE(word.length(), minLen) << "Word too short: '" << word << "' (expected >= " << minLen << ")";
        EXPECT_FALSE(word.empty());

        // Check that all characters are valid
        for (char c : word)
        {
            EXPECT_TRUE(c >= 'a' && c <= 'z');
        }
    }
}

// Test word generation with only max length specified (using default minLength = 1)
TEST_F(GenerationTest, GenerateWordsWithMaxLengthOnly)
{
    for (size_t maxLen = 1; maxLen <= 10; ++maxLen)
    {
        std::string word = (*generator)(rng, 1, maxLen); // Explicit minLength = 1
        EXPECT_GE(word.length(), 1u);
        EXPECT_LE(word.length(), maxLen);
        EXPECT_FALSE(word.empty());

        // Check that all characters are valid
        for (char c : word)
        {
            EXPECT_TRUE(c >= 'a' && c <= 'z');
        }
    }
}

// Test that different seeds produce different results
TEST_F(GenerationTest, DifferentSeedsProduceDifferentResults)
{
    std::minstd_rand rng1(12345);
    std::minstd_rand rng2(54321);

    std::set<std::string> words1, words2;

    // Generate words with different seeds
    for (int i = 0; i < 20; ++i)
    {
        words1.insert((*generator)(rng1));
        words2.insert((*generator)(rng2));
    }

    // Should have some different words (very high probability)
    // We check that they're not identical sets
    EXPECT_FALSE(words1 == words2);
}

// Test reproducibility with same seed
TEST_F(GenerationTest, SameSeedProducesReproducibleResults)
{
    std::minstd_rand rng1(12345);
    std::minstd_rand rng2(12345);

    std::vector<std::string> words1, words2;

    // Generate words with same seed
    for (int i = 0; i < 10; ++i)
    {
        words1.push_back((*generator)(rng1));
        words2.push_back((*generator)(rng2));
    }

    // Should be identical
    EXPECT_EQ(words1, words2);
}

// Test generation with single character training
TEST_F(GenerationTest, GenerationWithSingleCharacterTraining)
{
    RandomWordGenerator singleGen;
    singleGen.analyzeWord("a");
    singleGen.finalize();

    std::minstd_rand testRng(12345);
    std::string      word = singleGen(testRng);

    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);

    // All characters should be 'a' or the word should be just "a"
    for (char c : word)
    {
        EXPECT_EQ(c, 'a');
    }
}

// Test generation with repeated pattern
TEST_F(GenerationTest, GenerationWithRepeatedPattern)
{
    RandomWordGenerator patternGen;

    // Add the same word multiple times to increase its influence
    for (int i = 0; i < 100; ++i)
    {
        patternGen.analyzeWord("abab");
    }
    patternGen.finalize();

    std::minstd_rand testRng(12345);

    // Generate multiple words and check they contain expected patterns
    bool foundExpectedPattern = false;
    for (int i = 0; i < 20; ++i)
    {
        std::string word = patternGen(testRng);
        EXPECT_FALSE(word.empty());
        EXPECT_GE(word.length(), 1u);

        // The generated words should show influence of the "abab" pattern
        if (word.find("ab") != std::string::npos || word.find("ba") != std::string::npos)
        {
            foundExpectedPattern = true;
        }
    }

    // At least some words should contain the pattern
    EXPECT_TRUE(foundExpectedPattern);
}

// Test max length of 0 (unlimited)
TEST_F(GenerationTest, UnlimitedLength)
{
    std::string word = (*generator)(rng, 1, 0); // minLength=1, maxLength=0 (unlimited)
    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);

    // Should be reasonable length (not infinite)
    EXPECT_LT(word.length(), 100u); // Very generous upper bound
}

// Test that very large max length doesn't cause issues
TEST_F(GenerationTest, VeryLargeMaxLength)
{
    std::string word = (*generator)(rng, 1, 1000);
    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);
    EXPECT_LE(word.length(), 1000u);
}

// Test generation from minimal training data
TEST_F(GenerationTest, MinimalTrainingData)
{
    RandomWordGenerator minimalGen;
    minimalGen.analyzeWord("x");
    minimalGen.analyzeWord("y");
    minimalGen.finalize();

    std::minstd_rand testRng(12345);
    std::string      word = minimalGen(testRng);

    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);

    // Should only contain 'x' and 'y'
    for (char c : word)
    {
        EXPECT_TRUE(c == 'x' || c == 'y') << "Unexpected character: " << c;
    }
}

// Test auto-finalization behavior
TEST_F(GenerationTest, AutoFinalizationOnGeneration)
{
    RandomWordGenerator autoGen;
    autoGen.analyzeWord("hello");
    autoGen.analyzeWord("world");

    EXPECT_FALSE(autoGen.isFinalized()); // Should not be finalized yet

    // Don't call finalize() explicitly
    std::minstd_rand testRng(12345);
    std::string      word = autoGen(testRng);

    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);

    // Should now be finalized after calling operator()
    EXPECT_TRUE(autoGen.isFinalized());
    EXPECT_FALSE(autoGen.analyzeWord("newword")); // Should fail since it's now finalized
}

// Test auto-finalization with empty generator
TEST_F(GenerationTest, AutoFinalizationWithEmptyGenerator)
{
    RandomWordGenerator emptyGen;
    // Don't add any words, don't call finalize()

    EXPECT_FALSE(emptyGen.isFinalized()); // Should not be finalized yet

    std::minstd_rand testRng(12345);
    std::string      word = emptyGen(testRng);

    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);

    // Should now be finalized after calling operator()
    EXPECT_TRUE(emptyGen.isFinalized());
    EXPECT_FALSE(emptyGen.analyzeWord("newword")); // Should fail since it's now finalized
}

// Test minimum length enforcement
TEST_F(GenerationTest, MinimumLengthEnforcement)
{
    // Test various minimum lengths
    for (size_t minLen = 1; minLen <= 8; ++minLen)
    {
        for (int trial = 0; trial < 10; ++trial)
        {
            std::string word = (*generator)(rng, minLen, 0); // No max length limit
            EXPECT_GE(word.length(), minLen) << "Word '" << word << "' is too short (expected >= " << minLen << ")";
            EXPECT_FALSE(word.empty());
        }
    }
}

// Test edge case: minLength = 0 (should behave like minLength = 1)
TEST_F(GenerationTest, ZeroMinLength)
{
    std::string word = (*generator)(rng, 0, 10); // minLength=0, maxLength=10
    EXPECT_FALSE(word.empty());
    // Should still generate at least one character even with minLength=0
    EXPECT_GE(word.length(), 1u);
    EXPECT_LE(word.length(), 10u);
}

// Test edge case: minLength > maxLength (should handle gracefully)
TEST_F(GenerationTest, MinLengthGreaterThanMaxLength)
{
    // This should either generate words of minLength or handle the conflict gracefully
    std::string word = (*generator)(rng, 5, 3); // minLength=5, maxLength=3 (conflict)
    EXPECT_FALSE(word.empty());
    // The behavior depends on implementation, but word should not be empty
    EXPECT_GE(word.length(), 1u);
}
