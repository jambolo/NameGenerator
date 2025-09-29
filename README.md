# NameGenerator

NameGenerator is a C++ application that generates random names using statistical analysis of name distributions. It leverages the
RandomWordGenerator library to produce realistic-sounding names based on input frequency data. A markov-like chain is used.

## Features

- Generates male, female, and last names using customizable name distribution files.
- Uses Markov-like chain analysis for realistic name generation.
- Command-line interface with name length constraints.
- Supports minimum and maximum length constraints for generated names.
- Outputs configurable number of names per run.

## Requirements

- C++17 compiler
- CMake >= 3.21 (tested with Ninja generator)
- [GoogleTest](https://github.com/google/googletest) for unit testing
- [CLI11](https://github.com/CLIUtils/CLI11) for command-line parsing

## Building

```sh
git clone https://github.com/jambolo/NameGenerator.git
cd NameGenerator
mkdir build && cd build
cmake -G Ninja ..
ninja
```

## Applications & Usage

See [apps/README.md](apps/README.md) for details about the available applications and their command-line options.

## License

See [LICENSE](LICENSE) for details.

