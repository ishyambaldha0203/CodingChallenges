# Implementation of User Defined String Class

This directory contains the implementation of a simple string class `String` that provides basic string functionality similar to `std::string`.
It supports dynamic resizing and implements the RAII idiom.

## Directory Structure

The Task-1 directory has the following structure:
```
Task-1
├── Makefile
├── README.md
├── String.hpp
└── TestString.cpp
```
## Usage

To compile and run the tests for the `String` class, use the provided `Makefile`:

```bash
make
./TestString
```

## Implementation Notes

1. The "String" class is not thread safe just like std::string. The user need to implement the appropriate synchronization logic to prevent the instance.
2. The UserDefined::String class uses **std::copy** instead of **strcpy or memcpy** for the following reasons:
    - A std::copy is a template function that can work with any data type and any iterator type, not just pointers. It’s part of the C++ Standard Library and is generally safer and more flexible than strcpy.
    - A strcpy is a C-style function that only works with null-terminated strings and pointers. It continues to copy characters until it encounters a null character, which means it needs to scan the entire string. This can potentially lead to buffer overflow if the destination isn’t large enough.
    - In terms of performance, memcpy is usually the fastest for memory-to-memory copy. However, it requires the size of the data to be copied, which means you need to know the length of the string in advance. In contrast, strcpy and std::copy determine the length of the string dynamically.
    - **Modern compilers are quite smart and may optimize calls to std::copy with memcpy if the targets are Plain Old Data (POD) types.**
    (https://stackoverflow.com/questions/4707012/is-it-better-to-use-stdmemcpy-or-stdcopy-in-terms-to-performance)
    - If safety and ease of use are a priority (which they often should be), std::copy is generally recommended over strcpy. If you’re working in a performance-critical context and you know the size of the data you’re copying, memcpy might be the most efficient.
