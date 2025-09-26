#include <gtest/gtest.h>
#include <RandomWordGenerator/Generator.h>
#include <sstream>

class SerializationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a generator with known data
        generator = std::make_unique<RandomWordGenerator>();
        generator->analyzeWord("hello");
        generator->analyzeWord("world");
        generator->analyzeWord("test");
        generator->finalize();
        
        rng.seed(12345);
    }

    std::unique_ptr<RandomWordGenerator> generator;
    std::minstd_rand rng;
};

// Test serialization to stream
TEST_F(SerializationTest, SerializeToStream) {
    EXPECT_TRUE(generator->isFinalized()); // Should be finalized from setup
    
    std::ostringstream oss;
    EXPECT_NO_THROW(oss << *generator);
    
    std::string serialized = oss.str();
    EXPECT_FALSE(serialized.empty());
    EXPECT_FALSE(oss.fail()); // Should succeed
    
    // Should contain floating point numbers
    EXPECT_TRUE(serialized.find('.') != std::string::npos);
}

// Test serialization of unfinalized generator fails
TEST_F(SerializationTest, SerializeUnfinalizedGeneratorFails) {
    RandomWordGenerator unfinalizedGen;
    unfinalizedGen.analyzeWord("test"); // Add data but don't finalize
    
    EXPECT_FALSE(unfinalizedGen.isFinalized()); // Should not be finalized
    
    std::ostringstream oss;
    oss << unfinalizedGen;
    
    // Should set fail bit
    EXPECT_TRUE(oss.fail());
    
    // Generator should still not be finalized after failed serialization
    EXPECT_FALSE(unfinalizedGen.isFinalized());
    
    // Output should be empty or minimal
    std::string serialized = oss.str();
    EXPECT_TRUE(serialized.empty()); // No data should be written on failure
}

// Test deserialization from stream
TEST_F(SerializationTest, DeserializeFromStream) {
    // First serialize
    std::ostringstream oss;
    oss << *generator;
    
    // Then deserialize
    std::istringstream iss(oss.str());
    RandomWordGenerator deserialized;
    
    EXPECT_FALSE(deserialized.isFinalized()); // Should start unfinalized
    EXPECT_NO_THROW(iss >> deserialized);
    EXPECT_TRUE(deserialized.isFinalized()); // Should be finalized after deserialization
    
    // Should be able to generate words
    std::string word = deserialized(rng);
    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);
}

// Test round-trip serialization consistency
TEST_F(SerializationTest, RoundTripConsistency) {
    std::minstd_rand rng1(12345);
    std::minstd_rand rng2(12345);
    
    // Generate words from original with explicit parameters for consistency
    std::vector<std::string> originalWords;
    for (int i = 0; i < 10; ++i) {
        originalWords.push_back((*generator)(rng1, 1, 0)); // Use explicit parameters
    }
    
    // Serialize and deserialize
    std::ostringstream oss;
    oss << *generator;
    
    std::istringstream iss(oss.str());
    RandomWordGenerator deserialized;
    iss >> deserialized;
    
    EXPECT_TRUE(deserialized.isFinalized()); // Should be finalized after deserialization
    
    // Generate words from deserialized version with same parameters
    std::vector<std::string> deserializedWords;
    for (int i = 0; i < 10; ++i) {
        deserializedWords.push_back(deserialized(rng2, 1, 0)); // Use explicit parameters
    }
    
    // Should produce the same sequence
    EXPECT_EQ(originalWords, deserializedWords);
}

// Test deserialization of invalid data
TEST_F(SerializationTest, DeserializeInvalidData) {
    std::istringstream iss("invalid data here");
    RandomWordGenerator gen;
    
    EXPECT_FALSE(gen.isFinalized()); // Should start unfinalized
    
    iss >> gen;
    EXPECT_TRUE(iss.fail());
    
    // Generator should remain unfinalized after failed deserialization
    EXPECT_FALSE(gen.isFinalized());
}

// Test deserialization of partial data
TEST_F(SerializationTest, DeserializePartialData) {
    std::istringstream iss("0.1 0.2 0.3"); // Not enough data
    RandomWordGenerator gen;
    
    EXPECT_FALSE(gen.isFinalized()); // Should start unfinalized
    
    iss >> gen;
    EXPECT_TRUE(iss.fail());
    
    // Generator should remain unfinalized after failed deserialization
    EXPECT_FALSE(gen.isFinalized());
}

// Test deserialization with negative values
TEST_F(SerializationTest, DeserializeWithNegativeValues) {
    std::ostringstream oss;
    oss << *generator;
    std::string serialized = oss.str();
    
    // Replace first value with negative
    size_t firstSpace = serialized.find(' ');
    if (firstSpace != std::string::npos) {
        serialized = "-1.0 " + serialized.substr(firstSpace + 1);
    }
    
    std::istringstream iss(serialized);
    RandomWordGenerator gen;
    
    EXPECT_FALSE(gen.isFinalized()); // Should start unfinalized
    
    iss >> gen;
    EXPECT_TRUE(iss.fail());
    
    // Generator should remain unfinalized after failed deserialization
    EXPECT_FALSE(gen.isFinalized());
}

// Test deserialization with values > 1.0
TEST_F(SerializationTest, DeserializeWithValuesGreaterThanOne) {
    std::ostringstream oss;
    oss << *generator;
    std::string serialized = oss.str();
    
    // Replace first value with > 1.0
    size_t firstSpace = serialized.find(' ');
    if (firstSpace != std::string::npos) {
        serialized = "1.5 " + serialized.substr(firstSpace + 1);
    }
    
    std::istringstream iss(serialized);
    RandomWordGenerator gen;
    
    EXPECT_FALSE(gen.isFinalized()); // Should start unfinalized
    
    iss >> gen;
    EXPECT_TRUE(iss.fail());
    
    // Generator should remain unfinalized after failed deserialization
    EXPECT_FALSE(gen.isFinalized());
}

// Test serialization of empty generator
TEST_F(SerializationTest, SerializeEmptyGenerator) {
    RandomWordGenerator emptyGen;
    
    EXPECT_FALSE(emptyGen.isFinalized()); // Should start unfinalized
    emptyGen.finalize(); // Empty but finalized
    EXPECT_TRUE(emptyGen.isFinalized()); // Should now be finalized
    
    std::ostringstream oss;
    EXPECT_NO_THROW(oss << emptyGen);
    EXPECT_FALSE(oss.fail()); // Should succeed since it's finalized
    
    std::string serialized = oss.str();
    EXPECT_FALSE(serialized.empty());
    
    // Deserialize and test
    std::istringstream iss(serialized);
    RandomWordGenerator deserializedEmpty;
    
    EXPECT_FALSE(deserializedEmpty.isFinalized()); // Should start unfinalized
    EXPECT_NO_THROW(iss >> deserializedEmpty);
    EXPECT_TRUE(deserializedEmpty.isFinalized()); // Should be finalized after deserialization
    
    // Should be able to generate words (uniform distribution)
    std::minstd_rand testRng(12345);
    std::string word = deserializedEmpty(testRng);
    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);
}

// Test that deserialized generator is marked as finalized
TEST_F(SerializationTest, DeserializedGeneratorIsFinalized) {
    // Serialize original
    std::ostringstream oss;
    oss << *generator;
    
    // Deserialize
    std::istringstream iss(oss.str());
    RandomWordGenerator deserialized;
    
    EXPECT_FALSE(deserialized.isFinalized()); // Should start unfinalized
    iss >> deserialized;
    EXPECT_TRUE(deserialized.isFinalized()); // Should be finalized after deserialization
    
    // Should be able to generate immediately (indicating it's finalized)
    std::string word = deserialized(rng);
    EXPECT_FALSE(word.empty());
    EXPECT_GE(word.length(), 1u);
    
    // Should not be able to analyze new words
    EXPECT_FALSE(deserialized.analyzeWord("newword"));
}

// Test isFinalized state during serialization operations
TEST_F(SerializationTest, FinalizationStatePreservedDuringSerialization) {
    // Test with finalized generator
    EXPECT_TRUE(generator->isFinalized());
    
    std::ostringstream oss1;
    oss1 << *generator;
    
    EXPECT_TRUE(generator->isFinalized()); // Should remain finalized
    
    // Test with unfinalized generator
    RandomWordGenerator unfinalized;
    unfinalized.analyzeWord("test");
    EXPECT_FALSE(unfinalized.isFinalized());
    
    std::ostringstream oss2;
    oss2 << unfinalized;
    
    EXPECT_FALSE(unfinalized.isFinalized()); // Should remain unfinalized
    EXPECT_TRUE(oss2.fail()); // Serialization should fail
}

// Test that deserialized generators work with new parameter combinations
TEST_F(SerializationTest, DeserializedGeneratorWorksWithNewParameters) {
    // Serialize and deserialize
    std::ostringstream oss;
    oss << *generator;
    
    std::istringstream iss(oss.str());
    RandomWordGenerator deserialized;
    iss >> deserialized;
    
    EXPECT_TRUE(deserialized.isFinalized());
    
    std::minstd_rand testRng(12345);
    
    // Test different parameter combinations
    std::string word1 = deserialized(testRng, 1, 5);
    EXPECT_GE(word1.length(), 1u);
    EXPECT_LE(word1.length(), 5u);
    
    std::string word2 = deserialized(testRng, 3, 0);
    EXPECT_GE(word2.length(), 3u);
    
    std::string word3 = deserialized(testRng, 2, 4);
    EXPECT_GE(word3.length(), 2u);
    EXPECT_LE(word3.length(), 4u);
}