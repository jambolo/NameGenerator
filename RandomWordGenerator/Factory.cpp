#include "Factory.h"

#include "Generator.h"

#include <iostream>
#include <numeric>

static std::string const ALPHABET = "abcdefghijklmnopqrstuvwxyz";

RandomWordGeneratorFactory::RandomWordGeneratorFactory()
    : frequencies_(new (float[RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1]))
    , cdfs_(new (float[RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1]))
{
    memset(frequencies_, 0, sizeof(*frequencies_)*(RandomWordGenerator::ALPHABET_SIZE+1));
}

//! @return     pointer to the created RandomWordGenerator, or 0 if error
//!
//! @note       This function finalizes the the factory. No additional analysis can be done.

std::shared_ptr<RandomWordGenerator> RandomWordGeneratorFactory::create()
{
    if (!finalized_)
        finalize();

    return std::make_shared<RandomWordGenerator>(cdfs_);
}

//! @param  word    Word to process
//! @param  factor  Relative overall occurrence frequency of the word. 1.0f means it occurs with average frequency.
//!
//! @return     true if the text was successfully processed

bool RandomWordGeneratorFactory::analyzeWord(char const * word, float factor /*= 1.0f*/)
{
    size_t length = strlen(word);

    if (length == 0)
        return false;

    // All characters must be in the alphabet
    for (size_t i = 0; i < length; ++i)
    {
        if (ALPHABET.find(word[i]) == std::string::npos)
            return false;
    }

    size_t c0 = RandomWordGenerator::TERMINATOR;
    size_t c1 = RandomWordGenerator::TERMINATOR;
    size_t c2 = RandomWordGenerator::TERMINATOR;

    for (size_t i = 0; i < length; ++i)
    {
        size_t c = ALPHABET.find(word[i]);

        frequencies_[c0][c1][c2][c] += factor;

        c0 = c1;
        c1 = c2;
        c2 = c;
    }

    // Add the distribution for the terminator
    frequencies_[c0][c1][c2][RandomWordGenerator::TERMINATOR] += factor;

    finalized_ = false;
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

bool RandomWordGeneratorFactory::analyzeText(char const * text, float factor /*= 1.0f*/)
{
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

void RandomWordGeneratorFactory::finalize()
{
    // The table (when finalized) contains the cumulative distribution functions for all the letters.

    for (size_t i = 0; i < RandomWordGenerator::ALPHABET_SIZE + 1; ++i)
    {
        for (size_t j = 0; j < RandomWordGenerator::ALPHABET_SIZE + 1; ++j)
        {
            for (size_t k = 0; k < RandomWordGenerator::ALPHABET_SIZE + 1; ++k)
            {
                float * dist = frequencies_[i][j][k];
                float * cdf  = cdfs_[i][j][k];

                // Compute the CDF for the final character

                float sum = std::accumulate(dist, dist + RandomWordGenerator::ALPHABET_SIZE + 1, 0.0f);
                if (sum > 0.0f)
                {
                    float c = 0.0f;
                    for (size_t m = 0; m < RandomWordGenerator::ALPHABET_SIZE + 1; ++m)
                    {
                        c     += dist[m];
                        cdf[m] = c / sum;
                    }
                }
                else
                {
                    // This never occurs, so just make a CDF that always chooses the terminator
                    for (size_t m = 0; m < RandomWordGenerator::ALPHABET_SIZE; ++m)
                    {
                        cdf[m] = 0.0f;
                    }
                    cdf[RandomWordGenerator::ALPHABET_SIZE] = 1.0f;
                }
            }
        }
    }

    finalized_ = true;
}

std::ostream & operator <<(std::ostream & s, RandomWordGeneratorFactory const & f)
{
    for (int i = 0; i < RandomWordGenerator::ALPHABET_SIZE + 1; ++i)
    {
        for (int j = 0; j < RandomWordGenerator::ALPHABET_SIZE + 1; ++j)
        {
            for (int k = 0; k < RandomWordGenerator::ALPHABET_SIZE + 1; ++k)
            {
                for (int m = 0; m < RandomWordGenerator::ALPHABET_SIZE + 1; ++m)
                {
                    s << f.frequencies_[i][j][k][m] << ' ';
                }
            }
        }
    }
    return s;
}

std::istream & operator >>(std::istream & s, RandomWordGeneratorFactory & g)
{
    for (int i = 0; i < RandomWordGenerator::ALPHABET_SIZE + 1; ++i)
    {
        for (int j = 0; j < RandomWordGenerator::ALPHABET_SIZE + 1; ++j)
        {
            for (int k = 0; k < RandomWordGenerator::ALPHABET_SIZE + 1; ++k)
            {
                for (int m = 0; m < RandomWordGenerator::ALPHABET_SIZE + 1; ++m)
                {
                    float p;
                    s >> p;
                    if (s.eof() || p < 0.0f || p > 1.0f)
                        return s;
                    g.cdfs_[i][j][k][m] = p;
                }
            }
        }
    }
    return s;
}
