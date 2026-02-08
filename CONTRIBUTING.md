# Contributing to libxmp

The libxmp development team welcomes contributions that follow the
guidelines of this document. Please read the entire document. Contributors
are expected to assign copyright/licensing to the libxmp development team
and to follow the policies detailed below.

* Do NOT contribute code, issues, pull requests, or documentation created
  using LLMs, "coding agents", "chat bots", or other generative "AI" software.

* Use clean-room reverse engineering principles wherever possible.


## How should I contribute code?

The current preferred manner of contribution is Git, via GitHub.
Detailed instructions are available at the end of this document.


## License and Copyright Assignment

During the pull request process, you will be prompted to assign the copyright
of your contributed code to one of the existing license holders under
the ***MIT license*** (also referred to as the ***Expat License*** by FSF):

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

This license should be placed at the top of new source code files
(substituting the copyright year as needed). For contributions to existing
source files, copyright will be assigned to the existing copyright holder(s).

### Exception: "Contributed" external libraries

External libraries that have been included as "contributed" code in
libxmp (including Lhasa, xz, miniz, and stb_vorbis) should retain their
original copyrights. Do not attempt to relicense such code to use libxmp's
license. These third-party licenses should be stated in `docs/CREDITS`.


## Reverse engineering policy

libxmp's tracker playback compatibility is largely based on clean-room
reverse engineering principles, such as black box testing and "wall"
communication. All contributions should follow clean-room principles,
unless otherwise not possible (our judgment call, not yours).

https://en.wikipedia.org/wiki/Clean-room_design

Specifically, sources that we do NOT consider to be clean, unless
the original copyright holders have expressly given permission
(proof will be requested):

* FOSS/OSS code with licenses more restrictive than the MIT and BSD-3
  licenses, including the (L)GPL, MPL, BSD-4, Creative Commons, and
  licenses with non-commercial clauses or ANY other requirement
  incompatible with the GNU General Public License versions 2 and 3.
  Here is a helpful list of licenses, if you aren't sure:
  https://www.gnu.org/licenses/license-list.en.html

* Software with contradictary license terms; for example, if the
  developer claims "public domain" in one place, but then states
  usage conditions elsewhere.

* Source code with a proprietary license, such as "source available",
  "all rights reserved", or source code with no stated license
  (typically, this means "all rights reserved").

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
contributing to libxmp is prohibited:

* libxmp and xmp are written in C and C++, which are notoriously unsafe
  languages. The vast majority of training data on these languages are
  also unsafe, sometimes in subtle ways. An LLM is not a thinking
  person that can reason through a solution; it is a stochastic
  algorithm that produces output that statistically looks like it
  "should be" correct. It can not be trusted to write safe C or C++,
  and our time would be better spent doing things other than debugging
  verbose automatically generated code.

* Likewise, "agent" issues, pull requests, and documentation are
  almost always overly verbose and often contain factual errors.
  We don't want to read or review LLM generated text. If there is
  something worth saying, please write it yourself.

* The license of LLM output is fundamentally indeterminate. It is
  not "public domain", as some claim, and there is no legal precedent
  for this claim—or *any* other claim—as it hasn't been sufficiently
  litigated (as of the writing of this document).

  libxmp prefers a sane and logical interpretation of the license of
  LLM training data and output: the output is a derivative work of the
  model, and the model is a derivative work of its training data, and
  thus the former are subjects to the license(s) of the latter.

  Under this interpretation, there is *no* coding model that we are
  aware of with a clean training set; they *all* contain code with
  licenses ranging from permissive to proprietary to copyleft,
  particularly *mutually exclusive licenses* which are incompatible
  with our license requirements stated above. Thus, *all* LLM output
  passing the copyrightability threshold is tainted and unsuitable
  for libxmp.

In a small number of situations, the output of an LLM ***may*** be
acceptable:

* Code generation that fails to pass the copyrightability threshold:
  in other words, small scale/single line autocomplete.

* Machine translation between languages for the purposes of developer
  communication. (This doesn't mean "ask Claude to write for you".)

* *Trusted* code scanning utilities may be able to provide useful
  feedback, but they should always be checked by a human, shouldn't
  block merging or spam excess comments if used in CI, and shouldn't
  profit or train off of our data.


## Using Git CLI and GitHub to contribute

If you already know how to use Git CLI (or your IDE's Git integration),
you can skip most of this section.

Create a GitHub account, add your SSH key, fork the libxmp repository
to your account, and clone it locally.

```sh
git clone git@github.com:MyAccount/libxmp.git

# If the default SSH port is blocked by your network, use this instead:
git clone ssh://git@ssh.github.com:443/MyAccount/libxmp.git
```

Create a new branch for the changes you are contributing:

```sh
git checkout -b my-patch
```

When making changes to the libxmp source code, you should verify that
your changes don't break any intended behavior using libxmp's built-in
regression testing system. The basic test files are included in both
source distributions and the Git repository; however, the full regression
test suite is *only* included in the Git repository.

libxmp uses GitHub Actions to automatically run the regression tests
for branches and pull requests, but it is still useful to run them yourself
before contributing. See BUILD.md for more information.

After making changes, commit them, and then push the branch to your
GitHub account's fork.

```sh
# Stage modified and new files for commit using "git add".
# Some types of files, such as .MOD, must be added with the force (-f) option.
git add file_1 file_2 file_3
git add -f my_module.mod

# You will be prompted to write a commit message.
# The first line should be a short commit summary of about 64
# characters or fewer; leave the second line blank, and then write
# a more detailed summary (if needed) on the following lines.
# These lines should be wrapped to 72 characters.
git commit

# Push your new commit(s) to your remote GitHub repository.
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
