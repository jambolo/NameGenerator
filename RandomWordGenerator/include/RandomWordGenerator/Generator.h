#if !defined(RANDOMWORDGENERATOR_GENERATOR_H)
#define RANDOMWORDGENERATOR_GENERATOR_H

#pragma once

#include <functional>
#include <random>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

class RandomWordGenerator
{
public:
    static constexpr char TERMINATOR = 0; // Value of the terminator character.

    //! Constructor.
    RandomWordGenerator();

    //! Constructor.
    explicit RandomWordGenerator(std::string_view alphabet);

    //! Destructor.
    ~RandomWordGenerator() = default;

    // Non-copyable but movable
    RandomWordGenerator(RandomWordGenerator const &)             = delete;
    RandomWordGenerator & operator=(RandomWordGenerator const &) = delete;
    RandomWordGenerator(RandomWordGenerator &&)                  = default;
    RandomWordGenerator & operator=(RandomWordGenerator &&)      = default;

    //! Adds a word to the distribution table.
    bool analyzeWord(std::string_view word, float factor = 1.0f);

    //! Adds words from the text to the distribution table.
    bool analyzeText(std::string_view text, float factor = 1.0f);

    //! Finalizes the generator.
    void finalize();

    //! Checks if the generator has been finalized.
    [[nodiscard]] bool isFinalized() const { return finalized_; }

    //! Returns a generated word.
    [[nodiscard]] std::string operator()(std::minstd_rand & rng);

private:
    using State = std::tuple<char, char, char>;
    struct Edge
    {
        char  c = TERMINATOR;
        float p = 0.0f;
    };

    using EdgeList = std::vector<Edge>;
    using EdgeMap  = std::unordered_map<char, float>;

    // Hash function for State (for use in unordered_map)
    struct StateHash
    {
        [[nodiscard]] std::size_t operator()(State const & s) const;
    };

    // Randomly selects the next character following c.
    [[nodiscard]] char next(std::minstd_rand & rng, State const & s);

    // Returns true if the character is in the alphabet.
    [[nodiscard]] bool inAlphabet(char c) const { return alphabet_.find(c) != std::string::npos; }

    // Returns a random float in the range [0.0f, 1.0f).
    mutable std::uniform_real_distribution<float> randomFloat_{0.0f, 1.0f};

    // The alphabet of valid characters.
    std::string const alphabet_;

    // Transition matrix as a sparse matrix (does not contain probabilities until finalized)
    std::unordered_map<State, EdgeMap, StateHash> transitionMatrix_;

    // Cumulative distribution functions for each state as a sparse matrix. (invalid until finalized)
    std::unordered_map<State, EdgeList, StateHash> cdfs_;

    // True if the generator has been finalized.
    bool finalized_ = false;
};

#endif // !defined(RANDOMWORDGENERATOR_GENERATOR_H)
