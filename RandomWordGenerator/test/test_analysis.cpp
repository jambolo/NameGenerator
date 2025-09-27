#include <gtest/gtest.h>
#include <RandomWordGenerator/Generator.h>

class AnalysisTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = std::make_unique<RandomWordGenerator>();
    }

    std::unique_ptr<RandomWordGenerator> generator;
};

// Test analyzing a single valid word
TEST_F(AnalysisTest, AnalyzeSingleWord) {
    EXPECT_FALSE(generator->isFinalized()); // Should start unfinalized
    EXPECT_TRUE(generator->analyzeWord("hello"));
    EXPECT_FALSE(generator->isFinalized()); // Should still be unfinalized after adding words
    
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized()); // Should now be finalized
}

// Test analyzing empty word
TEST_F(AnalysisTest, AnalyzeEmptyWord) {
    EXPECT_FALSE(generator->analyzeWord(""));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
}

// Test analyzing word with invalid characters
TEST_F(AnalysisTest, AnalyzeWordWithInvalidCharacters) {
    EXPECT_FALSE(generator->analyzeWord("hello123"));
    EXPECT_FALSE(generator->analyzeWord("hello!"));
    EXPECT_FALSE(generator->analyzeWord("hello world"));
    EXPECT_FALSE(generator->analyzeWord("HELLO"));  // uppercase
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
}

// Test analyzing word with only valid lowercase characters
TEST_F(AnalysisTest, AnalyzeValidWords) {
    EXPECT_TRUE(generator->analyzeWord("a"));
    EXPECT_TRUE(generator->analyzeWord("z"));
    EXPECT_TRUE(generator->analyzeWord("abcdefghijklmnopqrstuvwxyz"));
    EXPECT_TRUE(generator->analyzeWord("hello"));
    EXPECT_TRUE(generator->analyzeWord("world"));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized until explicitly finalized
}

// Test analyzing word with frequency factor
TEST_F(AnalysisTest, AnalyzeWordWithFrequencyFactor) {
    EXPECT_TRUE(generator->analyzeWord("test", 2.0f));
    EXPECT_TRUE(generator->analyzeWord("example", 0.5f));
    EXPECT_TRUE(generator->analyzeWord("word", 1.0f));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
}

// Test that analysis fails after finalization
TEST_F(AnalysisTest, AnalysisFailsAfterFinalization) {
    EXPECT_TRUE(generator->analyzeWord("hello"));
    EXPECT_FALSE(generator->isFinalized());
    
    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());
    
    EXPECT_FALSE(generator->analyzeWord("world"));
}

// Test that analysis fails after auto-finalization through operator()
TEST_F(AnalysisTest, AnalysisFailsAfterAutoFinalization) {
    EXPECT_TRUE(generator->analyzeWord("hello"));
    EXPECT_FALSE(generator->isFinalized());
    
    // Auto-finalize by calling operator()
    std::minstd_rand rng(12345);
    std::string word = (*generator)(rng);
    EXPECT_FALSE(word.empty());
    EXPECT_TRUE(generator->isFinalized()); // Should now be finalized
    
    EXPECT_FALSE(generator->analyzeWord("world")); // Should fail
}

// Test analyzing text with multiple words
TEST_F(AnalysisTest, AnalyzeText) {
    EXPECT_TRUE(generator->analyzeText("hello world test"));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}

// Test analyzing empty text
TEST_F(AnalysisTest, AnalyzeEmptyText) {
    EXPECT_FALSE(generator->analyzeText(""));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
}

// Test analyzing text with mixed valid and invalid characters
TEST_F(AnalysisTest, AnalyzeTextWithMixedCharacters) {
    EXPECT_TRUE(generator->analyzeText("hello, world! This is a test."));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}

// Test analyzing text with only separators
TEST_F(AnalysisTest, AnalyzeTextWithOnlySeparators) {
    EXPECT_TRUE(generator->analyzeText("!@#$%^&*()"));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    // Should still work, just won't add any meaningful data
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}

// Test analyzing text with numbers and punctuation
TEST_F(AnalysisTest, AnalyzeTextWithNumbersAndPunctuation) {
    EXPECT_TRUE(generator->analyzeText("word1 word2, word3! word4? word5."));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}

// Test text analysis fails after finalization
TEST_F(AnalysisTest, TextAnalysisFailsAfterFinalization) {
    EXPECT_TRUE(generator->analyzeText("hello world"));
    EXPECT_FALSE(generator->isFinalized());
    
    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());
    
    EXPECT_FALSE(generator->analyzeText("more words"));
}

// Test text analysis fails after auto-finalization
TEST_F(AnalysisTest, TextAnalysisFailsAfterAutoFinalization) {
    EXPECT_TRUE(generator->analyzeText("hello world"));
    EXPECT_FALSE(generator->isFinalized());
    
    // Auto-finalize by calling operator()
    std::minstd_rand rng(12345);
    std::string word = (*generator)(rng);
    EXPECT_FALSE(word.empty());
    EXPECT_TRUE(generator->isFinalized()); // Should now be finalized
    
    EXPECT_FALSE(generator->analyzeText("more words")); // Should fail
}

// Test analyzing text with frequency factor
TEST_F(AnalysisTest, AnalyzeTextWithFrequencyFactor) {
    EXPECT_TRUE(generator->analyzeText("common words", 2.0f));
    EXPECT_TRUE(generator->analyzeText("rare words", 0.1f));
    EXPECT_FALSE(generator->isFinalized()); // Should remain unfinalized
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
}