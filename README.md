# julia_binary_interaction_playground

This small repository demonstrates a tiny C program that implements three
kinds of summation functions (scalars, vectors, and structs) and a simple
CLI to exercise them.

The main purpose is to use it to lean how to precompile existing C/Fortran code with [BinaryBuilder](binarybuilder.org), and -once that is done- generate Julia wrappers for the various functions.

Files of interest
- `binary_playground.c` - implementation and `main()` with CLI and optional
	dynamic loading via `dlopen`.
- `binary_playground.h` - public declarations and `MyStruct` definition.
- `Makefile` - builds the executable and a dynamic library.

Building

Build the executable:

```sh
make
```

Build the dynamic library (macOS `.dylib` or Linux `.so`):

```sh
make lib
```

Running

Run the scalar example and supply three floats:

```sh
./binary_playground --mode scalar --a 1.2 --b 3.4 --c 5.6
```

Run the built-in vector example:

```sh
./binary_playground --mode vector
```

Run the built-in struct example:

```sh
./binary_playground --mode struct
```

Run the program while forcing it to load and call the functions from the
shared library you built with `make lib`:

```sh
./binary_playground --use-lib --mode scalar --a 1 --b 2 --c 3
```

Cleaning

```sh
make clean
```

Notes
- The `Makefile` adds `-Wno-nullability-completeness` on macOS to suppress
	noisy system-header warnings.
- The program demonstrates both direct calls and dynamic symbol lookup with
	`dlsym`/`dlopen` for learning purposes.
