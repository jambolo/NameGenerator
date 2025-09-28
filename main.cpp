#include <RandomWordGenerator/Generator.h>

#include <CLI/CLI.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string_view>

namespace fs = std::filesystem;

namespace
{
// Use string_view and relative paths or make configurable
std::string_view constexpr MALE_NAME_DISTRIBUTION_FILE_NAME   = "dist.male.first.txt";
std::string_view constexpr FEMALE_NAME_DISTRIBUTION_FILE_NAME = "dist.female.first.txt";
std::string_view constexpr LAST_NAME_DISTRIBUTION_FILE_NAME   = "dist.all.last.txt";

std::unique_ptr<RandomWordGenerator> createGeneratorFromDistribution(std::string_view filename);
std::string                          generateName(RandomWordGenerator * firstGen,
                                                  RandomWordGenerator * lastGen,
                                                  std::minstd_rand &    rng,
                                                  int                   minSize,
                                                  int                   maxSize,
                                                  bool                  last);
}

int main(int argc, char ** argv)
{
    CLI::App app{"generate_names - Generates random names"};
    bool     version = false;
    int      minSize = 1;
    int      maxSize = std::numeric_limits<int>::max();
    bool     last    = false;
    int      count   = 1;
    enum class Gender
    {
        Male,
        Female,
        Both
    } gender    = Gender::Both;
    bool male   = false;
    bool female = false;
    bool both   = false;

    app.add_flag("-v,--version", version, "Show version information");
    app.add_option("--min", minSize, "Minimum name size (default: 1)")->check(CLI::Range(1, std::numeric_limits<int>::max()));
    app.add_option("--max", maxSize, "Maximum name size (default: no limit)")
        ->check(CLI::Range(1, std::numeric_limits<int>::max()));
    app.add_flag("--last", last, "Include a last name (default)");
    app.add_option("--count", count, "Number of names to generate (default: 1)")
        ->check(CLI::Range(1, std::numeric_limits<int>::max()));
    auto gender_group = app.add_option_group("Gender selection");
    gender_group->add_flag("--male", male, "Generate only male names");
    gender_group->add_flag("--female", female, "Generate only female names");
    gender_group->add_flag("--both", both, "Generate both male and female names (default)");

    // Parse command line
    CLI11_PARSE(app, argc, argv);

    // Determine gender mode
    int genderCount = (male ? 1 : 0) + (female ? 1 : 0) + (both ? 1 : 0);
    if (genderCount > 1)
    {
        std::cerr << "Error: Only one of --male, --female, or --both can be specified.\n";
        return 1;
    }
    if (male)
        gender = Gender::Male;
    else if (female)
        gender = Gender::Female;
    else // default or --both
        gender = Gender::Both;

    // Handle version flag
    if (version)
    {
        std::cout << "Name Generator v0.3.0\n";
        return 0;
    }

    // Validate size parameters
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

    for (int i = 0; i < count; ++i)
    {
        if (gender == Gender::Both)
        {
            bool isMale = std::uniform_int_distribution<int>(0, 1)(rng) == 0;
            if (isMale)
                std::cout << generateName(maleNameGenerator.get(), lastNameGenerator.get(), rng, minSize, maxSize, last) << '\n';
            else
                std::cout << generateName(femaleNameGenerator.get(), lastNameGenerator.get(), rng, minSize, maxSize, last) << '\n';
        }
        else if (gender == Gender::Male)
        {
            std::cout << generateName(maleNameGenerator.get(), lastNameGenerator.get(), rng, minSize, maxSize, last) << '\n';
        }
        else if (gender == Gender::Female)
        {
            std::cout << generateName(femaleNameGenerator.get(), lastNameGenerator.get(), rng, minSize, maxSize, last) << '\n';
        }
    }

    return 0;
}

namespace
{
std::unique_ptr<RandomWordGenerator> createGeneratorFromDistribution(std::string_view filename)
{
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
            // Convert to lowercase
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return std::tolower(c); });
            generator->analyzeWord(name, frequency);
        }
    }

    generator->finalize();
    return generator;
}

std::string generateName(RandomWordGenerator * firstNameGenerator,
                         RandomWordGenerator * lastNameGenerator,
                         std::minstd_rand &    rng,
                         int                   minSize,
                         int                   maxSize,
                         bool                  last)
{
    std::string firstName;

    do
    {
        firstName = firstNameGenerator->generate(rng);
    } while (firstName.size() < minSize || firstName.size() > maxSize);

    if (!last)
        return firstName;

    std::string lastName;

    do
    {
        lastName = lastNameGenerator->generate(rng);
    } while (lastName.size() < minSize || lastName.size() > maxSize);

    return firstName + " " + lastName;
}

} // anonymous namespace
