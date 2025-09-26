# RandomWordGenerator

RandomWordGenerator is a C++ library for generating random words based on statistical analysis of input data. It is designed to be used as a backend for applications that require realistic word or name generation, such as the NameGenerator app.

## Features

- Learns character transition probabilities from input words or text.
- Supports custom alphabets and word terminators.
- Allows setting minimum and maximum length for generated words.
- Serialization and deserialization support for saving/loading generator state.
- Unit tested with GoogleTest.

## API Overview

### Construction

```cpp
RandomWordGenerator();
RandomWordGenerator(RandomWordGenerator::Table table);
```

### Training

```cpp
bool analyzeWord(const char* word, float factor = 1.0f);
bool analyzeText(const char* text, float factor = 1.0f);
void finalize();
```

### Generation

```cpp
std::string operator()(std::minstd_rand& rng, size_t minLength = 1, size_t maxLength = 0);
```

### Serialization

```cpp
std::ostream& operator<<(std::ostream& s, const RandomWordGenerator& g);
std::istream& operator>>(std::istream& s, RandomWordGenerator& g);
```

### Finalization

- Call `finalize()` after training to prepare the generator for word generation.
- `operator()` will auto-finalize if not already finalized.

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
