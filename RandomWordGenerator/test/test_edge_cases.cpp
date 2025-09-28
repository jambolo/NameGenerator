#include <RandomWordGenerator/Generator.h>
#include <gtest/gtest.h>
#include <limits>

class EdgeCasesTest : public ::testing::Test
{
protected:
    void SetUp() override { rng.seed(12345); }

    std::minstd_rand rng;
};

// Test empty alphabet constructor (should still work)
TEST_F(EdgeCasesTest, EmptyAlphabetConstructor)
{
    EXPECT_NO_THROW(RandomWordGenerator gen(""));

    RandomWordGenerator emptyGen("");
    EXPECT_FALSE(emptyGen.analyzeWord("test")); // Should fail - no valid characters
    EXPECT_FALSE(emptyGen.analyzeText("!@#"));  // Should succeed but add no data

    emptyGen.finalize();
    std::string result = emptyGen.generate(rng);
    EXPECT_TRUE(result.empty()); // Should generate empty string
}

// Test single character alphabet
TEST_F(EdgeCasesTest, SingleCharacterAlphabet)
{
    RandomWordGenerator singleGen("x");
    EXPECT_TRUE(singleGen.analyzeWord("x"));
    EXPECT_TRUE(singleGen.analyzeWord("xx"));
    EXPECT_TRUE(singleGen.analyzeWord("xxx"));
    EXPECT_FALSE(singleGen.analyzeWord("xy")); // 'y' not in alphabet

    singleGen.finalize();

    for (int i = 0; i < 10; ++i)
    {
        std::string word = singleGen.generate(rng);
        EXPECT_FALSE(word.empty());
        for (char c : word)
        {
            EXPECT_EQ(c, 'x');
        }
    }
}

// Test alphabet with special characters
TEST_F(EdgeCasesTest, SpecialCharacterAlphabet)
{
    RandomWordGenerator specialGen("!@#$%");
    EXPECT_TRUE(specialGen.analyzeWord("!@#"));
    EXPECT_TRUE(specialGen.analyzeWord("$%"));
    EXPECT_FALSE(specialGen.analyzeWord("!@#a")); // 'a' not in alphabet

    specialGen.finalize();

    for (int i = 0; i < 10; ++i)
    {
        std::string word = specialGen.generate(rng);
        EXPECT_FALSE(word.empty());
        for (char c : word)
        {
            EXPECT_TRUE(c == '!' || c == '@' || c == '#' || c == '$' || c == '%') << "Invalid character: " << static_cast<int>(c);
        }
    }
}

// Test with extreme frequency factors
TEST_F(EdgeCasesTest, ExtremeFrequencyFactors)
{
    RandomWordGenerator extremeGen;

    // Very large frequency factor
    EXPECT_TRUE(extremeGen.analyzeWord("high", std::numeric_limits<float>::max()));

    // Very small positive frequency factor
    EXPECT_TRUE(extremeGen.analyzeWord("low", std::numeric_limits<float>::min()));

    // Zero frequency factor
    EXPECT_TRUE(extremeGen.analyzeWord("zero", 0.0f));

    // Negative frequency factor
    EXPECT_TRUE(extremeGen.analyzeWord("negative", -1.0f));

    // Infinity (if supported)
    EXPECT_TRUE(extremeGen.analyzeWord("inf", std::numeric_limits<float>::infinity()));

    extremeGen.finalize();

    // Should still be able to generate words
    for (int i = 0; i < 5; ++i)
    {
        std::string word = extremeGen.generate(rng);
        // May be empty or non-empty depending on implementation
        for (char c : word)
        {
            EXPECT_GE(c, 'a');
            EXPECT_LE(c, 'z');
        }
    }
}

// Test very long text analysis
TEST_F(EdgeCasesTest, VeryLongTextAnalysis)
{
    RandomWordGenerator longGen;

    // Create a very long text
    std::string longText;
    for (int i = 0; i < 1000; ++i)
    {
        longText += "word ";
    }

    EXPECT_TRUE(longGen.analyzeText(longText));
    longGen.finalize();

    std::string result = longGen.generate(rng);
    EXPECT_FALSE(result.empty());
    for (char c : result)
    {
        EXPECT_TRUE(c == 'w' || c == 'o' || c == 'r' || c == 'd');
    }
}

// Test text with only whitespace
TEST_F(EdgeCasesTest, WhitespaceOnlyText)
{
    RandomWordGenerator wsGen;

    EXPECT_FALSE(wsGen.analyzeText("   \t\n\r   "));
    EXPECT_FALSE(wsGen.analyzeText("")),

    wsGen.finalize();
    std::string result = wsGen.generate(rng);
    EXPECT_TRUE(result.empty()); // Should generate empty string
}

// Test mixed case alphabet (should work with custom alphabet)
TEST_F(EdgeCasesTest, MixedCaseCustomAlphabet)
{
    RandomWordGenerator mixedGen("aAbBcC");
    EXPECT_TRUE(mixedGen.analyzeWord("aAbBcC"));
    EXPECT_TRUE(mixedGen.analyzeWord("ABC"));
    EXPECT_TRUE(mixedGen.analyzeWord("abc"));
    EXPECT_FALSE(mixedGen.analyzeWord("d")); // 'd' not in alphabet

    mixedGen.finalize();

    for (int i = 0; i < 10; ++i)
    {
        std::string word = mixedGen.generate(rng);
        EXPECT_FALSE(word.empty());
        for (char c : word)
        {
            EXPECT_TRUE(c == 'a' || c == 'A' || c == 'b' || c == 'B' || c == 'c' || c == 'C') << "Invalid character: " << c;
        }
    }
}

// Test duplicate characters in alphabet
TEST_F(EdgeCasesTest, DuplicateCharactersInAlphabet)
{
    RandomWordGenerator dupGen("aabbcc");
    EXPECT_TRUE(dupGen.analyzeWord("abc"));
    EXPECT_FALSE(dupGen.analyzeWord("abcd")); // 'd' not in alphabet

    dupGen.finalize();

    for (int i = 0; i < 10; ++i)
    {
        std::string word = dupGen.generate(rng);
        EXPECT_FALSE(word.empty());
        for (char c : word)
        {
            EXPECT_TRUE(c == 'a' || c == 'b' || c == 'c');
        }
    }
}

// Test generator state after failed operations
TEST_F(EdgeCasesTest, StateAfterFailedOperations)
{
    RandomWordGenerator gen;

    // Try invalid operations
    EXPECT_FALSE(gen.analyzeWord(""));
    EXPECT_FALSE(gen.analyzeWord("INVALID"));
    EXPECT_FALSE(gen.analyzeText(""));

    // Generator should still be in valid state
    EXPECT_FALSE(gen.isFinalized());
    EXPECT_TRUE(gen.analyzeWord("valid"));

    gen.finalize();
    EXPECT_TRUE(gen.isFinalized());

    // Should be able to generate
    std::string result = gen.generate(rng);
    EXPECT_FALSE(result.empty());
}

// Test multiple consecutive empty analyze calls
TEST_F(EdgeCasesTest, MultipleEmptyAnalyzeCalls)
{
    RandomWordGenerator gen;

    // Multiple empty calls
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_FALSE(gen.analyzeWord(""));
        EXPECT_FALSE(gen.analyzeText(""));
    }

    EXPECT_FALSE(gen.isFinalized());

    // Should still work normally
    EXPECT_TRUE(gen.analyzeWord("test"));
    gen.finalize();

    std::string result = gen.generate(rng);
    EXPECT_FALSE(result.empty());
}

// Test generation with only termination transitions
TEST_F(EdgeCasesTest, OnlyTerminationTransitions)
{
    RandomWordGenerator termGen;

    // Add single character words only
    termGen.analyzeWord("a");
    termGen.analyzeWord("b");
    termGen.analyzeWord("c");

    termGen.finalize();

    // Should generate single character words
    for (int i = 0; i < 20; ++i)
    {
        std::string word = termGen.generate(rng);
        EXPECT_EQ(word.length(), 1u) << "Expected single character word, got: " << word;
        EXPECT_TRUE(word[0] == 'a' || word[0] == 'b' || word[0] == 'c');
    }
}

// Test very large number of analyze operations
TEST_F(EdgeCasesTest, ManyAnalyzeOperations)
{
    RandomWordGenerator manyGen;

    // Add many words
    for (int i = 0; i < 1000; ++i)
    {
        EXPECT_TRUE(manyGen.analyzeWord("test"));
    }

    EXPECT_FALSE(manyGen.isFinalized());
    manyGen.finalize();
    EXPECT_TRUE(manyGen.isFinalized());

    // Should still generate correctly
    std::string result = manyGen.generate(rng);
    EXPECT_FALSE(result.empty());
    for (char c : result)
    {
        EXPECT_TRUE(c == 't' || c == 'e' || c == 's');
    }
}
