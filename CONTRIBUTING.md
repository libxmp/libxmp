# Contributing to libxmp

The libxmp development team welcomes contributions that follow the
guidelines of this document.


## How should I contribute code?

The current preferred manner of contribution is Git, via GitHub.
Detailed instructions are available at the end of this document.
Please read the entire document. Contributors are expected to
assign copyright/licensing to the libxmp development team and to
follow the reverse engineering and LLM usage policies detailed below.


## License and Copyright Assignment

You will be prompted to copyright assign all code that passes the
copyrightability threshold to one of the existing license holders
under the ***MIT license*** during the pull request process, usually
this specific instance of it (substituting copyright year as needed):

```C
/* Extended Module Player
 * Copyright (C) 1996-2026 Claudio Matsuoka and Hipolito Carraro Jr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
```

This license should be placed at the top of new files. For
contributions to existing files, copyright will be assigned to the
existing copyright holder(s).

### Exception: "Contributed" external libraries

External libraries that have been included as "contributed" code in
libxmp, including Lhasa, xz, miniz, and stb-vorbis all should retain
their original copyrights. Do not attempt to relicense such code to
use libxmp's license. These third-party licenses should be stated in
`docs/CREDITS`.


## Reverse engineering policy

libxmp's tracker playback compatibility is largely based on clean-room
reverse engineering principles, such as black box testing, "wall"
communication. All contributions should follow clean-room principles,
unless otherwise not possible (our judgment call, not yours).

https://en.wikipedia.org/wiki/Clean-room_design

Specifically, sources that we do NOT consider to be clean, unless
the original copyright holders have expressly given permission
(proof will be requested):

* FOSS/OSS code with licenses more restrictive than the MIT or BSD-3
  licenses. These licenses include the MPL, (L)GPL, BSD-4, Creative
  Commons (this should never be used on code), licenses with
  non-commercial clauses, and ANY type of license that is incompatible
  with the GNU General Public License versions 2 and 3. Here is a
  helpful list of licenses, if you aren't sure:
  https://www.gnu.org/licenses/license-list.en.html

* Software with contradictary license terms; for example, if the
  developer claims "public domain" in one place, but then states
  usage conditions elsewhere that are incompatible with code in
  the public domain.

* Source code with a proprietary license, such as "source available",
  "all rights reserved", or source code with no stated license (this
  implicitly means "all rights reserved").

* Disassemblies and decompilations are equivalent in license to their
  source binary. The (rare) exception is clean-room decompilation
  efforts made by writing clean source code and comparing the compiled
  binary output to the original binary.

Small amounts of proprietary code *may* be allowed if they are
absolutely necessary for compatibility (protected usage) or fail to
meet the requirements for copyrightability. In this case, please
clearly state where and how proprietary code was used.


## LLM policy

The usage of LLMs, "coding agents", "chat bots", et al. when
contributing to libxmp, outside of a handful of edge cases, is
forbidden:

* libxmp and xmp are written in C, which is a notoriously unsafe
  language. The vast majority of training data on this language is
  also unsafe, sometimes in subtle ways. An LLM is not a thinking
  person that can reason through a solution; it is a stochastic
  algorithm that produces output that statistically looks like it
  "should be" correct. It can not be trusted to write safe C, and
  our time would be better spent doing things other than debugging
  verbose automatically generated code.

* Likewise, automated "agent" issues and patches are almost always
  a huge waste of time for developers. We don't want to read LLM
  generated text, either.

* The license of LLM output is fundamentally indeterminate. It is
  not "public domain", as some claim, and there is no legal precedent
  for this claim—or *any* other claim—as it hasn't been sufficiently
  litigated (as of the writing of this document).

  A sane and logical interpretation of the license of LLM training
  data (and therefore its output) is that it is a derivative of all
  input data, and thus subject to the same licenses as the input data.
  Under this interpretation, there is *no* coding model that we are
  aware of with a clean training set; they *all* contain code with
  licenses ranging from permissive to proprietary to copyleft,
  particularly, *mutually exclusive licenses* which are incompatible
  with our license requirements stated above. It is because of this
  that we consider *all* LLM output that passes the copyrightability
  threshold to be tainted and unsuitable for libxmp.

* If you don't enjoy software engineering enough or don't care about
  tracker module software enough to write your own code, there's a
  decent chance you're also not going to check your LLM's code for
  errors before contributing it here. :)

In a small number of situations, the output of an LLM is considered
acceptable:

* Code generation that fails to pass the copyrightability threshold,
  in other words, small scale/single line autocomplete.

* Machine translation between languages for the purposes of developer
  communication. (User-facing translations are better performed by a
  human.)

* *Trusted* code scanning utilities may be able to provide useful
  feedback in CI, provided they don't block merging, spam excess
  comments, or sell our data.


## Using of GitHub to contribute (CLI, GCC/Clang)

Create a GitHub account, add your SSH key, fork the libxmp repository
to your account, clone it locally, and create a new branch for your
contribution.

```sh
git clone git@github.com:MyAccount/libxmp.git
cd libxmp
```

libxmp can be compiled with Autoconf and GNU Make (or CMake), a POSIX shell,
and a C compiler.

```sh
autoconf
./configure
make -j8

# CMake
mkdir builddir
cmake -B builddir -S . -DWITH_UNIT_TESTS=ON
cmake --build builddir
```

While making changes to the libxmp source code, you should verify
them against libxmp's built-in regression testing system. The basic
test files are included in both source distributions and the Git
repository; however, the full regression test suite is *only*
included in the Git repository.

```sh
# Basic test (Autoconf)
make test

# Full regression test suite (Autoconf)
(cd test-dev && autoconf && ./configure)
make devcheck -j8

# Full regression test suite (CMake)
(cd builddir && ctest)
```

Create a new branch for your contribution:

```sh
git checkout -b my-patch
```

After making changes, commit them, and then push the branch to your
GitHub account's fork.

```sh
# You will be prompted to write a commit message.
# The first line should be a short commit summary of about 64
# characters or fewer; leave the second line blank, and then write
# a more detailed summary (if needed) on the following lines.
# These lines should be wrapped to 72 characters.
git commit -a

git push --set-upstream origin my-patch
```

Navigate to your GitHub libxmp fork in your web browser of choice,
find your branch on the Branches page, click the "..." button next
to the `my-patch` branch to open the menu, and select "New pull
request". The pull request template will prompt you to assign
copyright, to verify that you have followed the guidelines of
this document, and to explain in detail any deviations from these
guidelines (if any); you can type `x` into the checkboxes, or click
them after making the pull request.
