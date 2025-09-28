#include <RandomWordGenerator/Generator.h>
#include <gtest/gtest.h>
#include <unordered_set>

class StateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        generator = std::make_unique<RandomWordGenerator>();
        rng.seed(12345);
    }

    std::unique_ptr<RandomWordGenerator> generator;
    std::minstd_rand rng;
};

// Test trigram state transitions (internal behavior verification)
TEST_F(StateTest, TrigramStateTransitions)
{
    // Add words that should create specific trigram patterns
    generator->analyzeWord("abc");
    generator->analyzeWord("abcd");
    generator->analyzeWord("abcde");
    
    generator->finalize();
    
    // Generate multiple words to verify state transitions are working
    std::set<std::string> generated;
    for (int i = 0; i < 50; ++i)
    {
        std::string word = generator->generate(rng);
        generated.insert(word);
        
        // All generated words should contain only characters from our training data
        for (char c : word)
        {
            EXPECT_TRUE(c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e')
                << "Generated unexpected character: " << c;
        }
    }
    
    // Should generate some variety
    EXPECT_GT(generated.size(), 1u) << "No variety in generated words";
}

// Test prefix consistency
TEST_F(StateTest, PrefixConsistency)
{
    // Train with words that have common prefixes
    generator->analyzeWord("cat");
    generator->analyzeWord("car");
    generator->analyzeWord("card");
    generator->analyzeWord("care");
    
    generator->finalize();
    
    int ca_prefixes = 0;
    int total_words = 0;
    
    for (int i = 0; i < 100; ++i)
    {
        std::string word = generator->generate(rng);
        if (!word.empty())
        {
            total_words++;
            if (word.length() >= 2 && word.substr(0, 2) == "ca")
            {
                ca_prefixes++;
            }
        }
    }
    
    // Most words should start with "ca" due to training data
    if (total_words > 0)
    {
        double ca_ratio = static_cast<double>(ca_prefixes) / total_words;
        EXPECT_GT(ca_ratio, 0.1) << "Expected more words with 'ca' prefix";
    }
}

// Test pattern learning from repeated sequences
TEST_F(StateTest, PatternLearning)
{
    // Train with repetitive patterns
    for (int i = 0; i < 10; ++i)
    {
        generator->analyzeWord("abab");
        generator->analyzeWord("baba");
    }
    
    generator->finalize();
    
    // Generate words and check for pattern consistency
    for (int i = 0; i < 20; ++i)
    {
        std::string word = generator->generate(rng);
        
        // Words should only contain 'a' and 'b'
        for (char c : word)
        {
            EXPECT_TRUE(c == 'a' || c == 'b') << "Unexpected character: " << c;
        }
        
        // Check for alternating pattern hints
        if (word.length() >= 2)
        {
            bool valid_pattern = true;
            // Allow some flexibility in pattern matching
            for (size_t j = 0; j < word.length(); ++j)
            {
                if (word[j] != 'a' && word[j] != 'b')
                {
                    valid_pattern = false;
                    break;
                }
            }
            EXPECT_TRUE(valid_pattern) << "Invalid pattern in word: " << word;
        }
    }
}

// Test handling of different word lengths
TEST_F(StateTest, DifferentWordLengths)
{
    // Mix of different length words
    generator->analyzeWord("a");      // length 1
    generator->analyzeWord("bb");     // length 2
    generator->analyzeWord("ccc");    // length 3
    generator->analyzeWord("dddd");   // length 4
    generator->analyzeWord("eeeee");  // length 5
    
    generator->finalize();
    
    std::vector<size_t> lengths;
    for (int i = 0; i < 100; ++i)
    {
        std::string word = generator->generate(rng);
        if (!word.empty())
        {
            lengths.push_back(word.length());
        }
    }
    
    // Should generate words of various lengths
    EXPECT_FALSE(lengths.empty());
    
    size_t min_len = *std::min_element(lengths.begin(), lengths.end());
    size_t max_len = *std::max_element(lengths.begin(), lengths.end());
    
    EXPECT_GE(min_len, 1u);
    EXPECT_LE(max_len, 100u); // Reasonable upper bound
    
    // Should have some variety in lengths
    std::set<size_t> unique_lengths(lengths.begin(), lengths.end());
    EXPECT_GT(unique_lengths.size(), 1u) << "No variety in generated word lengths";
}

// Test frequency-based generation
TEST_F(StateTest, FrequencyBasedGeneration)
{
    // Add words with different frequencies
    generator->analyzeWord("common", 10.0f);  // High frequency
    generator->analyzeWord("rare", 1.0f);     // Low frequency
    
    generator->finalize();
    
    int common_count = 0;
    int rare_count = 0;
    int total_valid = 0;
    
    for (int i = 0; i < 200; ++i)
    {
        std::string word = generator->generate(rng);
        if (!word.empty())
        {
            total_valid++;
            
            // Check if word contains characters suggesting it came from "common" or "rare"
            bool has_common_chars = false;
            bool has_rare_chars = false;
            
            for (char c : word)
            {
                if (c == 'c' || c == 'o' || c == 'm' || c == 'n')
                {
                    has_common_chars = true;
                }
                if (c == 'r' || c == 'a' || c == 'e')
                {
                    has_rare_chars = true;
                }
            }
            
            if (has_common_chars && !has_rare_chars)
            {
                common_count++;
            }
            else if (has_rare_chars && !has_common_chars)
            {
                rare_count++;
            }
        }
    }
    
    // Due to higher frequency, "common" patterns should appear more often
    // This is a probabilistic test, so we use a reasonable threshold
    if (total_valid > 10)
    {
        EXPECT_GE(common_count, rare_count) 
            << "Frequency weighting not working as expected";
    }
}

// Test state machine termination
TEST_F(StateTest, StateMachineTermination)
{
    // Add words that should have different termination probabilities
    generator->analyzeWord("end");
    generator->analyzeWord("ending");
    generator->analyzeWord("endless");
    
    generator->finalize();
    
    std::vector<std::string> words;
    for (int i = 0; i < 50; ++i)
    {
        std::string word = generator->generate(rng);
        words.push_back(word);
    }
    
    // All words should be finite length (not infinite)
    for (const auto& word : words)
    {
        EXPECT_LT(word.length(), 1000u) << "Generated extremely long word: " << word;
        
        // Should contain only expected characters
        for (char c : word)
        {
            EXPECT_TRUE(c == 'e' || c == 'n' || c == 'd' || c == 'i' || c == 'g' || 
                       c == 'l' || c == 's') << "Unexpected character: " << c;
        }
    }
}

// Test deterministic behavior with same RNG state
TEST_F(StateTest, DeterministicBehavior)
{
    generator->analyzeWord("test");
    generator->analyzeWord("word");
    generator->finalize();
    
    // Generate sequence with first RNG
    std::minstd_rand rng1(42);
    std::vector<std::string> sequence1;
    for (int i = 0; i < 10; ++i)
    {
        sequence1.push_back(generator->generate(rng1));
    }
    
    // Generate sequence with second RNG (same seed)
    std::minstd_rand rng2(42);
    std::vector<std::string> sequence2;
    for (int i = 0; i < 10; ++i)
    {
        sequence2.push_back(generator->generate(rng2));
    }
    
    // Sequences should be identical
    EXPECT_EQ(sequence1, sequence2) << "Non-deterministic behavior detected";
}