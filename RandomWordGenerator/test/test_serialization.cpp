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
    
    // Generate words from original
    std::vector<std::string> originalWords;
    for (int i = 0; i < 10; ++i) {
        originalWords.push_back((*generator)(rng1));
    }
    
    // Serialize and deserialize
    std::ostringstream oss;
    oss << *generator;
    
    std::istringstream iss(oss.str());
    RandomWordGenerator deserialized;
    iss >> deserialized;
    
    EXPECT_TRUE(deserialized.isFinalized()); // Should be finalized after deserialization
    
    // Generate words from deserialized version
    std::vector<std::string> deserializedWords;
    for (int i = 0; i < 10; ++i) {
        deserializedWords.push_back(deserialized(rng2));
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

// Test deserialization with invalid values
TEST_F(SerializationTest, DeserializeWithInvalidValues) {
    std::ostringstream oss;
    oss << *generator;
    std::string serialized = oss.str();
    
    // Replace first value with invalid value
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
    
    // Should not be able to analyze new words
    EXPECT_FALSE(deserialized.analyzeWord("newword"));
}