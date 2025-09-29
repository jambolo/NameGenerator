# Applications

This directory contains the executable applications built with the RandomWordGenerator library.

## Executables

### generate_names
Generates random names using dictionary analysis of name datasets.

Names are listed one per line and are lowercase only.

**Usage:**
```bash
generate_names [OPTIONS]
```

**Options:**
- `-v, --version`: Show version information
- `--min SIZE`: Minimum name size (default: 1)
- `--max SIZE`: Maximum name size (default: no limit)
- `--count COUNT`: Number of names to generate (default: 1)
- `--last`: Include a last name (default: off)
- `--male`: Generate only male names
- `--female`: Generate only female names
- `--both`: Generate both male and female names (default)

Only one of `--male`, `--female`, or `--both` can be specified.

**Examples:**
```bash
# Generate 5 male full names (first + last)
generate_names --male --count 5 --last

# Generate 10 female first names only
generate_names --female --count 10

# Generate 8 names, randomly male or female, with first and last names at least 4 characters
generate_names --both --count 8 --min 4 --last
```

**Example Output:**
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

### generate_words
Generates random words using dictionary analysis of word datasets.

**Usage:**
```bash
generate_words [OPTIONS]
```

**Options:**
- `-v, --version`: Show version information
- `--count COUNT`: Number of words to generate (default: 10)
- `--min SIZE`: Minimum word size (default: 1)
- `--max SIZE`: Maximum word size (default: no limit)

**Examples:**
```bash
# Generate 5 words
generate_words --count 5

# Generate 10 words with minimum length of 4 characters
generate_words --count 10 --min 4

# Generate 3 words between 5 and 8 characters
generate_words --count 3 --min 5 --max 8
```

**Dictionary Files:**

The application expects a dictionary file named `count_1w.txt` in the executable's directory by default.

Each line should be formatted as:
```
word count
```

Example:
```
the 23135851162
of 13151942776
and 12997637966
...
```

The `word` field is treated as case-insensitive and converted to lowercase. The `count` field is used as a weight for the word generation algorithm.


## Build Output

Both executables are built to the `build/bin/` directory.