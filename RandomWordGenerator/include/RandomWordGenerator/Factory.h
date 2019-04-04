#if !defined(RANDOMWORDGENERATOR_FACTORY_H)
#define RANDOMWORDGENERATOR_FACTORY_H

#pragma once

#include <memory>
#include <iosfwd>

#include <RandomWordGenerator/Generator.h>

class RandomWordGeneratorFactory
{
public:
    //! Constructor.
    RandomWordGeneratorFactory();

    //! Adds a word to the distribution table.
    bool analyzeWord( char const * word, float factor = 1.0f );

    //! Adds words from the text to the distribution table.
    bool analyzeText( char const * text, float factor = 1.0f );

    //! Creates a RandomWordGenerator from the distribution data.
    std::shared_ptr<RandomWordGenerator> create();

private:
    friend std::ostream & operator<<( std::ostream & s, RandomWordGeneratorFactory const & data );
    friend std::istream & operator>>( std::istream & s, RandomWordGeneratorFactory & data );

    void finalize();

    float (*frequencies_)[RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1];
    float (*cdfs_)[RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1][RandomWordGenerator::ALPHABET_SIZE + 1];
    bool  finalized_ = false;
};

//! Inserts a RandomWordGeneratorFactory into a stream.
std::ostream & operator<<( std::ostream & s, RandomWordGeneratorFactory const & f );

//! Extracts a RandomWordGeneratorFactory from a stream.
std::istream & operator>>( std::istream & s, RandomWordGeneratorFactory & f );

#endif // !defined(RANDOMWORDGENERATOR_FACTORY_H)
