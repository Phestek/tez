# The Tez Programming Language

This is the main code repository for Tez programming language. It contains
the compiler and documentation.

## Building from source

You need C++ compiler that supports C++17 standard. Recommended one is GCC 7.0 or newer. For some reason, it won't build
with Clang 5.0 (I'm investigating the error and probably the one to blame is Clang's std::optional implementation). It
was not tested on any MSVC version, but I assume it won't work (it still struggles with C++14, so C++17 support probably
doesn't even exists).

You also need [LLVM] library installed in your system, as I don't ship it with the compiler.

Tests are using [GTest] framework.

[LLVM]: https://llvm.org
[GTest]: https://github.com/google/googletest

## Hello world and compilation.

```
func main() -> Int {
    return 0;
}
```
$ ./tezc hello.tez -o hello.c
```
$ 

