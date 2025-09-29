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
std::unique_ptr<RandomWordGenerator> createGeneratorFromDistribution(std::string_view filename);
}

int main(int argc, char ** argv)
{
    CLI::App    app{"generate_words - Generates random words using distribution analysis"};
    bool        version          = false;
    int         count            = 10;
    int         minSize          = 0;
    int         maxSize          = 0;
    std::string distributionFile = "count_1w.txt";

    app.add_flag("-v,--version", version, "Show version information");
    app.add_option("--min", minSize, "Minimum name size (default: 1)")->check(CLI::Range(1, std::numeric_limits<int>::max()));
    app.add_option("--max", maxSize, "Maximum name size (default: no limit)")
        ->check(CLI::Range(1, std::numeric_limits<int>::max()));
    app.add_option("--count", count, "Number of names to generate (default: 1)")
        ->check(CLI::Range(1, std::numeric_limits<int>::max()));

    // Parse command line
    CLI11_PARSE(app, argc, argv);

    // Handle version flag
    if (version)
    {
        std::cout << "generate_words v0.1.0\n";
        return 0;
    }

    // Validate size parameters
    if (maxSize == 0)
        maxSize = std::numeric_limits<int>::max(); // No maximum size
    if (minSize > maxSize)
    {
        std::cerr << "Error: Minimum word size cannot be greater than maximum word size\n";
        return 1;
    }

    // Create a word generator
    auto wordGenerator = createGeneratorFromDistribution(distributionFile);
    if (!wordGenerator)
    {
        std::cerr << "Cannot create word generator from '" << distributionFile << "'.\n";
        return 1;
    }

    std::random_device entropy;
    std::minstd_rand   rng(entropy());

    // Generate words
    for (int i = 0; i < count; ++i)
    {
        std::string word;

        do
        {
            word = wordGenerator->generate(rng);
        } while (word.size() < minSize || word.size() > maxSize);

        std::cout << word << '\n';
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
        std::string        word;
        int64_t            count;

        // Try to parse as distribution format (word frequency cumulative rank)
        if (iss >> word >> count)
        {
            // Convert word to lowercase
            std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
            generator->analyzeWord(word, static_cast<float>(count));
        }
    }

    generator->finalize();
    return generator;
}

} // anonymous namespace
