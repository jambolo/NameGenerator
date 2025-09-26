# NameGenerator

NameGenerator is a C++ application that generates random names using statistical analysis of name distributions. It leverages the RandomWordGenerator library to produce realistic-sounding names based on input frequency data.

## Features

- Generates male, female, and last names using customizable distribution files.
- Uses Markov chain-like analysis for realistic name generation.
- Command-line interface with options for version display and configuration.
- Supports minimum and maximum length constraints for generated names.

## Requirements

- C++17 compiler
- CMake >= 3.21 (tested with Ninja generator)
- [GoogleTest](https://github.com/google/googletest) for unit testing
- [Misc library](https://github.com/jambolo/Misc) (required dependency)
- [CLI11](https://github.com/CLIUtils/CLI11) for command-line parsing

## Building

```sh
git clone https://github.com/jambolo/NameGenerator.git
cd NameGenerator
mkdir build && cd build
cmake -G Ninja ..
ninja
```

## Usage

```sh
generate_name.exe [OPTIONS]
```

## Options
- `-h, --help`: Show help message and exit.
- `-v, --version`: Show version information

By default, the application will generate 10 male and 10 female names using the provided distribution files. You can specify the `--version` flag to display version information.

## Distribution Files

The application expects distribution files for male, female, and last names. These files should be in the format:

```
name frequency cumulative rank
```

Example:
```
John 0.012 0.012 1
James 0.011 0.023 2
... 
```

## License

See [LICENSE](LICENSE) for details.

