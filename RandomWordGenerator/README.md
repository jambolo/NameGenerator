# RandomWordGenerator

RandomWordGenerator is a C++ library for generating random words based on statistical analysis of input data. It is designed to be
used as a backend for applications that require realistic word or name generation.

The generator is based on a Markov-like chain model. The next character in a generated word is chosen based on the preceding three characters.

## Features

- Learns character transition probabilities from input words or text
- Supports training via `analyzeWord` and `analyzeText`
- Markov chain-like model using a configurable alphabet and word terminator
- Serialization and deserialization support for saving/loading generator state
- Unit tested with GoogleTest

## API Overview

### Construction
```cpp
RandomWordGenerator();
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
std::string generate(std::minstd_rand& rng);
```
- Generates a word using the trained model and provided random number generator.

## Requirements

- C++17 compiler
- CMake >= 3.21

## Testing

Unit tests are provided using GoogleTest. To build and run tests:

```sh
cmake -DRANDOMWORDGEN_BUILD_TESTS=ON -G Ninja ..
ninja
ctest
```
