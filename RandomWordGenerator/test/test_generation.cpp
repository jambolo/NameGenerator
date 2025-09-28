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
        std::string word = generator->generate(rng);
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
        words1.push_back(generator->generate(rng1));
        words2.push_back(generator->generate(rng2));
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
    std::string      word = singleGen.generate(testRng);

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
    std::string      word = autoGen.generate(testRng);

    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);

    // Should now be finalized after calling generate()
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
    std::string      word = emptyGen.generate(testRng);

    EXPECT_TRUE(word.empty());

    // Should now be finalized after calling generate()
    EXPECT_TRUE(emptyGen.isFinalized());
    EXPECT_FALSE(emptyGen.analyzeWord("newword")); // Should fail since it's now finalized
}

// Test generation with custom alphabet
TEST_F(GenerationTest, CustomAlphabetGeneration)
{
    RandomWordGenerator customGen("abc");
    customGen.analyzeWord("abc");
    customGen.analyzeWord("bca");
    customGen.analyzeWord("cab");
    customGen.finalize();

    std::minstd_rand testRng(12345);
    for (int i = 0; i < 10; ++i)
    {
        std::string word = customGen.generate(testRng);
        EXPECT_FALSE(word.empty());

        // All characters should be from custom alphabet
        for (char c : word)
        {
            EXPECT_TRUE(c == 'a' || c == 'b' || c == 'c') << "Invalid character: " << c;
        }
    }
}

// Test generation with numeric alphabet
TEST_F(GenerationTest, NumericAlphabetGeneration)
{
    RandomWordGenerator numGen("0123456789");
    numGen.analyzeWord("123");
    numGen.analyzeWord("456");
    numGen.analyzeWord("789");
    numGen.finalize();

    std::minstd_rand testRng(12345);
    for (int i = 0; i < 10; ++i)
    {
        std::string word = numGen.generate(testRng);
        EXPECT_FALSE(word.empty());

        // All characters should be digits
        for (char c : word)
        {
            EXPECT_TRUE(c >= '0' && c <= '9') << "Invalid character: " << c;
        }
    }
}

// Test generation with binary alphabet
TEST_F(GenerationTest, BinaryAlphabetGeneration)
{
    RandomWordGenerator binGen("01");
    binGen.analyzeWord("0");
    binGen.analyzeWord("1");
    binGen.analyzeWord("01");
    binGen.analyzeWord("10");
    binGen.analyzeWord("11");
    binGen.analyzeWord("00");
    binGen.finalize();

    std::minstd_rand testRng(12345);
    for (int i = 0; i < 20; ++i)
    {
        std::string word = binGen.generate(testRng);
        EXPECT_FALSE(word.empty());

        // All characters should be 0 or 1
        for (char c : word)
        {
            EXPECT_TRUE(c == '0' || c == '1') << "Invalid character: " << c;
        }
    }
}

// Test generation diversity with larger sample
TEST_F(GenerationTest, GenerationDiversity)
{
    RandomWordGenerator diverseGen;
    diverseGen.analyzeWord("cat");
    diverseGen.analyzeWord("dog");
    diverseGen.analyzeWord("bird");
    diverseGen.analyzeWord("fish");
    diverseGen.analyzeWord("lion");
    diverseGen.analyzeWord("tiger");
    diverseGen.finalize();

    std::minstd_rand      testRng(12345);
    std::set<std::string> generatedWords;

    // Generate many words to check diversity
    for (int i = 0; i < 100; ++i)
    {
        std::string word = diverseGen.generate(testRng);
        EXPECT_FALSE(word.empty());
        generatedWords.insert(word);
    }

    // Should generate more than just a few unique words
    EXPECT_GT(generatedWords.size(), 5u) << "Generated words lack diversity";
}

// Test generation with repeated single word
TEST_F(GenerationTest, RepeatedSingleWordGeneration)
{
    RandomWordGenerator singleWordGen;
    singleWordGen.analyzeWord("hello");
    singleWordGen.finalize();

    std::minstd_rand testRng(12345);
    for (int i = 0; i < 10; ++i)
    {
        std::string word = singleWordGen.generate(testRng);
        EXPECT_FALSE(word.empty());
        EXPECT_GE(word.length(), 1u);

        // Should contain only characters from "hello"
        for (char c : word)
        {
            EXPECT_TRUE(c == 'h' || c == 'e' || c == 'l' || c == 'o') << "Invalid character: " << c;
        }
    }
}

// Test generation with weighted words
TEST_F(GenerationTest, WeightedWordGeneration)
{
    RandomWordGenerator weightedGen;
    // Add "a" with very high weight
    for (int i = 0; i < 100; ++i)
    {
        weightedGen.analyzeWord("a", 10.0f);
    }
    // Add "b" with low weight
    weightedGen.analyzeWord("b", 1.0f);
    weightedGen.finalize();

    std::minstd_rand testRng(12345);
    int              countA = 0, countB = 0;

    // Generate words and count occurrences
    for (int i = 0; i < 100; ++i)
    {
        std::string word = weightedGen.generate(testRng);
        if (word == "a")
            countA++;
        else if (word == "b")
            countB++;
    }

    // "a" should appear much more frequently than "b"
    EXPECT_GT(countA, countB) << "Weight distribution not working correctly";
}

// Test generation length statistics
TEST_F(GenerationTest, GenerationLengthStatistics)
{
    std::minstd_rand    testRng(12345);
    std::vector<size_t> lengths;

    for (int i = 0; i < 50; ++i)
    {
        std::string word = generator->generate(testRng);
        lengths.push_back(word.length());
    }

    // Calculate basic statistics
    size_t minLength = *std::min_element(lengths.begin(), lengths.end());
    size_t maxLength = *std::max_element(lengths.begin(), lengths.end());

    EXPECT_GT(minLength, 0u) << "Generated empty word";
    EXPECT_LT(maxLength, 1000u) << "Generated unreasonably long word";

    // Most words should be reasonable length (not all single characters)
    size_t longWords = std::count_if(lengths.begin(), lengths.end(), [](size_t len) { return len > 2; });
    EXPECT_GT(longWords, 0u) << "No words longer than 2 characters generated";
}

// Test generation after multiple finalization calls
TEST_F(GenerationTest, GenerationAfterMultipleFinalization)
{
    // generator is already finalized in SetUp
    generator->finalize(); // Call again
    generator->finalize(); // And again

    std::minstd_rand testRng(12345);
    for (int i = 0; i < 5; ++i)
    {
        std::string word = generator->generate(testRng);
        EXPECT_FALSE(word.empty());
    }
}
