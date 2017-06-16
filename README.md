[![Build Status](https://travis-ci.org/WebAssembly/wabt.svg?branch=master)](https://travis-ci.org/WebAssembly/wabt) [![Windows status](https://ci.appveyor.com/api/projects/status/79hqj5l0qggw645d/branch/master?svg=true)](https://ci.appveyor.com/project/WebAssembly/wabt/branch/master)

# WABT: The WebAssembly Binary Toolkit

WABT (we pronounce it "wabbit") is a suite of tools for WebAssembly, including:

 - **wast2wasm**: translate from [WebAssembly text format](http://webassembly.github.io/spec/text/index.html) to the [WebAssembly binary format](http://webassembly.github.io/spec/binary/index.html)
 - **wasm2wast**: the inverse of wast2wasm, translate from the binary format back to the text format (also known as a .wast)
 - **wasm-objdump**: print information about a wasm binary. Similiar to objdump.
 - **wasm-interp**: decode and run a WebAssembly binary file using a stack-based interpreter
 - **wast-desugar**: parse .wast text form as supported by the spec interpreter (s-expressions, flat syntax, or mixed) and print "canonical" flat format
 - **wasm-link**: simple linker for merging multiple wasm files.

These tools are intended for use in (or for development of) toolchains or other
systems that want to manipulate WebAssembly files. Unlike the WebAssembly spec
interpreter (which is written to be as simple, declarative and "speccy" as
possible), they are written in C/C++ and designed for easier integration into
other systems. Unlike [Binaryen](https://github.com/WebAssembly/binaryen) these
tools do not aim to provide an optimization platform or a higher-level compiler
target; instead they aim for full fidelity and compliance with the spec (e.g.
1:1 round-trips with no changes to instructions).

## Online Demos

Wabt has been compiled to JavaScript via emscripten. Some of the functionality is available in the following demos:

- [index](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/index.html)
- [wast2wasm](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/wast2wasm/)
- [wasm2wast](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/wasm2wast/)

## Cloning

Clone as normal, but don't forget to get the submodules as well:

```console
$ git clone --recursive https://github.com/WebAssembly/wabt
$ cd wabt
```

This will fetch the testsuite and gtest repos, which are needed for some tests.

## Building (macOS and Linux)

You'll need [CMake](https://cmake.org). If you just run `make`, it will run
CMake for you, and put the result in `out/clang/Debug/` by default:

> Note: If you are on macOS, you will need to use CMake version 3.2 or higher

```console
$ make
```

This will build the default version of the tools: a debug build using the Clang
compiler.

There are many make targets available for other configurations as well. They
are generated from every combination of a compiler, build type and
configuration.

 - compilers: `gcc`, `clang`, `gcc-i686`, `gcc-fuzz`
 - build types: `debug`, `release`
 - configurations: empty, `asan`, `msan`, `lsan`, `ubsan`, `no-re2c-bison`,
   `no-tests`

They are combined with dashes, for example:

```console
$ make clang-debug
$ make gcc-i686-release
$ make clang-debug-lsan
$ make gcc-debug-no-re2c-bison
```

You can also run CMake yourself, the normal way:

```console
$ mkdir build
$ cd build
$ cmake ..
...
```

## Building (Windows)

You'll need [CMake](https://cmake.org). You'll also need
[Visual Studio](https://www.visualstudio.com/) (2015 or newer) or 
[MinGW](http://www.mingw.org/).

You can run CMake from the command prompt, or use the CMake GUI tool. See
[Running CMake](https://cmake.org/runningcmake/) for more information.

When running from the commandline, create a new directory for the build
artifacts, then run cmake from this directory:

```console
> cd [build dir]
> cmake [wabt project root] -DCMAKE_BUILD_TYPE=[config] -DCMAKE_INSTALL_PREFIX=[install directory] -G [generator]
```

The `[config]` parameter should be a CMake build type, typically `DEBUG` or `RELEASE`.

The `[generator]` parameter should be the type of project you want to generate,
for example `"Visual Studio 14 2015"`. You can see the list of available
generators by running `cmake --help`.

To build the project, you can use Visual Studio, or you can tell CMake to do it:

```console
> cmake --build [wabt project root] --config [config] --target install
```

This will build and install to the installation directory you provided above.

So, for example, if you want to build the debug configuration on Visual Studio 2015:

```
> mkdir build
> cd build
> cmake .. -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_INSTALL_PREFIX=..\bin -G "Visual Studio 14 2015"
> cmake --build .. --config DEBUG --target install
```

## Changing the parser or lexer

If you make changes to `src/wast-parser.y`, you'll need to install Bison.
Before you upload your PR, please run `make update-bison` to update the
prebuilt C sources in `src/prebuilt/`.

If you make changes to `src/wast-lexer.cc`, you'll need to install
[re2c](http://re2c.org). Before you upload your PR, please run `make
update-re2c` to update the prebuilt C sources in `src/prebuilt/`.

CMake will detect if you don't have re2c or Bison installed and use the
prebuilt source files instead.

## Running wast2wasm

Some examples:

```sh
# parse and typecheck test.wast
$ out/wast2wasm test.wast

# parse test.wast and write to binary file test.wasm
$ out/wast2wasm test.wast -o test.wasm

# parse spec-test.wast, and write verbose output to stdout (including the
# meaning of every byte)
$ out/wast2wasm spec-test.wast -v

# parse spec-test.wast, and write files to spec-test.json. Modules are written
# to spec-test.0.wasm, spec-test.1.wasm, etc.
$ out/wast2wasm spec-test.wast --spec -o spec-test.json
```

You can use `-h` to get additional help:

```console
$ out/wast2wasm -h
```

Or try the [online demo](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/wast2wasm/).

## Running wasm2wast

Some examples:

```sh
# parse binary file test.wasm and write s-expression file test.wast
$ out/wasm2wast test.wasm -o test.wast

# parse test.wasm and write test.wast
$ out/wasm2wast test.wasm -o test.wast
```

You can use `-h` to get additional help:

```console
$ out/wasm2wast -h
```

Or try the [online demo](https://cdn.rawgit.com/WebAssembly/wabt/013802ca01035365e2459c70f0508481393ac075/demo/wasm2wast/).

## Running wasm-interp

Some examples:

```sh
# parse binary file test.wasm, and type-check it
$ out/wasm-interp test.wasm

# parse test.wasm and run all its exported functions
$ out/wasm-interp test.wasm --run-all-exports

# parse test.wasm, run the exported functions and trace the output
$ out/wasm-interp test.wasm --run-all-exports --trace

# parse test.json and run the spec tests
$ out/wasm-interp test.json --spec

# parse test.wasm and run all its exported functions, setting the value stack
# size to 100 elements
$ out/wasm-interp test.wasm -V 100 --run-all-exports
```

As a convenience, you can use `test/run-interp.py` to convert a .wast file to
binary first, then run it in the interpreter:

```console
$ test/run-interp.py --spec spec-test.wast
20/20 tests.passed.
```

You can use `-h` to get additional help:

```console
$ out/wasm-interp -h
$ out/run-interp.py -h
```

## Running the test suite

To run all the tests with default configuration:

```console
$ make test
```

Every make target has a matching `test-*` target.

```console
$ make gcc-debug-asan
$ make test-gcc-debug-asan
$ make clang-release
$ make test-clang-release
...
```

You can also run the Python test runner script directly:

```console
$ test/run-tests.py
```

To run a subset of the tests, use a glob-like syntax:

```console
$ test/run-tests.py const -v
+ dump/const.txt (0.002s)
+ parse/assert/bad-assertreturn-non-const.txt (0.003s)
+ parse/expr/bad-const-i32-overflow.txt (0.002s)
+ parse/expr/bad-const-f32-trailing.txt (0.004s)
+ parse/expr/bad-const-i32-garbage.txt (0.005s)
+ parse/expr/bad-const-i32-trailing.txt (0.003s)
+ parse/expr/bad-const-i32-underflow.txt (0.003s)
+ parse/expr/bad-const-i64-overflow.txt (0.002s)
+ parse/expr/bad-const-i32-just-negative-sign.txt (0.004s)
+ parse/expr/const.txt (0.002s)
[+10|-0|%100] (0.11s)

$ test/run-tests.py expr*const*i32
+ parse/expr/bad-const-i32-just-negative-sign.txt (0.002s)
+ parse/expr/bad-const-i32-overflow.txt (0.003s)
+ parse/expr/bad-const-i32-underflow.txt (0.002s)
+ parse/expr/bad-const-i32-garbage.txt (0.004s)
+ parse/expr/bad-const-i32-trailing.txt (0.002s)
[+5|-0|%100] (0.11s)
```

When tests are broken, they will give you the expected stdout/stderr as a diff:

```console
$ <whoops, turned addition into subtraction in the interpreter>
$ test/run-tests.py interp/binary
- interp/binary.txt
  STDOUT MISMATCH:
  --- expected
  +++ actual
  @@ -1,4 +1,4 @@
  -i32_add() => i32:3
  +i32_add() => i32:4294967295
   i32_sub() => i32:16
   i32_mul() => i32:21
   i32_div_s() => i32:4294967294

**** FAILED ******************************************************************
- interp/binary.txt
[+0|-1|%100] (0.13s)
```

## Writing New Tests

Tests must be placed in the test/ directory, and must have the extension
`.txt`. The directory structure is mostly for convenience, so for example you
can type `test/run-tests.py interp` to run all the interpreter tests.
There's otherwise no logic attached to a test being in a given directory.

That being said, try to make the test names self explanatory, and try to test
only one thing. Also make sure that tests that are expected to fail start with
`bad-`.

The test format is straightforward:

```wast
;;; KEY1: VALUE1A VALUE1B...
;;; KEY2: VALUE2A VALUE2B...
(input (to)
  (the executable))
(;; STDOUT ;;;
expected stdout
;;; STDOUT ;;)
(;; STDERR ;;;
expected stderr
;;; STDERR ;;)
```

The test runner will copy the input to a temporary file and pass it as an
argument to the executable (which by default is `out/wast2wasm`).

The currently supported list of keys:

- `TOOL`: a set of preconfigured keys, see below.
- `EXE`: the executable to run, defaults to out/wast2wasm
- `STDIN_FILE`: the file to use for STDIN instead of the contents of this file.
- `FLAGS`: additional flags to pass to the executable
- `ERROR`: the expected return value from the executable, defaults to 0
- `SLOW`: if defined, this test's timeout is doubled.
- `SKIP`: if defined, this test is not run. You can use the value as a comment.
- `TODO`,`NOTE`: useful place to put additional info about the test.

The currently supported list of tools:

- `wast2wasm`: runs `wast2wasm`
- `run-roundtrip`: runs the `run-roundtrip.py` script. This does a roundtrip
  conversion using `wast2wasm` and `wasm2wast`, making sure the .wasm results
  are identical.
- `run-interp`: runs the `run-interp.py` script, running all exported functions
- `run-interp-spec`: runs the `run-interp.py` script with the `--spec` flag

When you first write a test, it's easiest if you omit the expected stdout and
stderr. You can have the test harness fill it in for you automatically. First
let's write our test:

```sh
$ cat > test/my-awesome-test.txt << HERE
;;; TOOL: run-interp-spec
(module
  (export "add2" 0)
  (func (param i32) (result i32)
    (i32.add (get_local 0) (i32.const 2))))
(assert_return (invoke "add2" (i32.const 4)) (i32.const 6))
(assert_return (invoke "add2" (i32.const -2)) (i32.const 0))
HERE
```

If we run it, it will fail:

```
- my-awesome-test.txt
  STDOUT MISMATCH:
  --- expected
  +++ actual
  @@ -0,0 +1 @@
  +2/2 tests passed.

**** FAILED ******************************************************************
- my-awesome-test.txt
[+0|-1|%100] (0.03s)
```

We can rebase it automatically with the `-r` flag. Running the test again shows
that the expected stdout has been added:

```console
$ test/run-tests.py my-awesome-test -r
[+1|-0|%100] (0.03s)
$ test/run-tests.py my-awesome-test
[+1|-0|%100] (0.03s)
$ tail -n 3 test/my-awesome-test.txt
(;; STDOUT ;;;
2/2 tests passed.
;;; STDOUT ;;)
```

## Sanitizers

To build with the [LLVM sanitizers](https://github.com/google/sanitizers),
append the sanitizer name to the target:

```console
$ make clang-debug-asan
$ make clang-debug-msan
$ make clang-debug-lsan
$ make clang-debug-ubsan
```

There are configurations for the Address Sanitizer (ASAN), Memory Sanitizer
(MSAN), Leak Sanitizer (LSAN) and Undefine Behavior Sanitizer (UBSAN). You can
read about the behaviors of the sanitizers in the link above, but essentially
the Address Sanitizer finds invalid memory accesses (use after free, access
out-of-bounds, etc.), Memory Sanitizer finds uses of uninitialized memory, 
the Leak Sanitizer finds memory leaks, and the Undefined Behavior Sanitizer
finds undefined behavior (surprise!).

Typically, you'll just want to run all the tests for a given sanitizer:

```console
$ make test-asan
```

You can also run the tests for a release build:

```console
$ make test-clang-release-asan
...
```

The Travis bots run all of these tests (and more). Before you land a change,
you should run them too. One easy way is to use the `test-everything` target:

```console
$ make test-everything
```

To run everything the Travis bots do, you can use the following scripts:

```console
$ CC=gcc scripts/travis-build.sh
$ CC=gcc scripts/travis-test.sh
$ CC=clang scripts/travis-build.sh
$ CC=clang scripts/travis-test.sh
```
