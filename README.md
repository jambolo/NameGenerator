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
```bash
generate_names [OPTIONS]
```

## Options
- `-v, --version`: Show version information
- `--min SIZE`: Minimum name size (default: 1)
- `--max SIZE`: Maximum name size (default: no limit)
- `--count COUNT`: Number of names to generate (default: 1)
- `--last`: Include a last name (default: off)
- `--male`: Generate only male names
- `--female`: Generate only female names
- `--both`: Generate both male and female names (default)

Only one of `--male`, `--female`, or `--both` can be specified.

**Examples**
```bash
# Generate 5 male full names (first + last)
generate_names --male --count 5 --last

# Generate 10 female first names only
generate_names --female --count 10

# Generate 8 names, randomly male or female, with first and last names at least 4 characters
generate_names --both --count 8 --min 4 --last
```

## Example Output
```
john smith
james brown
mary jones
linda taylor
...
```

**Dictionary Files:**

The application expects dictionary files for male, female, and last names in the executable's directory:
- `dist.male.first.txt`
- `dist.female.first.txt`
- `dist.all.last.txt`

Each line should be formatted as:
```
name frequency cumulative rank
```

Example:
```
John 0.012 0.012 1
James 0.011 0.023 2
...
```
Only the `name` and `frequency` fields are used; `cumulative` and `rank` are ignored. `name` is treated as case-insensitive.

## License

See [LICENSE](LICENSE) for details.

