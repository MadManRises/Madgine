include(Util)

once()


macro(compile_shaders target)

    get_property(sources TARGET ${target} PROPERTY SOURCES)
    
    foreach (source ${sources})
        set (compile FALSE)
        
        get_filename_component(ext ${source} EXT)

        if (ext STREQUAL ".PS_hlsl")
            set(compile TRUE)
            set(shadertype ps)
        endif()
        if (ext STREQUAL ".VS_hlsl")
            set(compile TRUE)
            set(shadertype vs)
        endif()
        if (ext STREQUAL ".GS_hlsl")
            set(compile TRUE)
            set(shadertype gs)
        endif()
        if (ext STREQUAL ".sl")
            set(compile TRUE)
            set(shadertype sl)
        endif()
        if (ext STREQUAL ".hlsl")
            set(compile TRUE)
            set_source_files_properties(${source} PROPERTIES VS_TOOL_OVERRIDE "None")
        endif()
        if (compile)
            get_filename_component(name ${source} NAME)
            add_custom_command(TARGET ${target}
                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${source} data/${name}
                MAIN_DEPENDENCY ${source}
                COMMENT "Copying HLSL ${shadertype} shader: ${name}"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                VERBATIM)
        endif()

    endforeach()

endmacro(compile_shaders)