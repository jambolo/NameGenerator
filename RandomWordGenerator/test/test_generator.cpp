#include <RandomWordGenerator/Generator.h>
#include <gtest/gtest.h>

// Test fixture for basic RandomWordGenerator functionality
class RandomWordGeneratorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        generator = std::make_unique<RandomWordGenerator>();
        rng.seed(12345); // Use fixed seed for reproducible tests
    }

    std::unique_ptr<RandomWordGenerator> generator;
    std::minstd_rand                     rng;
};

// Test basic construction
TEST_F(RandomWordGeneratorTest, Construction)
{
    EXPECT_NO_THROW(RandomWordGenerator gen);
}

// Test construction with custom alphabet
TEST_F(RandomWordGeneratorTest, ConstructionWithCustomAlphabet)
{
    EXPECT_NO_THROW(RandomWordGenerator gen("abc"));
    EXPECT_NO_THROW(RandomWordGenerator gen("0123456789"));
    EXPECT_NO_THROW(RandomWordGenerator gen("xyz"));
}

// Test initial finalization state
TEST_F(RandomWordGeneratorTest, InitialFinalizationState)
{
    EXPECT_FALSE(generator->isFinalized());
}

// Test explicit finalization
TEST_F(RandomWordGeneratorTest, ExplicitFinalization)
{
    EXPECT_FALSE(generator->isFinalized());

    generator->analyzeWord("test");
    EXPECT_FALSE(generator->isFinalized()); // Still not finalized after adding words

    generator->finalize();
    EXPECT_TRUE(generator->isFinalized()); // Now should be finalized
}

// Test finalization of empty generator
TEST_F(RandomWordGeneratorTest, FinalizeEmptyGenerator)
{
    EXPECT_FALSE(generator->isFinalized());

    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized());

    // Should generate nothing
    std::string result = generator->generate(rng);
    EXPECT_TRUE(result.empty());
}

// Test multiple calls to finalize (should be safe)
TEST_F(RandomWordGeneratorTest, MultipleFinalizeCallsAreSafe)
{
    EXPECT_FALSE(generator->isFinalized());

    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());

    EXPECT_NO_THROW(generator->finalize());
    EXPECT_TRUE(generator->isFinalized()); // Should remain finalized
}

// Test that analyzeWord fails after finalization
TEST_F(RandomWordGeneratorTest, AnalyzeWordFailsAfterFinalization)
{
    EXPECT_TRUE(generator->analyzeWord("hello")); // Should succeed
    EXPECT_FALSE(generator->isFinalized());

    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());

    EXPECT_FALSE(generator->analyzeWord("world")); // Should fail after finalization
}

// Test that analyzeText fails after finalization
TEST_F(RandomWordGeneratorTest, AnalyzeTextFailsAfterFinalization)
{
    EXPECT_TRUE(generator->analyzeText("hello world")); // Should succeed
    EXPECT_FALSE(generator->isFinalized());

    generator->finalize();
    EXPECT_TRUE(generator->isFinalized());

    EXPECT_FALSE(generator->analyzeText("more text")); // Should fail after finalization
}

// Test copy constructor and move semantics (assignment not supported due to const members)
TEST_F(RandomWordGeneratorTest, CopyConstructorAndMoveSemantics)
{
    generator->analyzeWord("test");
    generator->analyzeWord("word");

    // Test copy constructor
    RandomWordGenerator copy(*generator);
    EXPECT_EQ(copy.isFinalized(), generator->isFinalized());

    // Both instances should be able to finalize independently
    copy.finalize();
    EXPECT_TRUE(copy.isFinalized());
    EXPECT_FALSE(generator->isFinalized()); // Original should still be unfinalized

    // Test move constructor
    generator->analyzeWord("another");
    bool original_finalized = generator->isFinalized();
    RandomWordGenerator moved(std::move(*generator));
    EXPECT_EQ(moved.isFinalized(), original_finalized);

    // Test move assignment
    RandomWordGenerator generator2;
    generator2.analyzeWord("another");
    RandomWordGenerator move_assigned = std::move(generator2);
    EXPECT_FALSE(move_assigned.isFinalized());
}
