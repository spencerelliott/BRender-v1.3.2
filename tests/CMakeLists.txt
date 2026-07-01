add_executable(br_begin_test br_begin_test.c)
target_link_libraries(br_begin_test PRIVATE brender_core m)
brender_apply_common_target_settings(br_begin_test)
target_include_directories(br_begin_test PRIVATE ${BRENDER_PUBLIC_INCLUDES})

add_test(NAME br_begin_test COMMAND br_begin_test)

if(BRENDER_BUILD_SOFTREND AND BRENDER_BUILD_PENTPRIM)
    add_executable(br_softrend_test br_softrend_test.c)
    target_link_libraries(br_softrend_test PRIVATE brender_core softrend pentprim m)
    brender_apply_common_target_settings(br_softrend_test)
    target_include_directories(br_softrend_test PRIVATE ${BRENDER_PUBLIC_INCLUDES})
    add_test(NAME br_softrend_test COMMAND br_softrend_test)
endif()

if(BRENDER_BUILD_PENTPRIM)
    add_executable(br_pentprim_rast_test br_pentprim_rast_test.c)
    target_link_libraries(br_pentprim_rast_test PRIVATE pentprim m)
    brender_apply_common_target_settings(br_pentprim_rast_test)
    target_include_directories(br_pentprim_rast_test PRIVATE
        ${BRENDER_PUBLIC_INCLUDES}
        "${CMAKE_SOURCE_DIR}/drivers/pentprim"
    )
    add_test(NAME br_pentprim_rast_test COMMAND br_pentprim_rast_test)
endif()
