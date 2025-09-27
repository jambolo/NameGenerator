#if !defined(RANDOMWORDGENERATOR_GENERATOR_H)
#define RANDOMWORDGENERATOR_GENERATOR_H

#pragma once

#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>

class RandomWordGenerator
{
public:
    static constexpr std::string_view ALPHABET   = "abcdefghijklmnopqrstuvwxyz";
    static constexpr char             TERMINATOR = 0; // Value of the terminator character.

    //! Constructor.
    RandomWordGenerator();

    //! Destructor.
    ~RandomWordGenerator() = default;

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
    struct Edge
    {
        char  c = TERMINATOR;
        float p = 0.0f;
    };
    using EdgeList = std::vector<Edge>;
    using EdgeMap  = std::unordered_map<char, float>;

    // Randomly selects the next character following c.
    char next(std::minstd_rand & rng, char c);

    // Returns a random float in the range [0.0f, 1.0f).
    std::uniform_real_distribution<float> randomFloat_ = std::uniform_real_distribution<float>(0.0f, 1.0f);

    // Transition matrix as a sparse matrix (does not contain probabilities until finalized)
    std::unordered_map<char, EdgeMap> transitionMatrix_;

    // Cumulative distribution functions for each character as a sparse matrix. (invalid until finalized)
    std::unordered_map<char, EdgeList> cdfs_;

    // True if the generator has been finalized.
    bool finalized_ = false;
};

#endif // !defined(RANDOMWORDGENERATOR_GENERATOR_H)
