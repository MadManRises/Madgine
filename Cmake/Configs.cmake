

file(STRINGS ${MADGINE_SOURCE}/Configs.txt __configs)

set (configs "")

foreach (config_string ${__configs})
  if ("${config_string}" MATCHES "([^:]*) : ([^:]*)")
    string(REPLACE " " "" config "${CMAKE_MATCH_1}")
    set (configs ${configs} ${config})
    set (CONFIG_${config}_GENERATOR ${CMAKE_MATCH_2})
    set (CONFIG_${config}_NAME ${CMAKE_MATCH_1})
  endif()
    
endforeach()
