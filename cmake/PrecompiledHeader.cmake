include (Util)

once()

if (EMSCRIPTEN OR NOT COMMAND target_precompile_headers)

function(target_precompile_headers)
endfunction(target_precompile_headers)

endif()

