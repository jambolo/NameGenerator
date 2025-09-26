#include <CLI/CLI.hpp>
#include <RandomWordGenerator/Generator.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>

namespace fs = std::filesystem;

namespace
{
// Use string_view and relative paths or make configurable
static constexpr std::string_view MALE_NAME_DISTRIBUTION_FILE_NAME   = "dist.male.first.txt";
static constexpr std::string_view FEMALE_NAME_DISTRIBUTION_FILE_NAME = "dist.female.first.txt";
static constexpr std::string_view LAST_NAME_DISTRIBUTION_FILE_NAME   = "dist.all.last.txt";

std::unique_ptr<RandomWordGenerator> createGeneratorFromDistribution(std::string_view filename);
}

int main(int argc, char ** argv)
{
    CLI::App app{"Name Generator - Generates random names using distribution analysis"};
    bool     version = false;
    int      minSize = 0;
    int      maxSize = 0;

    app.add_flag("-v,--version", version, "Show version information");
    app.add_option("--min", minSize, "Minimum name size (0 = ignore)")->check(CLI::NonNegativeNumber);
    app.add_option("--max", maxSize, "Maximum name size (0 = ignore)")->check(CLI::NonNegativeNumber);

    // Parse command line
    CLI11_PARSE(app, argc, argv);

    // Handle version flag
    if (version)
    {
        std::cout << "Name Generator v0.1.0\n";
        return 0;
    }

    // Validate size parameters
    if (maxSize == 0)
        maxSize = std::numeric_limits<int>::max(); // No maximum size
    if (minSize > maxSize)
    {
        std::cerr << "Error: Minimum name size cannot be greater than maximum name size\n";
        return 1;
    }

    // Create a male name generator
    auto maleNameGenerator = createGeneratorFromDistribution(MALE_NAME_DISTRIBUTION_FILE_NAME);
    if (!maleNameGenerator)
    {
        std::cerr << "Cannot create word generator from '" << MALE_NAME_DISTRIBUTION_FILE_NAME << "'.\n";
        return 1;
    }

    // Create a female name generator

    auto femaleNameGenerator = createGeneratorFromDistribution(FEMALE_NAME_DISTRIBUTION_FILE_NAME);
    if (!femaleNameGenerator)
    {
        std::cerr << "Cannot create word generator from '" << FEMALE_NAME_DISTRIBUTION_FILE_NAME << "'.\n";
        return 1;
    }

    // Create a last name generator

    auto lastNameGenerator = createGeneratorFromDistribution(LAST_NAME_DISTRIBUTION_FILE_NAME);
    if (!lastNameGenerator)
    {
        std::cerr << "Cannot create word generator from '" << LAST_NAME_DISTRIBUTION_FILE_NAME << "'.\n";
        return 1;
    }

    std::random_device entropy;
    std::minstd_rand   rng(entropy());

    // Generate 10 male names

    std::cout << "\n---- Male Names ----\n";
    for (int i = 0; i < 10; ++i)
    {
        std::string firstName;
        std::string lastName;

        do
        {
            firstName = (*maleNameGenerator)(rng);
            lastName  = (*lastNameGenerator)(rng);
        } while (firstName.size() < minSize || firstName.size() > maxSize || lastName.size() < minSize ||
                 lastName.size() > maxSize);
        std::cout << firstName << ' ' << lastName << '\n';
    }

    // Generate 10 female names

    std::cout << "\n---- Female Names ----\n";
    for (int i = 0; i < 10; ++i)
    {
        std::string firstName;
        std::string lastName;

        do
        {
            firstName = (*femaleNameGenerator)(rng);
            lastName  = (*lastNameGenerator)(rng);
        } while (firstName.size() < minSize || firstName.size() > maxSize || lastName.size() < minSize ||
                 lastName.size() > maxSize);
        std::cout << firstName << ' ' << lastName << '\n';
    }

    return 0;
}

namespace
{
std::unique_ptr<RandomWordGenerator> createGeneratorFromDistribution(std::string_view filename)
{
    // Use filesystem for better path handling
    if (!fs::exists(filename))
    {
        return nullptr;
    }

    std::ifstream file{std::string(filename)};
    if (!file.is_open())
        return nullptr;

    auto generator = std::make_unique<RandomWordGenerator>();

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string        name;
        float              frequency, cumulative;
        int                rank;

        if (iss >> name >> frequency >> cumulative >> rank)
        {
            // Use lambda for character transformation
            std::transform(name.begin(), name.end(), name.begin(), [](char c) { return std::tolower(c); });
            generator->analyzeWord(name, frequency);
        }
    }

    generator->finalize();
    return generator;
}

} // anonymous namespace
