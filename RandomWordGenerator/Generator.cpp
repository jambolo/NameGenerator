#include "Generator.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

RandomWordGenerator::RandomWordGenerator()
    : transitionMatrix_{}
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
    if (!std::all_of(word.begin(), word.end(), [](char c) { return ALPHABET.find(c) != std::string_view::npos; }))
        return false;

    char c0 = TERMINATOR;
    for (auto c : word)
    {
        transitionMatrix_[c0][c] += factor;
        c0 = c;
    }

    // Add the implicit transition to the terminator
    transitionMatrix_[c0][TERMINATOR] += factor;

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

    // Lambda to check if character is in alphabet
    auto inAlphabet = [](char c)
    {
        return ALPHABET.find(c) != std::string_view::npos;
    };

    // Find the start of the first word
    auto start = std::find_if(text.begin(), text.end(), inAlphabet);

    while (start != text.end())
    {
        // Find the end of the word
        auto end = std::find_if_not(start + 1, text.end(), inAlphabet);

        // Analyze the word
        analyzeWord(text.substr(start - text.begin(), end - start), factor);

        // Find the start of the next word
        start = std::find_if(end, text.end(), inAlphabet);
    }

    return true;
}

void RandomWordGenerator::finalize()
{
    if (finalized_)
        return;

    // Initialize the CDF for each transition from each character
    for (auto & entry : transitionMatrix_)
    {
        auto & cdf = cdfs_[entry.first];
        auto & row = entry.second;
        float  sum = 0.0f;

        // Compute the cumulative occurrences for each entry in the row and store it in the CDF. Also compute the total sum of
        // occurrences to normalize later.
        cdf.resize(row.size()); // Create a CDF with the same number of entries as the row
        std::transform(row.begin(), row.end(), cdf.begin(), [&sum](auto const & e) { return Edge{e.first, sum += e.second}; });

        // Divide by final sum to get the probabilities. If the sum is zero, then all entries remain zero.
        if (sum > 0.0f)
            std::for_each(row.begin(), row.end(), [sum](auto & e) { e.second /= sum; });

        // Divide by final sum to get the cumulative distribution function. If the sum is zero, then all entries remain zero.
        if (sum > 0.0f)
            std::for_each(cdf.begin(), cdf.end(), [sum](auto & e) { e.p /= sum; });
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
    for (char c = next(rng, TERMINATOR); c != TERMINATOR; c = next(rng, c))
        word += c;

    return word;
}

char RandomWordGenerator::next(std::minstd_rand & rng, char c)
{
    auto pCdf = cdfs_.find(c);

    // If the CDF for the given character doesn't exist, return the terminator
    if (pCdf == cdfs_.end())
        return TERMINATOR;

    auto const & cdf = pCdf->second;
    auto         pE = std::upper_bound(cdf.begin(), cdf.end(), randomFloat_(rng), [](float v, auto const & e) { return v < e.p; });

    // upper_bound can return end() due to rounding errors or if all entries are zero
    if (pE == cdf.end())
        return cdf.back().c; // Use the last entry

    return pE->c;
}
