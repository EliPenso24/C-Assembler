# Two-Pass Assembler (C)

A two-pass assembler written in ANSI C for a hypothetical processor defined in a university systems-programming lab course. It translates assembly source files into machine-code output files.

## Pipeline

For each input file the assembler runs four stages:

1. **Macro expansion** (`preprocessor.c`): expands `mcro ... mcroend` macros into a `.am` file.
2. **First pass** (`first_pass.c`): builds the symbol table and computes the instruction and data counters.
3. **Second pass** (`second_pass.c`): encodes instructions and resolves labels.
4. **Output** (`output_generator.c`): writes the output files, only if no errors were found.

## Build

```
make
```

Compiles with `gcc -ansi -Wall -pedantic -g`. `make clean` removes object files, the binary and generated outputs.

## Usage

```
./assembler file1.as file2.as ...
```

Each argument must be a path ending in `.as`. Files are processed independently. Errors are printed per file. In the code, a file that fails the first or second pass does not get `.ob`, `.ent` or `.ext` files written in that run. The `.am` file may still be written, and files left over from earlier runs are not deleted.

## Output files

For an input `name.as`:

| File | Content |
|------|---------|
| `name.am` | Source after macro expansion |
| `name.ob` | Object code: instruction and data sizes, then memory image in hex (code starts at address 100) |
| `name.ent` | Entry symbols and their addresses (only if the file has `.entry`) |
| `name.ext` | Addresses where external symbols are used (only if the file uses `.extern` symbols) |

## Repository contents

- Repository root: all `*.c` and `*.h` sources, `makefile` and this README. Build from here.
- `tests/`: `good_file.as` (sample using macros, `.extern`, `.entry` and data directives) and `file1.as` to `file4.as`.
- `examples-output/`: generated `.am`, `.ob`, `.ent` and `.ext` files from the original project folder.
- `docs/`: screenshots of test runs (`*.png`).
- `build-artifacts/`: compiled object files and the `assembler` binary, kept as they were in the original project folder.

The files are the original project files, unchanged. Only their folder location differs from the original project folder, so run the assembler from the repository root and pass paths, for example `./assembler tests/good_file.as`. Output files are written next to the input file.

## Notes

The included screenshots show runs of the assembler on the supplied test inputs that report errors (for example "invalid statement" lines and a failed first pass). The intent of each test input is not documented here, and this README makes no claim about which tests pass.
