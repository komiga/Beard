
## Beard

*Beard* is a TUI library for C++11.

## Dependencies

Beard uses:

1. [duct++](https://github.com/komiga/duct-cpp) (HEAD)
2. [ceformat](https://github.com/komiga/ceformat) (HEAD)
3. [All the Maths‽](https://github.com/komiga/am) (HEAD)

See `dep/README.md` for dependency setup.

## Building

Beard is a compiled shared library (by default).

All features support Clang 3.5+ with libc++ (SVN head). GCC is not supported
due to the ceformat dependency. This may be alleviated in the near future, but
Beard is currently in heavy development, so expect things to break often.

Once dependencies are setup (see `dep/README.md`), the library and tests can be
compiled using plash's standard project protocol: http://komiga.com/pp-cpp

## License

*Beard* carries the MIT license, which can be found both below and in the
`LICENSE` file.

```
Copyright (c) 2013 Tim Howard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
