find_package(Perl REQUIRED)

function(brender_run_tokgen output_dir)
    set(tok_file "${BRENDER_SOURCE_DIR}/core/fw/pretok.tok")
    set(tokgen "${BRENDER_SOURCE_DIR}/make/tokgen.pl")
    set(out_c "${output_dir}/pretok.c")
    set(out_h "${output_dir}/pretok.h")
    set(out_type "${output_dir}/toktype.c")

    add_custom_command(
        OUTPUT ${out_c} ${out_h} ${out_type}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir}
        COMMAND ${Perl_EXECUTABLE} ${tokgen} < ${tok_file}
        WORKING_DIRECTORY ${output_dir}
        DEPENDS ${tok_file} ${tokgen} "${BRENDER_SOURCE_DIR}/core/fw/tokens.db"
        COMMENT "Generating BRender token tables"
        VERBATIM
    )
endfunction()

function(brender_run_resgen base_name fns_file output_dir)
    set(resgen "${BRENDER_SOURCE_DIR}/make/resgen.pl")
    set(out_h "${output_dir}/${base_name}.h")

    add_custom_command(
        OUTPUT ${out_h}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir}
        COMMAND ${Perl_EXECUTABLE} ${resgen} ${out_h} _img.rsp w < ${fns_file}
        DEPENDS ${fns_file} ${resgen}
        COMMENT "Generating ${base_name}.h resident image"
        VERBATIM
    )
endfunction()

function(brender_run_classgen output_dir)
    set(cgh "${output_dir}/dev_objs.cgh")
    set(classgen "${BRENDER_SOURCE_DIR}/make/classgen.pl")
    set(cpp_input "${BRENDER_SOURCE_DIR}/core/fw/dev_objs.hpp")

    add_custom_command(
        OUTPUT ${cgh}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir}
        COMMAND sh -c "${CMAKE_C_COMPILER} -E -D__CLASSGEN__ -I'${BRENDER_SOURCE_DIR}/inc' -I'${BRENDER_SOURCE_DIR}/ddi_inc' '${cpp_input}' | ${Perl_EXECUTABLE} ${classgen} dev_objs > '${cgh}'"
        DEPENDS ${cpp_input} ${classgen}
        COMMENT "Generating dev_objs.cgh"
        VERBATIM
    )
endfunction()
