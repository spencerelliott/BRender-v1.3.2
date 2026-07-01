add_executable(br_begin_test br_begin_test.c)
target_link_libraries(br_begin_test PRIVATE brender_core m)
brender_apply_common_target_settings(br_begin_test)
target_include_directories(br_begin_test PRIVATE ${BRENDER_PUBLIC_INCLUDES})

add_test(NAME br_begin_test COMMAND br_begin_test)
