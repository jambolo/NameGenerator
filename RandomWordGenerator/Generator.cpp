#include "Generator.h"

#include <algorithm>
#include <cstring>
#include <iostream>

RandomWordGenerator::RandomWordGenerator()
{
    std::memset(frequencies_, 0, sizeof(frequencies_));
    std::memset(cdfs_, 0, sizeof(cdfs_));
    finalized_ = false;
}

//! @param    table    Distribution function table

RandomWordGenerator::RandomWordGenerator(Table table)
{
    std::memcpy(cdfs_, table, sizeof(cdfs_));
    finalized_ = true;
}

//! @param  word    Word to process
//! @param  factor  Relative overall occurrence frequency of the word. 1.0f means it occurs with average frequency.
//!
//! @return     true if the text was successfully processed

bool RandomWordGenerator::analyzeWord(char const * word, float factor /*= 1.0f*/)
{
    if (!word || finalized_)
        return false;

    size_t length = strlen(word);

    if (length == 0)
        return false;

    // All characters must be in the alphabet
    for (size_t i = 0; i < length; ++i)
    {
        if (ALPHABET.find(word[i]) == std::string::npos)
            return false;
    }

    size_t c0 = TERMINATOR;

    for (size_t i = 0; i < length; ++i)
    {
        size_t c = ALPHABET.find(word[i]);

        frequencies_[c0][c] += factor;

        c0 = c;
    }

    // Add the distribution for the terminator
    frequencies_[c0][TERMINATOR] += factor;

    return true;
}

//! Words are separated by any characters that are not part of the alphabet.
//!
//! @param  text    Text to process
//! @param  factor  Relative overall occurrence frequency of the words in the text. 1.0f means they occurs with average frequency.
//!
//! @return     true if the text was successfully processed
//!
//! @warning    The text is not added if the factory has been finalized or the text is empty.

bool RandomWordGenerator::analyzeText(char const * text, float factor /*= 1.0f*/)
{
    if (!text || finalized_)
        return false;

    size_t       length      = strlen(text);
    char const * end_of_text = text + length;

    if (length == 0)
        return false;

    char const * start = text;

    // Skip to the first character found in the alphabet
    while (start < end_of_text && ALPHABET.find(*start) == std::string::npos)
    {
        ++start;
    }

    while (start < end_of_text)
    {
        // Find the end of the word (first character not in the alphabet
        char const * end = start + 1;
        while (end < end_of_text && ALPHABET.find(*end) != std::string::npos)
        {
            ++end;
        }

        // Analyze the word
        analyzeWord(std::string(start, end).c_str(), factor);

        // Move to the start of the next word
        start = end;
        while (start < end_of_text && ALPHABET.find(*start) == std::string::npos)
        {
            ++start;
        }
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

    finalized_ = true;
}

//!
//! @param  rng         Entropy source
//! @param  maxLength   Maximum number of characters in the word. If max_length == 0, then the length is unbounded.
//!
//! @return        The generated word

std::string RandomWordGenerator::operator()(std::minstd_rand & rng, size_t minLength /* = 1*/, size_t maxLength /* = 0*/)
{
    // If the generator has not been finalized, then finalize it now
    if (!finalized_)
        finalize();

    std::string word;
    size_t      i0 = ALPHABET_SIZE;

    // Generate up to maxLength characters (or unlimited if maxLength == 0)
    while (word.size() < maxLength || maxLength == 0)
    {
        // Generate the next character
        char c = nextCharacter(rng, i0);

        // If the word is terminated then we are done unless the minimum length has not been reached
        if (c == 0)
        {
            if (word.size() < minLength)
            {
                // Try again
                word.clear();
                i0 = ALPHABET_SIZE;
                continue;
            }
            else
            {
                break; // Done
            }
        }

        // Append the character to the word
        word += c;

        // Keep track of the last character
        i0 = toIndex(c);
    }

    return word;
}

char RandomWordGenerator::nextCharacter(std::minstd_rand & rng, size_t i0)
{
    auto begin = &cdfs_[i0][0];
    auto end   = &cdfs_[i0][ALPHABET_SIZE + 1];
    auto i     = std::upper_bound(begin, end, randomFloat_(rng));
    return (i != end) ? toCharacter(std::distance(begin, i)) : 0;
}

std::ostream & operator<<(std::ostream & s, RandomWordGenerator const & g)
{
    // If the generator has not been finalized, then fail
    if (!g.finalized_)
    {
        s.setstate(std::ios::failbit);
        return s;
    }

    for (int i = 0; i < RandomWordGenerator::ALPHABET_SIZE + 1; ++i)
    {
        for (int j = 0; j < RandomWordGenerator::ALPHABET_SIZE + 1; ++j)
        {
            s << g.cdfs_[i][j] << ' ';
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
