# RandomWordGenerator

RandomWordGenerator is a C++ library for generating random words based on statistical analysis of input data. It is designed to be
used as a backend for applications that require realistic word or name generation.

The generator is based on a unigram Markov chain model.

## Features

- Learns character transition probabilities from input words or text
- Supports custom training via `analyzeWord` and `analyzeText`
- Markov chain-like model using a configurable alphabet and word terminator
- Serialization and deserialization support for saving/loading generator state
- Unit tested with GoogleTest

## API Overview

### Construction
```cpp
RandomWordGenerator();
RandomWordGenerator(RandomWordGenerator::Table table);
```

### Training
```cpp
bool analyzeWord(std::string_view word, float factor = 1.0f);
bool analyzeText(std::string_view text, float factor = 1.0f);
void finalize();
```
- Call `finalize()` after training to prepare the generator for word generation. If not called, it will auto-finalize on first use.

### Generation
```cpp
std::string operator()(std::minstd_rand& rng);
```
- Generates a word using the trained model and provided random number generator.

### Serialization
```cpp
std::ostream& operator<<(std::ostream& s, const RandomWordGenerator& g);
std::istream& operator>>(std::istream& s, RandomWordGenerator& g);
```
- Save/load generator state for reuse.

## Requirements

- C++17 compiler
- CMake >= 3.21

## Building

This library is built as part of the NameGenerator project. See the top-level [README](../README.md) for build instructions.

## Testing

Unit tests are provided using GoogleTest. To build and run tests:

```sh
cmake -DRANDOMWORDGEN_BUILD_TESTS=ON -G Ninja ..
ninja
ctest
```

## License

See [LICENSE](../LICENSE) for details.
