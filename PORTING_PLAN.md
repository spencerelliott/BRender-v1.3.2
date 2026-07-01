# BRender CMake Port — Progress Plan

Living document for tracking the portable **C-only CMake build** of BRender v1.3.2 on modern POSIX hosts. Update this file at the end of each agent run: check off completed items, note blockers, and record the branch/PR you were working on.

**Related docs:** [BUILDING.md](BUILDING.md) (build instructions), [README.md](README.md) (overview).

---

## Goal

Build and run the BRender software rendering stack on Linux/macOS using **CMake + C90**, without legacy x86 assembly (`BRENDER_USE_ASM=OFF`). Target outcome: initialize the engine, register softrend + pentprim drivers, rasterize geometry offscreen, and eventually display frames on POSIX.

---

## Quick verification (run after every change)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
ctest --test-dir build --output-on-failure
```

**Expected today:** all 3 tests pass.

| Test | Links | What it proves |
|------|-------|----------------|
| `br_begin_test` | `brender_core` | `BrBegin()` / `BrEnd()` |
| `br_softrend_test` | `brender_core softrend pentprim` | `SOFTPRMF` + `SOFTRNDF` devices register; float primitive library + renderer facility found |
| `br_pentprim_rast_test` | `pentprim` | `TriangleRender_Z_I8_D16` writes pixels to a 64×64 buffer (~556 pixels) |

Manual spot-check:

```bash
./build/tests/br_pentprim_rast_test   # prints: pixels=556, ok
./build/tests/br_softrend_test        # prints: softrend devices=2 ..., ok
```

---

## Branch and PR status

| Item | Status |
|------|--------|
| PR #1 — CMake Phase 0 (core + `br_begin_test`) | **Merged** to `main` |
| PR #2 — softrend/pentprim tests + C rasterizers | **Open (draft)** — branch `cursor/softrend-test-pentprim-2eed` |

**Branch naming for new agent work:** `cursor/<descriptive-name>-2eed`

**Rebase note:** PR #2 was rebased onto `main` after PR #1 merged. The branch should contain only the pentprim/test commit(s) on top of `main`, not duplicate Phase 0 history.

---

## Phase 0 — Core CMake skeleton

- [x] Root `CMakeLists.txt`, `cmake/BRenderOptions.cmake`, `core/CMakeLists.txt`
- [x] Static libraries: `brfw`, `brhost`, `brst`, `brpm`, `brmt`, `brdb`, `brfm`
- [x] Aggregate link target `brender_core` with `LINK_GROUP:RESCAN` for correct static link order
- [x] C replacements for small ASM utilities:
  - [x] `core/fw/blockops.c`
  - [x] `core/host/host_posix.c`
  - [x] `core/pixelmap/memloops_c.c`, `fonts_c.c`
  - [x] `core/fw/magicsym_c.c`
- [x] `br_begin_test` smoke test
- [x] `BUILDING.md` and `README.md` (initial)

### Critical fixes already applied (do not regress)

1. **`BrBegin` symbol collision** — `v1db_p.h` macro-renames `BrBegin`/`BrEnd`. Framework entry points in `core/fw/brbegin.c` need `#undef BrBegin` / `#undef BrEnd` before defining the real functions.
2. **64-bit segfault in resource allocator** — GCC/Clang need `#pragma pack(4)` in `inc/compiler.h` and `core/inc/compiler.h` so struct layout matches original 32-bit builds. Without this, `UserToRes()` and pointer-walking code break on 64-bit.
3. **`fw_p.h` struct tags** — Driver-private includes hide some typedefs. Prototypes in `inc/fw_p.h` and `core/inc/fw_p.h` use explicit `struct br_renderer_facility *` etc.
4. **Compile flags** — `cmake/BRenderOptions.cmake` sets `BASED_FLOAT=1`, `__BR_POSIX__=1`, `BRENDER_USE_ASM=OFF` by default.

---

## Phase 1 — Software renderer driver (`softrend`)

- [x] `drivers/softrend/CMakeLists.txt` — globs `*.c`, always includes `softrend_asm_c.c`
- [x] `BRENDER_BUILD_SOFTREND=ON` (default ON)
- [x] C replacement: `drivers/softrend/softrend_asm_c.c` (triangle subdivision; needs `#include "shortcut.h"` for `X`/`Y`/`Z`/`W`/`S0`/`S1` macros)
- [x] `drv_ip.h` — removed duplicate static/non-static prototype clash for `flush`, `synchronise`, `commandMode*`
- [x] `v1model.c` — alias `V1Face_CullOneSidedPerspective_P6` → `V1Face_CullOneSidedPerspective` when ASM P6 path absent
- [x] `br_softrend_test` — static registration via `BrDevAddStatic(BrDrv1SoftPrimBegin)` then `BrDevAddStatic(BrDrv1SoftRendBegin)`

### Driver registration (important for integration tests)

| Symbol | Device name | Role |
|--------|-------------|------|
| `BrDrv1SoftPrimBegin` | `SOFTPRMF` | Primitive library (`Default-Primitives-Float`) |
| `BrDrv1SoftRendBegin` | `SOFTRNDF` | Renderer facility (`Default-Renderer-Float`) |

`BrDevBegin("SOFT")` is **not** the softrend driver — that is the POSIX host default device.

CMake renames driver entry points per target:

```cmake
# softrend: BrDrv1Begin=BrDrv1SoftRendBegin
# pentprim: BrDrv1Begin=BrDrv1SoftPrimBegin
```

### Phase 1 remaining

- [ ] End-to-end render test: create renderer + offscreen pixelmap, draw a triangle, read back pixels
- [ ] Exercise softrend geometry path beyond driver registration (model prep, bucket sort, etc.)
- [ ] Audit remaining softrend ASM references when `BRENDER_USE_ASM=OFF` (e.g. `v1m_386.asm`, `gen_386.asm`, `subdiv.asm`) — currently using C fallbacks or stubs

---

## Phase 2 — Software primitives driver (`pentprim`)

- [x] `drivers/pentprim/CMakeLists.txt`
- [x] `BRENDER_BUILD_PENTPRIM=ON` (default ON)
- [x] Core driver sources compile: `driver.c`, `device.c`, `object.c`, `plib.c`, `pstate.c`, `sbuffer.c`, `match.c`, `work.c`, `frcp.c`, `rcp.c`, `dither.c`, `p_piz.c`, `p_pi.c`, `l_piz.c`, `l_pi.c`
- [x] C utility replacements: `drivers/pentprim/pentprim_util_c.c` (`SafeFixedMac2Div`, `_sar16`, `RasteriseBufferDisable`)
- [x] C math: `core/math/fixed_div_c.c` (`BrFixedDiv`, `BrFixedDivR`, `BrFixedDivF`) — required by rasterizers and softrend
- [x] **3 real C triangle rasterizers** in `drivers/pentprim/pentprim_rast_c.c`:
  - [x] `TriangleRender_Z_I8_D16` — flat indexed colour + 16-bit Z
  - [x] `TriangleRender_ZI_I8_D16` — interpolated intensity + Z
  - [x] `TriangleRender_I8` — flat indexed, no Z
- [x] **166 stub rasterizers** in `drivers/pentprim/pentprim_rast_stubs.c` (auto-generated no-ops so the library links)
- [x] `USE_MMX=0` — MMX tables disabled in CMake build
- [x] `br_pentprim_rast_test` — direct unit test setting global `work` buffer and calling `TriangleRender_Z_I8_D16`
- [x] Compile fixes: `plib.h` (stray control char), `pstate.c` (`BrTokenValueQuery` extra_size arg)

### Pentprim architecture notes

- **~50 ASM files**, **~169 `TriangleRender_*` entry points** total (3 implemented, 166 stubbed; prototypes in `drivers/pentprim/drv_ip.h`).
- Primitive tables live in `prim_*.c` / `prm_*.c` files, **included** from `match.c` (not separate compile units). Float build uses `prim_t8f.c`, `prm_t15f.c`, `prm_t16f.c`.
- Global raster state: `work` struct (`drivers/pentprim/work.h`, `work.c`). Tests must set `work.colour` and `work.depth` before calling rasterizers.
- Vertex components use fixed-point screen coords: `C_SX`, `C_SY`, `C_SZ`, `C_I` via `brp_vertex.comp_x[]`.

### Phase 2 remaining (largest future effort)

- [ ] Port more `TriangleRender_*` rasterizers (priority order suggestion):
  1. Indexed + textured + Z (`TriangleRender_*PIZ*` family) — needed for textured models
  2. True-colour RGB 555/565/888 paths
  3. Perspective-correct texture (`TriangleRender_*P*` / `*PIT*` families)
- [ ] **Trapezoid render paths** — `awtm.c`, `persp.c`, `awtmz.c`, `perspz.c` are **not** in CMake build; they depend on ASM `TrapezoidRender*` symbols (`t_pia.asm`, `t_piza.asm`, etc.)
- [ ] Regenerate `pentprim_rast_stubs.c` via script when new rasterizers are implemented (avoid manual drift)
- [ ] Line/point rasterizers (`LineRender*`, `PointRender*`) — separate from triangle stubs
- [ ] Optional: add `prim_t8f.c`-level integration test that goes through `match.c` primitive selection, not just direct rasterizer calls

### Stub regeneration hint

When adding a real rasterizer, remove its stub from `pentprim_rast_stubs.c`. Long-term, a CMake custom command could diff `drv_ip.h` prototypes against implemented symbols and emit stubs for the rest.

---

## Phase 3 — POSIX display output

- [ ] Framebuffer/window backend (SDL2, X11, or Wayland)
- [ ] `BrDevBegin` path that opens a display and blits pixelmaps
- [ ] Input/event loop hookup (optional)

**Not started.** Core init works; there is no visible window yet.

---

## Phase 4 — Broader port (lower priority / out of scope for now)

- [ ] Fixed-point build (`BASED_FIXED`) via CMake
- [ ] `BRENDER_USE_ASM=ON` path for original x86 ASM (Watcom/NASM toolchain)
- [ ] Legacy hardware drivers: `ddraw`, `3dfx`, `vesa`, `mcga`, `myst_win`
- [ ] DMAKE/`makeall` parity documentation

---

## CMake options reference

| Option | Default | Notes |
|--------|---------|-------|
| `BRENDER_FLOAT` | ON | Only float builds supported in CMake port |
| `BRENDER_USE_ASM` | OFF | Leave OFF for portable build |
| `BRENDER_BUILD_TESTS` | ON | |
| `BRENDER_BUILD_SOFTREND` | ON | |
| `BRENDER_BUILD_PENTPRIM` | ON | |

---

## Key file map (CMake port additions)

```
CMakeLists.txt                          # root options + subdirs
cmake/BRenderOptions.cmake              # common compile defs/includes
core/CMakeLists.txt                     # core libs + fixed_div_c.c
core/math/fixed_div_c.c                 # BrFixedDiv*
core/fw/blockops.c, host/host_posix.c   # Phase 0 C replacements
drivers/softrend/CMakeLists.txt
drivers/softrend/softrend_asm_c.c
drivers/pentprim/CMakeLists.txt
drivers/pentprim/pentprim_rast_c.c       # 3 real rasterizers
drivers/pentprim/pentprim_rast_stubs.c  # 166 stubs
drivers/pentprim/pentprim_util_c.c
tests/CMakeLists.txt
tests/br_begin_test.c
tests/br_softrend_test.c
tests/br_pentprim_rast_test.c
BUILDING.md
```

---

## Suggested next tasks (pick up here)

1. **Merge PR #2** once reviewed — brings pentprim + tests to `main`.
2. **End-to-end offscreen render test** (`tests/br_softrend_render_test.c`):
   - `BrBegin()` → register both drivers → `BrRendererFacilityFind` → create renderer
   - Allocate offscreen `br_pixelmap` (indexed 8 or RGB)
   - Submit a single triangle primitive; read back non-zero pixels
   - Link: `brender_core softrend pentprim`
3. **Port one textured rasterizer** — start with a simple `TriangleRender_*PIZ2TI*` or similar from `prim_l8.c` tables; use existing ASM file as algorithm reference.
4. **Trapezoid stubs** — add no-op `TrapezoidRender*` C stubs so `awtm.c`/`persp.c` can be added to the build, then replace with real C incrementally.
5. **SDL2 display driver** (new `drivers/sdl/` or host-layer extension) — minimal window + blit.

---

## Agent handoff checklist

Before ending a run, please:

- [ ] Run `ctest --test-dir build --output-on-failure` and note results below
- [ ] Update checkboxes in this file for work completed
- [ ] Add a row to **Run log** with branch, PR, and blockers
- [ ] Commit and push; create or update the draft PR

### Run log

| Date | Branch | PR | Agent focus | Tests | Notes |
|------|--------|-----|-------------|-------|-------|
| 2026-07-01 | `cursor/cmake-c-only-phase0-2eed` | #1 | Phase 0 core CMake | `br_begin_test` pass | Merged to `main` |
| 2026-07-01 | `cursor/softrend-test-pentprim-2eed` | #2 | softrend fixes, pentprim Phase 1, 3 tests | all 3 pass | Rebased on `main` after #1 merge |

---

*Last updated: 2026-07-01*
