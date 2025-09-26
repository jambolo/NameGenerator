#include "Generator.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

RandomWordGenerator::RandomWordGenerator()
    : frequencies_{}
    , cdfs_{}
    , finalized_{false}
{
}

//! @param    table    Distribution function table

RandomWordGenerator::RandomWordGenerator(Table table)
    : frequencies_{}
    , finalized_{true}
{
    // Copy table row by row into cdfs_
    for (size_t i = 0; i < cdfs_.size(); ++i)
    {
        std::copy(table[i], table[i] + cdfs_[i].size(), cdfs_[i].begin());
    }
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

    size_t i0 = TERMINATOR;
    for (auto c : word)
    {
        auto i = ALPHABET.find(c);
        frequencies_[i0][i] += factor;
        i0 = i;
    }

    // Add the distribution for the terminator
    frequencies_[i0][TERMINATOR] += factor;

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

    // Convert frequencies to cumulative distribution functions
    for (size_t j = 0; j <= ALPHABET_SIZE; ++j)
    {
        float sum = 0.0f;
        for (size_t k = 0; k <= ALPHABET_SIZE; ++k)
        {
            sum += frequencies_[j][k];
            cdfs_[j][k] = sum;
        }
        if (sum > 0.0f)
        {
            for (size_t k = 0; k <= ALPHABET_SIZE; ++k)
            {
                cdfs_[j][k] /= sum;
            }
        }
        else
        {
            // If there were no frequencies, then make the distribution uniform
            float p = 1.0f / (ALPHABET_SIZE + 1);
            for (size_t k = 0; k <= ALPHABET_SIZE; ++k)
            {
                cdfs_[j][k] = p * (k + 1);
            }
        }
    }

    // Mark the generator as finalized. frequencies_ is no longer considered valid
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
    for (size_t i0 = next(rng, TERMINATOR); i0 != TERMINATOR; i0 = next(rng, i0))
    {
        word += toCharacter(i0);
    }

    return word;
}

size_t RandomWordGenerator::next(std::minstd_rand & rng, size_t i0)
{
    assert(i0 <= ALPHABET_SIZE);
    auto const & cdf = cdfs_[i0];
    auto         i   = std::upper_bound(cdf.begin(), cdf.end(), randomFloat_(rng));
    // Note: end is possible due to rounding errors
    return (i != cdf.end()) ? std::distance(cdf.begin(), i) : TERMINATOR;
}

std::ostream & operator<<(std::ostream & s, RandomWordGenerator const & g)
{
    // If the generator has not been finalized, then fail
    if (!g.finalized_)
    {
        s.setstate(std::ios::failbit);
        return s;
    }

    for (auto const & row : g.cdfs_)
    {
        for (float value : row)
        {
            s << value << ' ';
        }
    }
    return s;
}

std::istream & operator>>(std::istream & s, RandomWordGenerator & g)
{
    for (int i = 0; i < RandomWordGenerator::ALPHABET_SIZE + 1; ++i)
    {
        for (int j = 0; j < RandomWordGenerator::ALPHABET_SIZE + 1; ++j)
        {
            float p;
            s >> p;
            if (s.fail() || p < 0.0f || p > 1.0f)
            {
                s.setstate(std::ios::failbit);
                return s;
            }
            g.cdfs_[i][j] = p;
        }
    }
    g.finalized_ = true;
    return s;
}
