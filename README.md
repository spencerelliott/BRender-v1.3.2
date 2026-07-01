# Argonaut Blazing Render (BRender)

This is the source code to version 1.3.2 of the BRender engine by Argonaut Software/Argonaut Games, from approximately 1998/11/17.

See **[BUILDING.md](BUILDING.md)** for CMake build instructions on modern Linux/macOS/POSIX systems.

See **[PORTING_PLAN.md](PORTING_PLAN.md)** for the CMake port roadmap, completed work, and agent handoff notes.

## CMake port (work in progress)

A portable **C-only** build path is being added alongside the original DMAKE project files:

- Core libraries (`brfw`, `brhost`, `brst`, `brpm`, `brmt`, `brdb`, `brfm`) build with CMake 3.20+.
- The **software renderer** driver (`softrend`) builds with C geometry helpers; legacy x86 ASM is off by default.
- The **software primitives** driver (`pentprim`) builds with a small set of C triangle rasterizers; remaining ASM entry points are stubbed.
- Tests: `br_begin_test` (init), `br_softrend_test` (driver registration), `br_pentprim_rast_test` (offscreen rasterizer).
- **Not yet available:** POSIX display output, full `pentprim` rasterizer port, and fixed-point CMake builds.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

The original DOS/Windows DMAKE build remains in the tree for reference.

See also [the release of a 1997 version of the BRender engine](https://github.com/foone/BRender-1997).

## Source of this code

This was given to Foone Turing by a programmer who wishes to remain anonymous, based on a game project that had licensed BRender for use.

## Approval to open source this software

Approval to open source this was given in an email from Jez San, former CEO of Argonaut. 

## Credits

| Role          | Name           | 
| ------------- | -------------  |
| Programming   | `Sam Littlewood` `Dan Piponi` `Simon Everett` `Philip Pratt` |
| Ports         | `Simon Everett` `Philip Pratt` `Patrick Buckland` `Zareh Johannes`  `Anthony Savva` `Stainless Software Ltd`    |
| Testing       | `Philip Pratt`  |
| Project Management | `Stefano Zammattio` |
| Technical Reference Manual | `Crosbie Fitch` `Philip Pratt` |
| Tutorial & Installation Guides | `Robbie McQuaid` | 
| Marketing | `Paul Ayscough` `Mitra Faulkner` |
| Sales | `Edwin Masterson` |
| Artwork| `Michel Bowes` |
| Technical Support | `Neela Dass` `Tony Roberts` `John Gay` `Jon Wolff` |
| With Thanks to | `Pete Warnes` `Mike Day` `Tim Gummer` `Vinay Gupta` `Marcia Petterson` `Jez San` | 

(From the 1996 Technical Reference Manual. More names will be added if they are uncovered)

The open sourcing of this version was done by Scott Hanselman and Foone Turing, with help from an anonymous contributor. 

## License

This is released under the MIT license.


Copyright (c) 1998 Argonaut Software Limited 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
