#include "Generator.h"

#include <algorithm>
#include <Misc/Assertx.h>

RandomWordGenerator::RandomWordGenerator()
    : cdfs_(new (float[ALPHABET_SIZE + 1][ALPHABET_SIZE + 1][ALPHABET_SIZE + 1][ALPHABET_SIZE+1]))
{
    memset(cdfs_, 0, sizeof(*cdfs_)*(ALPHABET_SIZE + 1));
}

//! @param    table    Distribution function table
//!
//! Structure of the distribution function table.
//!
//! The entry table[a][b][c] represents the distribution function of the character that follows the sequence (a, b, c), such
//! that table[a][b][c][d] is the probability that a character in the range [0, d] follows (a, b, c). Note that a, b, c, and d
//! are the indexes of the characters in the alphabet, and not the characters themselves.
//!
//! @note       The word terminator is an implicit character in the alphabet with an index of ALPHABET_SIZE.

RandomWordGenerator::RandomWordGenerator(Table table)
    : cdfs_(new (float[ALPHABET_SIZE + 1][ALPHABET_SIZE + 1][ALPHABET_SIZE + 1][ALPHABET_SIZE + 1]))
{
    memcpy(cdfs_, table, sizeof(*cdfs_)*(ALPHABET_SIZE + 1));
}

RandomWordGenerator::~RandomWordGenerator()
{
    delete[] cdfs_;
}

//!
//! @param  rng         Entropy source
//! @param  maxLength   Maximum number of characters in the word. If max_length == 0, then the length is unbounded.
//!
//! @return        The generated word

std::string RandomWordGenerator::operator ()(std::minstd_rand & rng, size_t maxLength /* = 0*/)
{
    std::string word;
    size_t      i0     = ALPHABET_SIZE;
    size_t      i1     = ALPHABET_SIZE;
    size_t      i2     = ALPHABET_SIZE;
    size_t      length = 0;

    // Generate up to maxLength characters (or unlimited if maxLength == 0)

    while (word.size() <= maxLength || maxLength == 0)
    {
        // Generate the next character
        char c = nextCharacter(rng, i0, i1, i2);

        // If the word is terminated then we are done
        if (c == 0)
            break;

        // Append the character to the word
        word += c;

        // Keep track of the last three characters
        i0 = i1;
        i1 = i2;
        i2 = toIndex(c);
    }

    return word;
}

char RandomWordGenerator::nextCharacter(std::minstd_rand & rng, size_t i0, size_t i1, size_t i2)
{
    auto begin = &cdfs_[i0][i1][i2][0];
    auto end   = &cdfs_[i0][i1][i2 + 1][0];
    auto i     = std::upper_bound(begin, end, randomFloat_(rng));
    return (i != end) ? toCharacter(std::distance(begin, i)) : 0;
}

std::ostream & operator <<(std::ostream & s, RandomWordGenerator const & g)
{
    for (int i = 0; i < RandomWordGenerator::ALPHABET_SIZE + 1; ++i)
    {
        for (int j = 0; j < RandomWordGenerator::ALPHABET_SIZE + 1; ++j)
        {
            for (int k = 0; k < RandomWordGenerator::ALPHABET_SIZE + 1; ++k)
            {
                for (int m = 0; m < RandomWordGenerator::ALPHABET_SIZE + 1; ++m)
                {
                    s << g.cdfs_[i][j][k][m] << ' ';
                }
            }
        }
    }
    return s;
}
std::istream & operator >>(std::istream & s, RandomWordGenerator & g)
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

