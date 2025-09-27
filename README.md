# NameGenerator

NameGenerator is a C++ application that generates random names using statistical analysis of name distributions. It leverages the
RandomWordGenerator library to produce realistic-sounding names based on input frequency data. A markov-like chain is used.

## Features

- Generates male, female, and last names using customizable name distribution files.
- Uses Markov-like chain analysis for realistic name generation.
- Command-line interface with name length constraints.
- Supports minimum and maximum length constraints for generated names.
- Outputs 10 male and 10 female full names (first + last) per run.

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

## Usage

```sh
generate_name.exe [OPTIONS]
```

## Options
- `-h, --help`: Show help message and exit.
- `-v, --version`: Show version information.
- `--min [N]`: Minimum name size (default: 0, ignored).
- `--max [N]`: Maximum name size (default: 0, ignored).

If `--min` or `--max` are specified, generated first and last names will be constrained to those lengths.

## Output

The application prints 10 male and 10 female names (first + last) to the console, using the provided distribution files.

Example output:
```
---- Male Names ----
john smith
james brown
...

---- Female Names ----
mary jones
linda taylor
...
```

## Distribution Files

The application expects distribution files for male, female, and last names in the executable's directory:

- `dist.male.first.txt`
- `dist.female.first.txt`
- `dist.all.last.txt`

Each file should be formatted as:

```
name frequency cumulative rank
```

Example:
```
John 0.012 0.012 1
James 0.011 0.023 2
...
```
`cumulative` and `rank` fields are ignored by the application.

## License

See [LICENSE](LICENSE) for details.

