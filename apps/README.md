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

## Build Output

Both executables are built to the `build/bin/` directory.