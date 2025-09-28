#include "Generator.h"

#include <algorithm>
#include <cassert>

//! Initializes the generator with the lowercase English alphabet. The transition graph is initially empty.
RandomWordGenerator::RandomWordGenerator()
    : alphabet_("abcdefghijklmnopqrstuvwxyz")
    , transitionMatrix_{}
    , cdfs_{}
    , finalized_{false}
{
}

//! Initializes the generator with a user-specified alphabet. The transition graph is initially empty.
//!
//! @param  alphabet    Alphabet of valid characters. The alphabet must not be empty and must not contain a 0 (the terminator).
RandomWordGenerator::RandomWordGenerator(std::string_view alphabet)
    : alphabet_(alphabet)
    , transitionMatrix_{}
    , cdfs_{}
    , finalized_{false}
{
}

//! Processes the given word and updates the transition graph with character transitions. The word is not processed if it
//! is empty or contains non-alphabet characters, or if the generator has been finalized.
//!
//! @param  word    Word to process.
//! @param  weight  The relative occurrence frequency of the word. Transitions in a word with a higher weight will have increased
//!                 probabilities.
//!
//! @return     true if the word was successfully processed, false otherwise.
bool RandomWordGenerator::analyzeWord(std::string_view word, float weight /*= 1.0f*/)
{
    if (word.empty() || finalized_)
        return false;

    // All characters must be in the alphabet
    if (!std::all_of(word.begin(), word.end(), [this](char c) { return inAlphabet(c); }))
        return false;

    State s{TERMINATOR, TERMINATOR, TERMINATOR};
    for (auto c : word)
    {
        transitionMatrix_[s][c] += weight;
        s = State{std::get<1>(s), std::get<2>(s), c};
    }

    // Add the implicit transition to the terminator
    transitionMatrix_[s][TERMINATOR] += weight;

    return true;
}

//! Splits the input text into words using non-alphabet characters as separators, and then each word is processed and added to the
//! transition graph. The text is not processed if it is empty or the generator has been finalized.
//!
//! @param  text    Text to process.
//! @param  weight  Relative overall occurrence frequency of the words in the text. Transitions in a word with a higher weight
//!                 will have increased probabilities.
//!
//! @return     true if the text was successfully processed, false otherwise.
bool RandomWordGenerator::analyzeText(std::string_view text, float weight /*= 1.0f*/)
{
    if (text.empty() || finalized_)
        return false;

    // Find the start of the first word
    auto start = std::find_if(text.begin(), text.end(), [this](char c) { return inAlphabet(c); });

    // If just whitespace, return false
    if (start == text.end())
        return false;

    while (start != text.end())
    {
        // Find the end of the word
        auto end = std::find_if_not(start + 1, text.end(), [this](char c) { return inAlphabet(c); });

        // Analyze the word
        analyzeWord(text.substr(start - text.begin(), end - start), weight);

        // Find the start of the next word
        start = std::find_if(end, text.end(), [this](char c) { return inAlphabet(c); });
    }

    return true;
}

//! After finalization, no further words or texts can be analyzed.
void RandomWordGenerator::finalize()
{
    if (finalized_)
        return;

    // Initialize the CDF for each transition from each character
    for (auto & [state, row] : transitionMatrix_)
    {
        auto & cdf = cdfs_[state];
        float  sum = 0.0f;

        // Compute the cumulative occurrences for each entry in the row and store it in the CDF. Also compute the total sum of
        // occurrences to normalize later.
        cdf.reserve(row.size()); // Reserve space for efficiency
        for (auto const & [c, p] : row)
        {
            sum += p;
            cdf.emplace_back(Edge{c, sum});
        }

        // Divide by final sum to get the probabilities and cumulative distribution function. If the sum is zero, then all entries
        // remain zero.
        if (sum > 0.0f)
        {
            float const invSum = 1.0f / sum;
            for (auto & [c, p] : row)
            {
                p *= invSum;
            }
            for (auto & edge : cdf)
            {
                edge.p *= invSum;
            }
        }
    }

    // Mark the generator as finalized.
    finalized_ = true;
}

//! Uses the finalized transition graph to generate a random word. If the generator has not been finalized already, it is
//! finalized automatically at this time.
//!
//! @param  rng     Entropy source (random number generator).
//!
//! @return     The generated word as a string.
std::string RandomWordGenerator::generate(std::minstd_rand & rng)
{
    // If the generator has not been finalized, then finalize it now
    if (!finalized_)
        finalize();

    std::string word;

    // Generate
    State s{TERMINATOR, TERMINATOR, TERMINATOR};
    for (char c = next(rng, s); c != TERMINATOR; c = next(rng, s))
    {
        word += c;
        s = State{std::get<1>(s), std::get<2>(s), c};
    }

    return word;
}

char RandomWordGenerator::next(std::minstd_rand & rng, State const & s)
{
    auto pCdf = cdfs_.find(s);

    // If the CDF for the given character doesn't exist, return the terminator
    if (pCdf == cdfs_.end())
        return TERMINATOR;

    auto const & cdf = pCdf->second;
    if (cdf.empty())
    {
        return TERMINATOR;
    }

    float const selection = randomFloat_(rng);
    auto const  pE        = std::upper_bound(cdf.begin(), cdf.end(), selection, [](float v, Edge const & e) { return v < e.p; });

    // upper_bound can return end() due to rounding errors or if all entries are zero
    if (pE == cdf.end())
        return cdf.back().c; // Use the last entry

    return pE->c;
}

// Hash function for State (for use in unordered_map)
std::size_t RandomWordGenerator::StateHash::operator()(State const & s) const
{
    auto hash_combine = [](std::size_t & seed, std::size_t value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };
    auto h0 = std::hash<char>{}(std::get<0>(s));
    auto h1 = std::hash<char>{}(std::get<1>(s));
    auto h2 = std::hash<char>{}(std::get<2>(s));

    size_t hash = 0;
    hash_combine(hash, h0);
    hash_combine(hash, h1);
    hash_combine(hash, h2);
    return hash;
}
