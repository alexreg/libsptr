libsptr
=======

*libsptr* is a library for using **smart pointers** in the C language, which works with any [GCC](GNU GCC)-compatible compiler.

The library is inspired by Franklin Mathieu's [libcsptr][GitHub Snaipe/libcsptr] library.

Building
--------

**Note:** To utilise just the core functionality, it is not strictly necessary to build the library; simply include `sptr.h` in the code that uses it.

To build the library, simply run `./build` from the root directory of the project. This produces all output in the `output` subdirectory.

Installing
----------

To install the library, first build it, then run `./install` as superuser from the root directory of the project. This will install the library under the default prefix directory `/usr/local`.

A custom prefix directory can be specified by setting the `INSTALL_PREFIX` environment variable before installing.

Usage
-----

The manual page for the API is `sptr(3)`.

A PDF version of the manual page may be found [here](https://bitbucket.org/alexreg/libsptr/downloads/sptr.pdf).

[GNU GCC]: https://gcc.gnu.org

[GitHub Snaipe/libcsptr]: https://github.com/Snaipe/libcsptr
