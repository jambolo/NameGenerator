#include <RandomWordGenerator/Generator.h>
#include <gtest/gtest.h>

class AnalysisTest : public ::testing::Test
{
protected:
    void SetUp() override { generator = std::make_unique<RandomWordGenerator>(); }

    std::unique_ptr<RandomWordGenerator> generator;
};

// Test analyzing a single valid word
TEST_F(AnalysisTest, AnalyzeSingleWord)
{
    EXPECT_FALSE(generator->isFinalized()); // Should start unfinalized
    EXPECT_TRUE(generator->analyzeWord("hello"));
    EXPECT_FALSE(generator->isFinalized()); // Should still be unfinalized after adding words

    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized()); // Should now be finalized
}

// Test analyzing empty word
TEST_F(AnalysisTest, AnalyzeEmptyWord)
{
    EXPECT_FALSE(generator->analyzeWord(""));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
}

// Test analyzing word with invalid characters
TEST_F(AnalysisTest, AnalyzeWordWithInvalidCharacters)
{
    EXPECT_FALSE(generator->analyzeWord("hello123"));
    EXPECT_FALSE(generator->analyzeWord("hello!"));
    EXPECT_FALSE(generator->analyzeWord("hello world"));
    EXPECT_FALSE(generator->analyzeWord("HELLO")); // uppercase
    EXPECT_FALSE(generator->isFinalized());        // Should remain unfinalized
}

// Test analyzing word with only valid lowercase characters
TEST_F(AnalysisTest, AnalyzeValidWords)
{
    EXPECT_TRUE(generator->analyzeWord("a"));
    EXPECT_TRUE(generator->analyzeWord("z"));
    EXPECT_TRUE(generator->analyzeWord("abcdefghijklmnopqrstuvwxyz"));
    EXPECT_TRUE(generator->analyzeWord("hello"));
    EXPECT_TRUE(generator->analyzeWord("world"));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized until explicitly finalized
}

// Test analyzing word with frequency factor
TEST_F(AnalysisTest, AnalyzeWordWithFrequencyFactor)
{
    EXPECT_TRUE(generator->analyzeWord("test", 2.0f));
    EXPECT_TRUE(generator->analyzeWord("example", 0.5f));
    EXPECT_TRUE(generator->analyzeWord("word", 1.0f));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
}

// Test analyzing words with zero or negative frequency factors
TEST_F(AnalysisTest, AnalyzeWordWithSpecialFrequencyFactors)
{
    EXPECT_TRUE(generator->analyzeWord("test", 0.0f));
    EXPECT_TRUE(generator->analyzeWord("example", -1.0f)); // Negative should be treated as valid
    EXPECT_FALSE(generator->isFinalized());
}

// Test that analysis fails after finalization
TEST_F(AnalysisTest, AnalysisFailsAfterFinalization)
{
    EXPECT_TRUE(generator->analyzeWord("hello"));
    EXPECT_FALSE(generator->isFinalized());

    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());

    EXPECT_FALSE(generator->analyzeWord("world"));
}

// Test that analysis fails after auto-finalization through operator()
TEST_F(AnalysisTest, AnalysisFailsAfterAutoFinalization)
{
    EXPECT_TRUE(generator->analyzeWord("hello"));
    EXPECT_FALSE(generator->isFinalized());

    // Auto-finalize by calling generate()
    std::minstd_rand rng(12345);
    std::string      word = generator->generate(rng);
    EXPECT_FALSE(word.empty());
    EXPECT_TRUE(generator->isFinalized()); // Should now be finalized

    EXPECT_FALSE(generator->analyzeWord("world")); // Should fail
}

// Test analyzing text with multiple words
TEST_F(AnalysisTest, AnalyzeText)
{
    EXPECT_TRUE(generator->analyzeText("hello world test"));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}

// Test analyzing empty text
TEST_F(AnalysisTest, AnalyzeEmptyText)
{
    EXPECT_FALSE(generator->analyzeText(""));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
}

// Test analyzing text with mixed valid and invalid characters
TEST_F(AnalysisTest, AnalyzeTextWithMixedCharacters)
{
    EXPECT_TRUE(generator->analyzeText("hello, world! This is a test."));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}

// Test analyzing text with only separators
TEST_F(AnalysisTest, AnalyzeTextWithOnlySeparators)
{
    EXPECT_FALSE(generator->analyzeText("!@#$%^&*()"));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    // Should still work, just won't add any meaningful data
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}

// Test analyzing text with numbers and punctuation
TEST_F(AnalysisTest, AnalyzeTextWithNumbersAndPunctuation)
{
    EXPECT_TRUE(generator->analyzeText("word1 word2, word3! word4? word5."));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}

// Test text analysis fails after finalization
TEST_F(AnalysisTest, TextAnalysisFailsAfterFinalization)
{
    EXPECT_TRUE(generator->analyzeText("hello world"));
    EXPECT_FALSE(generator->isFinalized());

    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());

    EXPECT_FALSE(generator->analyzeText("more words"));
}

// Test text analysis fails after auto-finalization
TEST_F(AnalysisTest, TextAnalysisFailsAfterAutoFinalization)
{
    EXPECT_TRUE(generator->analyzeText("hello world"));
    EXPECT_FALSE(generator->isFinalized());

    // Auto-finalize by calling generate()
    std::minstd_rand rng(12345);
    std::string      word = generator->generate(rng);
    EXPECT_FALSE(word.empty());
    EXPECT_TRUE(generator->isFinalized()); // Should now be finalized

    EXPECT_FALSE(generator->analyzeText("more words")); // Should fail
}

// Test analyzing text with frequency factor
TEST_F(AnalysisTest, AnalyzeTextWithFrequencyFactor)
{
    EXPECT_TRUE(generator->analyzeText("common words", 2.0f));
    EXPECT_TRUE(generator->analyzeText("rare words", 0.1f));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}

// Test with custom alphabets
TEST_F(AnalysisTest, CustomAlphabetAnalysis)
{
    // Binary alphabet
    RandomWordGenerator binaryGen("01");
    EXPECT_TRUE(binaryGen.analyzeWord("0"));
    EXPECT_TRUE(binaryGen.analyzeWord("1"));
    EXPECT_TRUE(binaryGen.analyzeWord("0110"));
    EXPECT_FALSE(binaryGen.analyzeWord("012")); // Invalid character '2'

    // Numeric alphabet
    RandomWordGenerator numericGen("0123456789");
    EXPECT_TRUE(numericGen.analyzeWord("123"));
    EXPECT_TRUE(numericGen.analyzeWord("0"));
    EXPECT_FALSE(numericGen.analyzeWord("12a")); // Invalid character 'a'

    // Mixed alphabet
    RandomWordGenerator mixedGen("abc123");
    EXPECT_TRUE(mixedGen.analyzeWord("a1b2c3"));
    EXPECT_FALSE(mixedGen.analyzeWord("a1b2c3d")); // Invalid character 'd'
}

// Test text analysis with custom alphabets
TEST_F(AnalysisTest, CustomAlphabetTextAnalysis)
{
    RandomWordGenerator customGen("abc");
    EXPECT_TRUE(customGen.analyzeText("aaa bbb ccc abc"));
    EXPECT_TRUE(customGen.analyzeText("a1b!c@d")); // Should extract 'a', 'b', 'c' as separate words
    customGen.finalize();

    std::minstd_rand rng(12345);
    std::string      result = customGen.generate(rng);
    // Result should only contain characters from custom alphabet
    for (char c : result)
    {
        EXPECT_TRUE(c == 'a' || c == 'b' || c == 'c') << "Generated invalid character: " << c;
    }
}

// Test edge cases with single character words
TEST_F(AnalysisTest, SingleCharacterWords)
{
    EXPECT_TRUE(generator->analyzeWord("a"));
    EXPECT_TRUE(generator->analyzeWord("b"));
    EXPECT_TRUE(generator->analyzeWord("z"));
    generator->finalize();

    std::minstd_rand rng(12345);
    for (int i = 0; i < 5; ++i)
    {
        std::string word = generator->generate(rng);
        EXPECT_GE(word.length(), 1u);
        for (char c : word)
        {
            EXPECT_GE(c, 'a');
            EXPECT_LE(c, 'z');
        }
    }
}

// Test very long words
TEST_F(AnalysisTest, VeryLongWords)
{
    std::string longWord(1000, 'a');
    EXPECT_TRUE(generator->analyzeWord(longWord));
    generator->finalize();

    std::minstd_rand rng(12345);
    std::string      result = generator->generate(rng);
    EXPECT_FALSE(result.empty());
    for (char c : result)
    {
        EXPECT_EQ(c, 'a');
    }
}
