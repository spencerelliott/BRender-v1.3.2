# Building BRender

BRender v1.3.2 originally used **DMAKE** with Watcom, MSVC, or Borland on DOS/Windows. This repository adds a **CMake** build path for modern POSIX hosts (Linux, macOS, and similar Unix systems) using **C only** — legacy x86 assembly is disabled by default.

## Requirements

- **CMake** 3.20 or newer
- A **C compiler** with C90 support (GCC, Clang, or Apple Clang)
- **Perl** is optional (codegen helpers exist but pre-generated token files are used in-tree)

## Quick start

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

Tests (when `BRENDER_BUILD_TESTS=ON`):

| Test | Requires | What it checks |
|------|----------|----------------|
| `br_begin_test` | core only | `BrBegin()` / `BrEnd()` |
| `br_softrend_test` | `softrend` + `pentprim` | Registers `SOFTPRMF` / `SOFTRNDF` devices and finds renderer/primitive facilities |
| `br_pentprim_rast_test` | `pentprim` | C rasterizer `TriangleRender_Z_I8_D16` writes pixels to an offscreen buffer |

The smoke test (`br_begin_test`) calls `BrBegin()` / `BrEnd()` and should print:

```
BrBegin
BrBegin=0
BrEnd
BrEnd=0
ok
```

## CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `BRENDER_FLOAT` | `ON` | Build with 32-bit floating-point scalars (`BASED_FLOAT`). Fixed-point builds are not supported by the CMake port yet. |
| `BRENDER_USE_ASM` | `OFF` | Enable legacy x86 assembly paths. Leave off for the portable C build. |
| `BRENDER_BUILD_TESTS` | `ON` | Build and register the `br_begin_test` smoke test with CTest. |
| `BRENDER_BUILD_SOFTREND` | `ON` | Build the software renderer driver (`softrend` static library). |
| `BRENDER_BUILD_PENTPRIM` | `ON` | Build the software primitives driver (`pentprim` static library). |

Example with all options explicit:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBRENDER_FLOAT=ON \
  -DBRENDER_USE_ASM=OFF \
  -DBRENDER_BUILD_TESTS=ON \
  -DBRENDER_BUILD_SOFTREND=ON \
  -DBRENDER_BUILD_PENTPRIM=ON
```

## Libraries produced

The CMake build produces these static libraries under `build/core/` (and `build/drivers/*/` when enabled):

| Target | Contents |
|--------|----------|
| `brfw` | Framework (resources, devices, files, tokens) |
| `brhost` | Host layer (POSIX stubs on `__BR_POSIX__`) |
| `brst` | Standard diagnostics, file I/O, and malloc allocator |
| `brpm` | Pixelmap support |
| `brmt` | Math library |
| `brdb` | V1 database (models, materials, actors) |
| `brfm` | File format loaders (3DS, BMP, GIF, TGA, etc.) |
| `softrend` | Software renderer driver (optional) |
| `pentprim` | Software primitives driver — triangle rasterizers (optional) |

Link against the aggregate interface target `brender_core`, which pulls in all core libraries with correct static link order:

```cmake
target_link_libraries(my_app PRIVATE brender_core m)
```

On Unix, add `m` for the math library. If you use the host layer’s `dlopen` support, also link `${CMAKE_DL_LIBS}` (already propagated via `brhost`).

## Include paths

Public headers live in:

- `inc/` — application-facing API (`brender.h`, etc.)
- `ddi_inc/` — device driver interface headers

Driver code may additionally include files from `drivers/<driver>/`.

## Porting notes

### C-only path (`BRENDER_USE_ASM=OFF`)

Small assembly utilities are replaced with C:

- `core/fw/blockops.c` — block copy/fill
- `core/host/host_posix.c` — POSIX host layer
- `core/pixelmap/memloops_c.c`, `fonts_c.c` — pixelmap helpers
- `drivers/softrend/softrend_asm_c.c` — triangle subdivision helpers
- `core/math/fixed_div_c.c` — `BrFixedDiv` / `BrFixedDivR` / `BrFixedDivF`
- `drivers/pentprim/pentprim_util_c.c` — `SafeFixedMac2Div`, `_sar16`, raster buffer disable
- `drivers/pentprim/pentprim_rast_c.c` — C triangle rasterizers (indexed + Z-buffered paths)
- `drivers/pentprim/pentprim_rast_stubs.c` — no-op stubs for unported `TriangleRender_*` entry points

Geometry paths in `softrend` use existing C fallbacks where available (`v1model.c`, `genrend.c`, etc.).

### Pentprim status (Phase 1)

The original `pentprim` driver has ~210 `TriangleRender_*` ASM entry points. The CMake build links three C rasterizers (`TriangleRender_Z_I8_D16`, `TriangleRender_ZI_I8_D16`, `TriangleRender_I8`) and stubs the rest so the library links. Trapezoid render paths (`awtm.c`, `persp.c`, etc.) are not included yet.

### Not yet ported

- **Remaining `pentprim` rasterizers** — textured, true-colour, and perspective variants (~200 ASM entry points).
- **`pentprim` trapezoid renderers** — `awtm.c`, `persp.c`, `awtmz.c`, `perspz.c` depend on ASM `TrapezoidRender*` symbols.
- **Hardware GPU drivers** (`ddraw`, `3dfx`, etc.) — tied to legacy APIs.
- **Display output on POSIX** — no SDL/X11/Wayland window yet; core init works but there is no framebuffer window.
- **Fixed-point (`BASED_FIXED`) builds** via CMake.

### 64-bit hosts

GCC/Clang builds use `#pragma pack(4)` in `compiler.h` so structures match the original 32-bit layout. This is required for the resource allocator and other pointer-walking code.

## Legacy DMAKE build

The original `makeall` / per-directory `makefile` flow is unchanged and targets DOS/Windows toolchains from the 1990s. It is not required for the CMake path above.

## Troubleshooting

**Link errors mentioning `BrMemSet`, `BrStrLen`, etc.**  
Make sure you link `brender_core` (or `brst` after `brfw`), not individual libraries in the wrong order.

**`BrBegin` undefined or wrong symbol**  
Application code should include `brender.h`. The framework defines `BrBegin` in `core/fw/brbegin.c`; the v1 database wraps it via macros in `v1db_p.h`.

**Softrend compile errors in driver-private headers**  
Driver translation units define `BR_*_PRIVATE` before including `brddi.h`, which hides some typedefs. Prototypes in `fw_p.h` use explicit `struct` tags where needed.
