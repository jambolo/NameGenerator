#if !defined(RANDOMWORDGENERATOR_GENERATOR_H)
#define RANDOMWORDGENERATOR_GENERATOR_H

#pragma once

#include <random>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

//! Generates random words using a Markov-like model based on character transition analysis.
//!
//! The RandomWordGenerator learns character transition probabilities from input words or text and uses this information to
//! generate realistic-sounding words. The generator uses a third-order Markov model where the next character is chosen based on
//! the preceding three characters (trigram analysis).
//!
//! ## Typical Usage:
//! 1. Create a generator with the desired alphabet
//! 2. Train it by analyzing sample words or text using analyzeWord() or analyzeText()
//! 3. Finalize the generator to prepare transition probabilities
//! 4. Generate random words using generate()
//!
//! @code
//! RandomWordGenerator generator;
//! generator.analyzeWord("hello");
//! generator.analyzeWord("world");
//! generator.finalize();
//!
//! std::minstd_rand rng;
//! std::string randomWord = generator.generate(rng);
//! @endcode
//!
//! The generator supports custom alphabets and weighted training data. Words containing characters outside the specified alphabet
//! are ignored during training.
//!
//! @note   The generator must be finalized before generating words. If not done explicitly,
//!         finalization occurs automatically on first use.
//! @note   The class is copyable and movable.
class RandomWordGenerator
{
public:
    //! Constructs a generator using the default alphabet (lowercase English letters).
    RandomWordGenerator();

    //! Constructs a generator using a custom alphabet.
    explicit RandomWordGenerator(std::string_view alphabet);

    //! Adds a word to the generator's transition graph.
    bool analyzeWord(std::string_view word, float factor = 1.0f);

    //! Adds words from the text to the generator's transition graph.
    bool analyzeText(std::string_view text, float factor = 1.0f);

    //! Finalizes the generator, allowing words to be generated.
    void finalize();

    //! Checks if the generator has been finalized.
    //!
    //! @return     true if the generator has been finalized, false otherwise.
    [[nodiscard]] bool isFinalized() const { return finalized_; }

    //! Generates a random word using the transition graph.
    [[nodiscard]] std::string generate(std::minstd_rand & rng);

private:
    // Value of the terminator character.
    static constexpr char TERMINATOR = 0;

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
