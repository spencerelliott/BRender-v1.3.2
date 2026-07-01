set(BRENDER_PUBLIC_INCLUDES
    "${BRENDER_SOURCE_DIR}/inc"
    "${BRENDER_SOURCE_DIR}/ddi_inc"
)

function(brender_apply_common_target_settings target)
    target_include_directories(${target}
        PUBLIC
            ${BRENDER_PUBLIC_INCLUDES}
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    target_compile_definitions(${target}
        PRIVATE
            BASED_FLOAT=1
            BASED_FIXED=0
            DEBUG=0
            PARANOID=0
            EVAL=0
            STATIC=static
            ADD_RCS_ID=0
            __BR_POSIX__=1
            BRENDER_USE_ASM=$<BOOL:${BRENDER_USE_ASM}>
    )
    if(UNIX)
        target_compile_definitions(${target} PRIVATE __GNUC__=1)
    endif()
    target_compile_options(${target} PRIVATE
        -Wall
        -Wno-unused-parameter
        -Wno-incompatible-function-pointer-types
    )
    set_target_properties(${target} PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON C_EXTENSIONS ON)
endfunction()
