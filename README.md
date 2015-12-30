<p style="text-align: center">
<img src="https://raw.githubusercontent.com/indiedotkim/LibDocument/master/logo/LibDocument-600.png" width="300" hspace="25" />
</p>

Building LibDocument
--------------------

### Xcode (Mac OS X)

#### Prerequisites

    brew install python3
    brew install pyenv
    pyenv install 3.5.0
    pyenv local 3.5.0
    eval "$(pyenv init -)"

#### Compiling & Testing

> **Note:** Python support can be removed for iOS builds using the `LDOC_NOPYTHON` compiler flag.
>
> In Xcode, go to your project settings, choose the "Build Settings" tab, scroll down to the "Apple LLVM - Preprocessing" section, expand the node "Preprocessor Macros" and add to "Debug"/"Release" the flag `LDOC_NOPYTHON=1`.

Setting path to modules for Python 3 and running Xcode in that environment:

    PYTHONPATH=`pwd`/test ./xcode.sh

Open generated file `libdocument.xcodeproj` in Xcode.

Build targets in order:

1. `python`
2. `googletest-lib` (build twice; Xcode mistakingly reports some output as errors)
3. `tests`

Unit tests:

-  run `tests` target

#### Building Documentation

Install appledoc:

    brew install appledoc

Set up a new build target in Xcode as described [here](https://github.com/tomaz/appledoc/blob/master/XcodeIntegrationScript.markdown).


### Make (Linux & Mac OS X)

    cmake -G 'Unix Makefiles'

### Common Errors

    Fatal Python error: Py_Initialize: unable to load the file system codec
    ImportError: No module named 'encodings'

