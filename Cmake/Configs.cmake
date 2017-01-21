

file(STRINGS ${MADGINE_SOURCE}/Configs.txt __configs)

set (configs "")

foreach (config_string ${__configs})
  if ("${config_string}" MATCHES "([^|]*) \\| ([^|]*)")
    string(REPLACE " " "" config "${CMAKE_MATCH_1}")
    set (configs ${configs} ${config})
    set (CONFIG_${config}_FLAGS ${CMAKE_MATCH_2})
    separate_arguments(CONFIG_${config}_FLAGS UNIX_COMMAND ${CONFIG_${config}_FLAGS})
    set (CONFIG_${config}_NAME ${CMAKE_MATCH_1})
    MESSAGE(STATUS "Read ${CONFIG_${config}_NAME} with flags ${CONFIG_${config}_FLAGS}")
  endif()
    
endforeach()
