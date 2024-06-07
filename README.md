```
# Persistent Hash Array Mapped Trie (HAMT) Implementation

This project implements a persistent Hash Array Mapped Trie (HAMT) data structure in C. The HAMT allows for efficient storage and retrieval of key-value pairs with a focus on persistence, enabling efficient versioning and rollback operations.

## Table of Contents

- [Background](#background)
- [Installation](#installation)
- [Usage](#usage)
- [Error Handling](#error-handling)
- [Contributing](#contributing)
- [License](#license)

## Background

A Hash Array Mapped Trie (HAMT) is a data structure that combines the properties of a hash table and a trie. It is particularly useful for persistent data structures, where old versions of the data need to be preserved while allowing efficient updates and queries.

This project provides a C implementation of a versioned HAMT, allowing for the creation, insertion, updating, deletion, and querying of key-value pairs while maintaining multiple versions of the data.

## Installation

To use this project, follow these steps:

1. Clone the repository to your local machine:

```bash
git clone <repository_url>
cd persistent-hamt
```

2. Compile the source files using the provided Makefile:

```bash
make clean
```

```bash
make
```

This will generate the shared library files.

## Usage

### Running Tests

To run the included test suite, execute the following command:

```bash
python linked_list_std_test.py
```

```bash
python linked_list_hamt_test.py
```

```bash
python hamt_testing.py
```

```bash
python hamt_experiments.py
```

This will execute a series of test cases to verify the functionality of the HAMT implementation.

### Running Example

An example program demonstrating the usage of the HAMT implementation is provided in `hamt.c`. To compile and run the example, use the following commands:

```bash
gcc -g hamt.c -o hamt
./hamt
```

This will compile the example program and execute it.

## Potential Error

### `OSError: libhamt.so: cannot open shared object file: No such file or directory`

If you encounter this error when running the example program or tests, it means that the system cannot find the shared library `libhamt.so`. To resolve this, you need to set the `LD_LIBRARY_PATH` environment variable to include the directory containing `libhamt.so`. You can do this using the following command:

```bash
export LD_LIBRARY_PATH=/path/to/libhamt.so:$LD_LIBRARY_PATH
```

Replace `/path/to/libhamt.so` with the actual path to the `libhamt.so` file in your system.
```