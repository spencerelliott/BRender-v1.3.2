file(GLOB BRENDER_SOFTREND_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

list(FILTER BRENDER_SOFTREND_SOURCES EXCLUDE REGEX ".*/softrend_asm_c\\.c$")
list(APPEND BRENDER_SOFTREND_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/softrend_asm_c.c")

add_library(softrend STATIC ${BRENDER_SOFTREND_SOURCES})
brender_apply_common_target_settings(softrend)
target_include_directories(softrend PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(softrend PUBLIC brender_core)
target_compile_definitions(softrend PRIVATE BrDrv1Begin=BrDrv1SoftRendBegin)
