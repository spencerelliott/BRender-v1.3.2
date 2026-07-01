set(BRENDER_FW_SOURCES
    fw/brbegin.c
    fw/brbhook.c
    fw/brlists.c
    fw/brqsort.c
    fw/bswap.c
    fw/datafile.c
    fw/devlist.c
    fw/devsetup.c
    fw/diag.c
    fw/error.c
    fw/file.c
    fw/fwsetup.c
    fw/genfile.c
    fw/image.c
    fw/lexer.c
    fw/loader.c
    fw/mem.c
    fw/object.c
    fw/objectc.c
    fw/pattern.c
    fw/pool.c
    fw/printf.c
    fw/register.c
    fw/resource.c
    fw/resreg.c
    fw/scratch.c
    fw/token.c
    fw/tokenval.c
    fw/assocarr.c
    fw/sys_conf.c
    fw/devquery.c
    fw/blockops.c
    fw/magicsym_c.c
)

add_library(brfw STATIC ${BRENDER_FW_SOURCES})
brender_apply_common_target_settings(brfw)
target_include_directories(brfw PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fw")
target_link_libraries(brfw PUBLIC brst brhost brpm)

add_library(brst STATIC
    std/stddiag.c
    std/stdmem.c
    std/stdfile.c
    std/stdlib.c
    std/math.c
    std/logwrite.c
    std/brexcept.c
)
brender_apply_common_target_settings(brst)

add_library(brhost STATIC
    host/himage.c
    host/hook.c
    host/hostcfg.c
    host/hstsetup.c
    host/memmgmt.c
    host/ports.c
    host/host_posix.c
)
brender_apply_common_target_settings(brhost)
target_include_directories(brhost PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/host")
if(UNIX)
    target_link_libraries(brhost PUBLIC ${CMAKE_DL_LIBS})
endif()

add_library(brpm STATIC
    pixelmap/fontptrs.c
    pixelmap/genclip.c
    pixelmap/gencopy.c
    pixelmap/pixelmap.c
    pixelmap/pmdsptch.c
    pixelmap/pmfile.c
    pixelmap/pmgen.c
    pixelmap/pmmem.c
    pixelmap/pmnull.c
    pixelmap/pmsetup.c
    pixelmap/quantize.c
    pixelmap/scale.c
    pixelmap/mipsupt.c
    pixelmap/memloops_c.c
    pixelmap/fonts_c.c
)
brender_apply_common_target_settings(brpm)
target_include_directories(brpm PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pixelmap")

add_library(brmt STATIC
    math/angles.c
    math/fixed.c
    math/fixed_div_c.c
    math/matrix23.c
    math/matrix34.c
    math/matrix4.c
    math/plane.c
    math/quat.c
    math/scalar.c
    math/transfrm.c
    math/vector.c
)
brender_apply_common_target_settings(brmt)

add_library(brdb STATIC
    v1db/actsupt.c
    v1db/custsupt.c
    v1db/dbsetup.c
    v1db/def_mat.c
    v1db/def_mdl.c
    v1db/def_otab.c
    v1db/enables.c
    v1db/ffhooks.c
    v1db/matsupt.c
    v1db/modrend.c
    v1db/modsupt.c
    v1db/otable.c
    v1db/pick.c
    v1db/prelight.c
    v1db/prepmap.c
    v1db/prepmatl.c
    v1db/prepmesh.c
    v1db/preptab.c
    v1db/regsupt.c
    v1db/render.c
    v1db/v1dbfile.c
    v1db/dbquery.c
)
brender_apply_common_target_settings(brdb)
target_link_libraries(brdb PUBLIC brfw brmt brpm)

add_library(brfm STATIC
    fmt/load3ds.c
    fmt/loadasc.c
    fmt/loadbmp.c
    fmt/loadgif.c
    fmt/loadiff.c
    fmt/loadnff.c
    fmt/loadscr.c
    fmt/loadtga.c
    fmt/savescr.c
    fmt/loadvue.c
)
brender_apply_common_target_settings(brfm)

add_library(brender_core INTERFACE)
target_link_libraries(brender_core INTERFACE ${BRENDER_CORE_LINK_LIBS})
target_include_directories(brender_core INTERFACE ${BRENDER_PUBLIC_INCLUDES})
