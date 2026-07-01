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

Example with all options explicit:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBRENDER_FLOAT=ON \
  -DBRENDER_USE_ASM=OFF \
  -DBRENDER_BUILD_TESTS=ON \
  -DBRENDER_BUILD_SOFTREND=ON
```

## Libraries produced

The CMake build produces these static libraries under `build/core/` (and `build/drivers/softrend/` when enabled):

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

Geometry paths in `softrend` use existing C fallbacks where available (`v1model.c`, `genrend.c`, etc.).

### Not yet ported

- **`pentprim` rasterizers** — hundreds of x86 ASM triangle render entry points; the largest remaining porting effort.
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
