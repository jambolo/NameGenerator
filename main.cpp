#include <RandomWordGenerator/Factory.h>
#include <RandomWordGenerator/Generator.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <algorithm>

namespace
{
    static char constexpr MALE_NAME_DISTRIBUTION_FILE_NAME[]   = "C:\\Users\\John\\Projects\\NameGenerator\\dist.male.first.txt";
    static char constexpr FEMALE_NAME_DISTRIBUTION_FILE_NAME[] = "C:\\Users\\John\\Projects\\NameGenerator\\dist.female.first.txt";
    static char constexpr LAST_NAME_DISTRIBUTION_FILE_NAME[]   = "C:\\Users\\John\\Projects\\NameGenerator\\dist.all.last.txt";

    std::shared_ptr<RandomWordGenerator> createGeneratorFromDistribution(char const * filename);
}

int main(int argc, char ** argv)
{
    // Create a male name generator

    std::shared_ptr<RandomWordGenerator> maleNameGenerator = createGeneratorFromDistribution(MALE_NAME_DISTRIBUTION_FILE_NAME);
    if (!maleNameGenerator)
    {
        std::cerr << "Cannot create word generator from '" << MALE_NAME_DISTRIBUTION_FILE_NAME << "'." << std::endl;
        return 1;
    }

    // Create a female name generator

    std::shared_ptr<RandomWordGenerator> femaleNameGenerator = createGeneratorFromDistribution(FEMALE_NAME_DISTRIBUTION_FILE_NAME);
    if (!femaleNameGenerator)
    {
        std::cerr << "Cannot create word generator from '" << FEMALE_NAME_DISTRIBUTION_FILE_NAME << "'." << std::endl;
        return 1;
    }

    // Create a last name generator

    std::shared_ptr<RandomWordGenerator> lastNameGenerator = createGeneratorFromDistribution(LAST_NAME_DISTRIBUTION_FILE_NAME);
    if (!lastNameGenerator)
    {
        std::cerr << "Cannot create word generator from '" << LAST_NAME_DISTRIBUTION_FILE_NAME << "'." << std::endl;
        return 1;
    }

    std::random_device entropy;
    std::minstd_rand   rng(entropy());

    // Generate 10 male names

    std::cout << std::endl << "---- Male Names ----" << std::endl;
    for (int i = 0; i < 10; ++i)
    {
        std::cout << (*maleNameGenerator)(rng) << ' ' << (*lastNameGenerator)(rng) << std::endl;
    }

    // Generate 10 female names

    std::cout << std::endl << "---- Female Names ----" << std::endl;
    for (int i = 0; i < 10; ++i)
    {
        std::cout << (*femaleNameGenerator)(rng) << ' ' << (*lastNameGenerator)(rng) << std::endl;
    }

    return 0;
}

namespace
{
std::shared_ptr<RandomWordGenerator> createGeneratorFromDistribution(char const * filename)
{
    RandomWordGeneratorFactory factory;

    std::ifstream file(filename);
    if (!file.is_open())
        return std::shared_ptr<RandomWordGenerator>();

    while (!file.eof())
    {
        std::string name;
        float       frequency;
        float       cumulative;
        int         rank;

        file >> name >> frequency >> cumulative >> rank;
        if (!file.eof())
        {
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            factory.analyzeWord(name.c_str(), frequency);
        }
    }

//        {
//            string dumpfilename    = filename;
//            dumpfilename += ".fdt.txt";
//
//            ofstream out( dumpfilename.c_str() );
//            if ( out.is_open() )
//            {
//                out << factory;
//            }
//            else
//            {
//                cout << "Unable to open '"<< dumpfilename << "' for output." << endl;
//            }
//        }

    return factory.create();
}
}
