set(PENTPRIM_CORE_SOURCES
    driver.c
    device.c
    object.c
    plib.c
    pstate.c
    sbuffer.c
    match.c
    work.c
    frcp.c
    rcp.c
    dither.c
    p_piz.c
    p_pi.c
    l_piz.c
    l_pi.c
    pentprim_util_c.c
    pentprim_rast_c.c
    pentprim_rast_stubs.c
)

add_library(pentprim STATIC ${PENTPRIM_CORE_SOURCES})
brender_apply_common_target_settings(pentprim)
target_include_directories(pentprim PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(pentprim PUBLIC brender_core brmt)
target_compile_definitions(pentprim PRIVATE
    BrDrv1Begin=BrDrv1SoftPrimBegin
    PARTS=0xffff
    AUTOLOAD=0
    USE_MMX=0
)
