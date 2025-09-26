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
        EXPECT_GE(word.length(), 1u);

        // Check that all characters are lowercase letters
        for (char c : word)
        {
            EXPECT_TRUE(c >= 'a' && c <= 'z') << "Invalid character: " << c;
        }
    }
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
    std::string word = singleGen(testRng);

    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);

    // All characters should be 'a'
    for (char c : word)
    {
        EXPECT_EQ(c, 'a');
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
    std::string word = autoGen(testRng);

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
    std::string word = emptyGen(testRng);

    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);

    // Should now be finalized after calling operator()
    EXPECT_TRUE(emptyGen.isFinalized());
    EXPECT_FALSE(emptyGen.analyzeWord("newword")); // Should fail since it's now finalized
}
