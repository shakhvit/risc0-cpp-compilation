# Risc0 C++ compilation example

## Quick Start

First, make sure [rustup] is installed.

Then, install the [Risc Zero toolchain](https://dev.risczero.com/api/zkvm/install).

This example works with `1.0.1` version of the toolchain.

To build all methods and execute the method within the zkVM, run the following
command:

```bash
make execute
```

or to prove:

```bash
make prove
```

[rustup]: https://rustup.rs

## Disabling external dependencies

External dependencies could be disabled by adding arguments to the CPPFLAGS environment variable. 

The format is the following:

``` bash
CPPFLAGS="-DNO_<lib_name>"
```


> For example, to disable libsodium you can use the following command

```
CPPFLAGS="-DNO_SODIUM" make ...
```

Here is the list of libs with their macros:

``` bash
libsodium = -DNO_SODIUM
libutf8proc = -DNO_UTF
```

## Building external dependencies

First, make sure that submodules are installed.

``` bash
git submodule update --init
```

Then, to build an external library use a corresponding bash script in the `scripts` folder.

> For example, to build libsodium, use the following script.

``` bash
./scripts/build-sodium.sh
```

Built files will be placed at `external/libs/<lib>/lib` and `external/libs/<lib>/include` and commited to the git, so, possibly, you don't even need to build external dependencies since they are prebuilt. 

