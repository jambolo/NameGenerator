#if !defined(RANDOMWORDGENERATOR_GENERATOR_H)
#define RANDOMWORDGENERATOR_GENERATOR_H

#pragma once

#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <string_view>

class RandomWordGenerator
{
public:
    static constexpr std::string_view ALPHABET      = "abcdefghijklmnopqrstuvwxyz";
    static constexpr size_t           ALPHABET_SIZE = ALPHABET.size(); // Number of represented characters.
    static constexpr size_t           TERMINATOR    = ALPHABET_SIZE;   // Index of the terminator.

    //! The distribution function table.
    //!
    //! The entry table[a] represents the distribution function of the character that follows a, such that table[a][b] is the
    //! probability that a character in the range [0, b] follows a. Note that a and b are the indexes of the characters in the
    //! alphabet, and not the characters themselves. The index ALPHABET_SIZE is used to represent the word terminator. The
    //! entry table[ALPHABET_SIZE] represents the distribution function of the first character in a word.
    using Table = float[ALPHABET_SIZE + 1][ALPHABET_SIZE + 1];

    //! Constructor.
    RandomWordGenerator();

    //! Destructor.
    ~RandomWordGenerator() = default;

    //! Constructor.
    explicit RandomWordGenerator(Table table);

    //! Adds a word to the distribution table.
    bool analyzeWord(std::string_view word, float factor = 1.0f);

    //! Adds words from the text to the distribution table.
    bool analyzeText(std::string_view text, float factor = 1.0f);

    //! Finalizes the generator.
    void finalize();

    //! Checks if the generator has been finalized.
    bool isFinalized() const { return finalized_; }

    //! Returns a generated word.
    std::string operator()(std::minstd_rand & rng);

private:
    // Serialization support
    friend std::ostream & operator<<(std::ostream & s, RandomWordGenerator const & g);
    friend std::istream & operator>>(std::istream & s, RandomWordGenerator & g);

    // Randomly selects the next character following i0.
    size_t   next(std::minstd_rand & rng, size_t i0);

    // Finds the index of a character in the alphabet, or TERMINATOR if not found.
    size_t toIndex(char c)
    {
        size_t result = ALPHABET.find(c);
        return (result != std::string::npos) ? result : TERMINATOR;
    }

    // Finds the ith character in the alphabet, or 0 if the index is out of range.
    char toCharacter(size_t i) { return (i < ALPHABET.size()) ? ALPHABET[i] : 0; }

    // Returns a random float in the range [0.0f, 1.0f).
    std::uniform_real_distribution<float> randomFloat_ = std::uniform_real_distribution<float>(0.0f, 1.0f);

    // Frequencies for each character following each character.
    std::array<std::array<float, ALPHABET_SIZE + 1>, ALPHABET_SIZE + 1> frequencies_;

    // Cumulative distribution functions for each character following each character.
    std::array<std::array<float, ALPHABET_SIZE + 1>, ALPHABET_SIZE + 1> cdfs_;

    // True if the generator has been finalized.
    bool  finalized_ = false;
};

//! Inserts a RandomWordGenerator into a stream.
std::ostream & operator<<(std::ostream & s, RandomWordGenerator const & g);

//! Extracts a RandomWordGenerator from a stream.
std::istream & operator>>(std::istream & s, RandomWordGenerator & g);

#endif // !defined(RANDOMWORDGENERATOR_GENERATOR_H)
