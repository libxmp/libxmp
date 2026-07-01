# Building libxmp

This document covers build commands to compile libxmp.
See CONTRIBUTING.md for guidelines on how to contribute patches.

This document doesn't cover every configuration option for libxmp.
See the `./configure` help text, CMakeLists.txt, or the relevant
Makefile.(arch) for more information.


## Autoconf/GNU Make (CLI)

libxmp can be compiled with Autoconf and GNU Make, a POSIX shell, and GCC
(or clang). This is libxmp's primary build system, and is recommended for
development on Linux/BSD, macOS, Haiku, and others.

(MSYS2 users will get better compilation times from CMake.)

```sh
# Configure libxmp
./autogen.sh
./configure

# Build and test libxmp
make
make check

# Run regression tests (only works when building from the Git repository)
(cd test-dev && autoconf && ./configure)
make devcheck
```

GNU Make supports parallel compilation via the command line option `-j#`, where
`#` is the number of compilation workers. This can be used reduce compilation
time. If `#` is omitted, GNU Make will spawn as many workers as possible.

```sh
# Build with 8 workers
make -j8
```

### Debug printing

To enable debug printing (warning: it is extremely verbose and should not
be enabled for CI), edit Makefile, uncomment this line, and rebuild:

```Makefile
#CFLAGS += -DDEBUG
```

### libxmp-lite (optional)

libxmp-lite can be built alongside libxmp using the following commands.
(If you are building libxmp-lite from its release tarball, just use the
same instructions as above.)

```sh
./configure --enable-lite
make lite
make check-lite
```

### Static library (optional)

Provide the `--enable-static` option to `./configure`, then build as usual:

```sh
./configure --enable-static
```

The shared library can also optionally be disabled with `--disable-shared`:

```sh
./configure --enable-static --disable-shared
```

### Installing libxmp

This build system can install the library (or libraries) to the `--prefix`
provided to `./configure` (by default, `/usr/local`).

Combine with `doas` or `sudo` as-needed:

```sh
make install
```


## CMake (CLI)

libxmp can also be built using CMake. This build system supports GCC, clang,
and Microsoft Visual C++ (MSVC).

```sh
# Configure libxmp
mkdir builddir
cmake -B builddir -S . -DCMAKE_BUILD_TYPE=Release
# When building from the Git repository, use this to enable regression tests.
cmake -B builddir -S . -DCMAKE_BUILD_TYPE=Release -DWITH_UNIT_TESTS=ON

# Build libxmp
cmake --build builddir

# Test libxmp and run regression tests (if enabled)
(cd builddir && ctest --output-on-failure)
```

For MSVC, the configuration must be provided at build time:

```sh
# Build libxmp
cmake --build builddir --config Release

# Test libxmp and run regression tests (if enabled)
cd builddir
ctest -C Release --output-on-failure
```

Like GNU Make, CMake supports parallel compilation via the command line
option `-j#` to reduce compilation time:

```sh
# Build with 8 workers
cmake --build builddir -j8
```

### Debug printing

To enable debug printing (warning: it is extremely verbose and should not
be enabled for CI), reconfigure with this build type and rebuild.

```sh
cmake -B builddir -S . -DCMAKE_BUILD_TYPE=Debug
```

For MSVC, the configuration must be provided at build time:

```sh
# Build libxmp
cmake --build builddir --config Debug

# Test libxmp and run regression tests (if enabled)
cd builddir
ctest -C Debug --output-on-failure
```

### libxmp-lite (optional)

libxmp-lite can be built alongside libxmp using the following commands.
(If you are building libxmp-lite from its release tarball, just use the
same instructions as above.)

```sh
cmake -B builddir -S . -DBUILD_LITE=ON
cmake --build builddir
(cd builddir && ctest)
```

### Static library (optional)

The CMake build system builds both the static and shared libraies by default
for most platforms.

The static library can optionally be disabled with `BUILD_STATIC`:

```sh
cmake -B builddir -S . -DBUILD_STATIC=OFF
```

The shared library can optionally be disabled with `BUILD_SHARED`:

```sh
cmake -B builddir -S . -DBUILD_SHARED=OFF
```

### Installing libxmp

This build system can install the library (or libraries).

Combine with `doas` or `sudo` as-needed:

```sh
cmake --install builddir
```

By default, this installs to `/usr/local`. The install prefix can be
customized by providing the option `--prefix`:

```sh
cmake --install builddir --prefix /path/to/my/prefix
```


## Microsoft Visual C++ (MSVC) with NMAKE (CLI)

libxmp does not contain a Visual Studio project, but supports MSVC via
NMAKE (or CMake, see above). This build system has no configuration step.

```sh
# Make and check libxmp
nmake -f Makefile.vc
nmake -f Makefile.vc check

# Run regression tests (NMAKE)
cd test-dev
nmake -f Makefile.vc

# libxmp-lite (optional)
nmake -f Makefile.vc lite
nmake -f Makefile.vc check-lite
```

The MSVC toolchain, command line tools, and Windows SDK (and optionally CMake)
can be installed independently of Visual Studio using the Visual Studio
installer. To use these tools, CMD.exe should be launched from one of the
preconfigured shells installed by the Visual Studio installer.

NMAKE does not support parallel compilation. The regression tests system
provides `/MP` to MSVC instead. This option is not used in the library
Makefiles, as it prevents incremental rebuilds. Because of this issue,
CMake may be preferable for development with MSVC.

### Debug printing

To enable debug printing (warning: it is extremely verbose and should not
be enabled for CI), edit Makefile.vc, uncomment this line, and rebuild:

```Makefile
#CFLAGS = $(CFLAGS) /DDEBUG
```


## Open Watcom (CLI)

libxmp has Watcom makefiles for DOS, OS/2, Windows x86, and Linux.
This build system does not support the regression tests, and probably
shouldn't be used for serious development.

### Building for DOS using Open Watcom

```sh
# Make and check libxmp
wmake -f Makefile.dos
wmake -f Makefile.dos check

# libxmp-lite (optional)
wmake -f Makefile.dos lite
wmake -f Makefile.dos check-lite
```

### Building for OS/2 using Open Watcom

```sh
# Make and check libxmp
wmake -f Makefile.os2
wmake -f Makefile.os2 check

# libxmp-lite (optional)
wmake -f Makefile.os2 lite
wmake -f Makefile.os2 check-lite
```

### Building for Windows x86 using Open Watcom

```sh
# Make and check libxmp
wmake -f Makefile.w32
wmake -f Makefile.w32 check

# libxmp-lite (optional)
wmake -f Makefile.w32 lite
wmake -f Makefile.w32 check-lite
```

### Building for Linux using Open Watcom

```sh
# Make and check libxmp
wmake -f Makefile.lnx
wmake -f Makefile.lnx check

# libxmp-lite (optional)
wmake -f Makefile.lnx lite
wmake -f Makefile.lnx check-lite
```

### Debug printing

To enable debug printing (warning: it is extremely verbose and should not
be enabled for CI), edit watcom.mif, uncomment this line, and rebuild:

```Makefile
#CFLAGS += -DDEBUG
```


## GCC/EMX KLIBC for OS/2 (CLI)

This build system does not support the regression tests, and probably
shouldn't be used for serious development.

```sh
# Make and check libxmp
make -f Makefile.emx
make -f Makefile.emx check

# libxmp-lite (optional)
make -f Makefile.emx lite
make -f Makefile.emx check-lite
```

### Debug printing

To enable debug printing (warning: it is extremely verbose and should not
be enabled for CI), edit Makefile.emx, uncomment this line, and rebuild:

```Makefile
#CFLAGS += -DDEBUG
```


## Developer-only targets and tools

The Autoconf/GNU Make build system is currently mandatory for several
development tasks. Even if developing with another build system, you should
have Autoconf and GNU Make available (in Windows, this is usually via MSYS2).

Like the regression tests, several of these require a cloned Git respository,
and will not work if attempted using an extracted release tarball.

### Preparing other build systems

The Autoconf/GNU Make build system is used to regenerate the Makefiles
for every other build system, which should be performed every time a
compilation unit is added or renamed:

```sh
make cmake-prepare
make vc-prepare
make watcom-prepare
make emx-prepare
```

The regression tests system uses automatically constructed source file lists
for CMake and NMAKE. Running the regression tests once with the GNU Make
build system is sufficient to rebuild the list of test source files. Adding
other new source files to the regression tests requires the following:

```sh
# Rebuild test-dev/Makefile.vc
(cd test-dev && make vc-prepare)

# There is currently no target to rebuild test-dev/CMakeLists.txt.
# Base source files must be edited in manually.
```

### Distribution and documentation

Preparing distribution tarballs and rebuilding the documentation is supported
by Autoconf/GNU Make and by CMake.

Required dependencies: `python3-docutils`, `rst2pdf`.

```sh
# Only rebuild the documentation
make docs

# This will rebuild the documentation, prepare the other build systems, and
# then produce a distribution tarball.
make dist

# To test the distribution tarball:
make distcheck

# To make a distribution tarball for libxmp-lite:
make -f Makefile.lite dist
```

### Coverage testing

This is only supported by the Autoconf/GNU Make build system and requires
the test-dev folder.

Required dependencies: `gcovr`, `lcov`.

```sh
# Build the coverage-instrumented library
make coverage

# If not done already:
(cd test-dev && autoconf && ./configure)

# Build and run the regression tests using the coverage-instrumented library
make covercheck
```

The build system also supports building a coverage-instrumented libxmp-lite
library (but with no equivalent regression tests or reporting):

```sh
make coverage-lite
```

### Development tools

libxmp's development tools are part of the regression tests system
and require Autoconf/GNU Make:

```sh
cd test-dev

# If not done already
autoconf && ./configure

make utilities
```

#### gen_module_data

This outputs information about data loaded from a module, and is primarily
used for loader tests (`test_loader_*.c`).

#### gen_mixer_data

This outputs information about module playback and is used by a wide variety
of regression tests.

#### xmpchk

This runs libxmp's fuzz testing routine on one or more modules and is useful
for reproducing crashes found by libFuzzer (see below).

### Fuzz testing

libxmp also supports fuzzing via libFuzzer. Building the fuzzers is performed
with the Autoconf/GNU Make build system, but it also requires CMake (to build
the instrumented libraries in their own directories).

Required dependencies: `clang`, `llvm`, `lld`, `compiler-rt`.

```sh
cd test-dev

# If not done already
autoconf && ./configure

make fuzzers
```

Use `libxmp_fuzz.sh` in the test-dev directory to run the fuzzers:

```sh
# Run one job of the AddressSanitizer + UndefinedBehaviorSanitizer fuzzer:
./libxmp_fuzz.sh asan

# AArch64-only: HWAddressSanitizer + UndefinedBehaviorSanitizer fuzzer:
./libxmp_fuzz.sh hwasan

# Run 16 jobs using 4 workers of the MemorySanitizer fuzzer:
./libxmp_fuzz.sh msan -jobs=16 -workers=4

# Perform a corpus merge using the AddressSanitizer and MemorySanitizer
# fuzzers (outputs to NEW_CORPUS):
./libxmp_fuzz.sh merge
```


## VSCodium / Code-OSS / Visual Studio Code

These IDEs have built-in Git support and can be configured to use Autoconf,
CMake, or MSVC build commands. This requires familiarity with the command
line build instructions. C language support is provided by clangd (or by
Microsoft C/C++ Intellisense for official Microsoft builds of Code).

Use these default settings:

.vscode/settings.json:
```json
{
  "editor.tabSize": 8,
  "editor.insertSpaces": false
}
```

.clangd:
```yaml
Diagnostics:
  # clangd often just gets this plain wrong and also has no clue that other
  # architectures may require some system includes that clangd doesn't.
  UnusedIncludes: None
```

clangd requires a compile_commands.json file at the root of the workspace
to determine the compiler and to resolve include paths.

For Autoconf/GNU Make on Linux et al., this file is generated using the
command line utility `bear`:
```sh
bear -- make -j8
```

For CMake, this file can be generated with the option
`CMAKE_EXPORT_COMPILE_COMMANDS` during configuration:
```sh
cmake -B builddir -S . -DWITH_UNIT_TESTS=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```
