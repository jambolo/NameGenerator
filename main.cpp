#include <CLI/CLI.hpp>
#include <RandomWordGenerator/Generator.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

namespace
{
static char constexpr MALE_NAME_DISTRIBUTION_FILE_NAME[]   = "C:\\Users\\John\\Projects\\NameGenerator\\dist.male.first.txt";
static char constexpr FEMALE_NAME_DISTRIBUTION_FILE_NAME[] = "C:\\Users\\John\\Projects\\NameGenerator\\dist.female.first.txt";
static char constexpr LAST_NAME_DISTRIBUTION_FILE_NAME[]   = "C:\\Users\\John\\Projects\\NameGenerator\\dist.all.last.txt";

std::shared_ptr<RandomWordGenerator> createGeneratorFromDistribution(char const * filename);
}

int main(int argc, char ** argv)
{
    CLI::App app{"Name Generator - Generates random names using distribution analysis"};

    bool version = false;
    app.add_flag("-v,--version", version, "Show version information");

    // Parse command line
    CLI11_PARSE(app, argc, argv);

    // Handle version flag
    if (version)
    {
        std::cout << "Name Generator v0.1.0" << std::endl;
        return 0;
    }

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
        std::cout << (*maleNameGenerator)(rng, 2) << ' ' << (*lastNameGenerator)(rng, 2) << std::endl;
    }

    // Generate 10 female names

    std::cout << std::endl << "---- Female Names ----" << std::endl;
    for (int i = 0; i < 10; ++i)
    {
        std::cout << (*femaleNameGenerator)(rng, 2) << ' ' << (*lastNameGenerator)(rng, 2) << std::endl;
    }

    return 0;
}

namespace
{
std::shared_ptr<RandomWordGenerator> createGeneratorFromDistribution(char const * filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return nullptr;

    std::shared_ptr<RandomWordGenerator> generator = std::make_shared<RandomWordGenerator>();

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
            generator->analyzeWord(name.c_str(), frequency);
        }
    }
    generator->finalize();

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

    return generator;
}

} // anonymous namespace
