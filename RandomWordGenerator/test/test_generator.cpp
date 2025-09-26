#include <gtest/gtest.h>
#include <RandomWordGenerator/Generator.h>

// Test fixture for basic RandomWordGenerator functionality
class RandomWordGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        generator = std::make_unique<RandomWordGenerator>();
        rng.seed(12345); // Use fixed seed for reproducible tests
    }

    std::unique_ptr<RandomWordGenerator> generator;
    std::minstd_rand rng;
};

// Test basic construction
TEST_F(RandomWordGeneratorTest, Construction) {
    EXPECT_NO_THROW(RandomWordGenerator gen);
}

// Test initial finalization state
TEST_F(RandomWordGeneratorTest, InitialFinalizationState) {
    EXPECT_FALSE(generator->isFinalized());
}

// Test explicit finalization
TEST_F(RandomWordGeneratorTest, ExplicitFinalization) {
    EXPECT_FALSE(generator->isFinalized());
    
    generator->analyzeWord("test");
    EXPECT_FALSE(generator->isFinalized()); // Still not finalized after adding words
    
    generator->finalize();
    EXPECT_TRUE(generator->isFinalized()); // Now should be finalized
}

// Test finalization of empty generator
TEST_F(RandomWordGeneratorTest, FinalizeEmptyGenerator) {
    EXPECT_FALSE(generator->isFinalized());
    
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());
    
    // Should generate something (uniform distribution when no data)
    std::string result = (*generator)(rng);
    EXPECT_FALSE(result.empty());
    EXPECT_GE(result.length(), 1u);
}

// Test multiple calls to finalize (should be safe)
TEST_F(RandomWordGeneratorTest, MultipleFinalizeCallsAreSafe) {
    EXPECT_FALSE(generator->isFinalized());
    
    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());
    
    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized()); // Should remain finalized
}

// Test that analyzeWord fails after finalization
TEST_F(RandomWordGeneratorTest, AnalyzeWordFailsAfterFinalization) {
    EXPECT_TRUE(generator->analyzeWord("hello")); // Should succeed
    EXPECT_FALSE(generator->isFinalized());
    
    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());
    
    EXPECT_FALSE(generator->analyzeWord("world")); // Should fail after finalization
}

// Test that analyzeText fails after finalization
TEST_F(RandomWordGeneratorTest, AnalyzeTextFailsAfterFinalization) {
    EXPECT_TRUE(generator->analyzeText("hello world")); // Should succeed
    EXPECT_FALSE(generator->isFinalized());
    
    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());
    
    EXPECT_FALSE(generator->analyzeText("more text")); // Should fail after finalization
}

// Test alphabet constants
TEST_F(RandomWordGeneratorTest, AlphabetConstants) {
    EXPECT_EQ(RandomWordGenerator::ALPHABET, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(RandomWordGenerator::ALPHABET_SIZE, 26);
    EXPECT_EQ(RandomWordGenerator::TERMINATOR, 26);
}

// Test construction with pre-built table
TEST_F(RandomWordGeneratorTest, ConstructionWithTable) {
    RandomWordGenerator::Table table{};
    
    // Initialize with uniform distribution
    for (size_t i = 0; i <= RandomWordGenerator::ALPHABET_SIZE; ++i) {
        float cumulative = 0.0f;
        for (size_t j = 0; j <= RandomWordGenerator::ALPHABET_SIZE; ++j) {
            cumulative += 1.0f / (RandomWordGenerator::ALPHABET_SIZE + 1);
            table[i][j] = cumulative;
        }
    }
    
    EXPECT_NO_THROW(RandomWordGenerator gen(table));
    RandomWordGenerator gen(table);
    
    // Should be finalized from construction
    EXPECT_TRUE(gen.isFinalized());
    
    // Should be able to generate words immediately
    std::string result = gen(rng);
    EXPECT_FALSE(result.empty());
    EXPECT_GE(result.length(), 1u);
    
    // Should not be able to analyze new words
    EXPECT_FALSE(gen.analyzeWord("test"));
}
