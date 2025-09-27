#include "Generator.h"

#include <algorithm>
#include <cassert>

//! Default constructor uses the default alphabet (lowercase English letters).
RandomWordGenerator::RandomWordGenerator()
    : alphabet_("abcdefghijklmnopqrstuvwxyz")
    , transitionMatrix_{}
    , cdfs_{}
    , finalized_{false}
{
}

//! Constructor with custom alphabet.
//!
//! @param  alphabet    Alphabet of valid characters. Must not be empty. Must not contain 0 (the terminator).
RandomWordGenerator::RandomWordGenerator(std::string_view alphabet)
    : alphabet_(alphabet)
    , transitionMatrix_{}
    , cdfs_{}
    , finalized_{false}
{
}

//! @param  word    Word to process
//! @param  factor  Relative overall occurrence frequency of the word. 1.0f means it occurs with average frequency.
//!
//! @return     true if the text was successfully processed

bool RandomWordGenerator::analyzeWord(std::string_view word, float factor /*= 1.0f*/)
{
    if (word.empty() || finalized_)
        return false;

    // All characters must be in the alphabet
    if (!std::all_of(word.begin(), word.end(), [this](char c) { return inAlphabet(c); }))
        return false;

    State s{TERMINATOR, TERMINATOR, TERMINATOR};
    for (auto c : word)
    {
        transitionMatrix_[s][c] += factor;
        s = State{std::get<1>(s), std::get<2>(s), c};
    }

    // Add the implicit transition to the terminator
    transitionMatrix_[s][TERMINATOR] += factor;

    return true;
}

//! Words are separated by any characters that are not part of the alphabet.
//!
//! @param  text    Text to process
//! @param  factor  Relative overall occurrence frequency of the words in the text. 1.0f means they occurs with average frequency.
//!
//! @return     true if the text was successfully processed
//!
//! @warning    The text is not added if the generator has been finalized or the text is empty.

bool RandomWordGenerator::analyzeText(std::string_view text, float factor /*= 1.0f*/)
{
    if (text.empty() || finalized_)
        return false;

    // Find the start of the first word
    auto start = std::find_if(text.begin(), text.end(), [this](char c) { return inAlphabet(c); });

    while (start != text.end())
    {
        // Find the end of the word
        auto end = std::find_if_not(start + 1, text.end(), [this](char c) { return inAlphabet(c); });

        // Analyze the word
        analyzeWord(text.substr(start - text.begin(), end - start), factor);

        // Find the start of the next word
        start = std::find_if(end, text.end(), [this](char c) { return inAlphabet(c); });
    }

    return true;
}

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

//!
//! @param  rng         Entropy source
//!
//! @return        The generated word as a string

std::string RandomWordGenerator::operator()(std::minstd_rand & rng)
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
    auto h0 = std::hash<char>{}(std::get<0>(s));
    auto h1 = std::hash<char>{}(std::get<1>(s));
    auto h2 = std::hash<char>{}(std::get<2>(s));
    return h0 ^ (h1 << 1) ^ (h2 << 2); // Simple hash combination
}
