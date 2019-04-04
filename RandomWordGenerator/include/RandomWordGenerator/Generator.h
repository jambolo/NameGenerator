#if !defined(RANDOMWORDGENERATOR_GENERATOR_H)
#define RANDOMWORDGENERATOR_GENERATOR_H

#pragma once

#include <cstdint>
#include <random>
#include <string>

class RandomWordGenerator
{
public:
    static size_t constexpr ALPHABET_SIZE = 26;         //<! Number of represented characters.
    static size_t constexpr TERMINATOR = ALPHABET_SIZE;   //!< Index of the terminator.

    using Table = float[ALPHABET_SIZE + 1][ALPHABET_SIZE + 1][ALPHABET_SIZE + 1][ALPHABET_SIZE+1];

    //! Constructor.
    RandomWordGenerator();

    //! Destructor.
    ~RandomWordGenerator();

    //! Constructor.
    RandomWordGenerator(Table table);

    //! Returns a generated word.
    std::string operator ()(std::minstd_rand & rng, size_t maxLength = 0);

private:
    friend std::ostream & operator <<(std::ostream & s, RandomWordGenerator const & g);
    friend std::istream & operator >>(std::istream & s, RandomWordGenerator & g);

    char   nextCharacter(std::minstd_rand & rng, size_t c0, size_t c1, size_t c2);
    size_t toIndex(char c)
    {
        size_t result = alphabet_.find(c);
        return (result != std::string::npos) ? result : TERMINATOR;
    }

    char toCharacter(size_t i)
    {
        return (i < alphabet_.size()) ? alphabet_[i] : 0;
    }

    std::uniform_real_distribution<float> randomFloat_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    std::string alphabet_ = "abcdefghijklmnopqrstuvwxyz";
    float (*cdfs_)[ALPHABET_SIZE + 1][ALPHABET_SIZE + 1][ALPHABET_SIZE + 1];
};

//! Inserts a RandomWordGenerator into a stream.
std::ostream & operator <<(std::ostream & s, RandomWordGenerator const & g);

//! Extracts a RandomWordGenerator from a stream.
std::istream & operator >>(std::istream & s, RandomWordGenerator & g);

#endif // !defined(RANDOMWORDGENERATOR_GENERATOR_H)
