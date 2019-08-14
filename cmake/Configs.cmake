include (Util)

once()

include(Workspace)

if (EXISTS ${Workspace}/cmake/Configs.txt)
    file(STRINGS ${Workspace}/cmake/Configs.txt __configs)
else()
    message(WARNING "/cmake/Configs.txt is not available in Workspace")
    set(__configs )
endif()

set (configs "")

foreach (config_string ${__configs})
  if ("${config_string}" MATCHES "([^|]*) \\| ([^|]*) \\| ([^|]*)")
    string(REPLACE " " "_" config "${CMAKE_MATCH_1}")
    set (configs ${configs} ${config})
    set (CONFIG_${config}_GENERATOR ${CMAKE_MATCH_2})
    set (CONFIG_${config}_FLAGS ${CMAKE_MATCH_3})
    separate_arguments(CONFIG_${config}_FLAGS UNIX_COMMAND ${CONFIG_${config}_FLAGS})
    set (CONFIG_${config}_NAME ${CMAKE_MATCH_1})
    MESSAGE(STATUS "Read ${CONFIG_${config}_NAME} with Generator ${CONFIG_${config}_GENERATOR} and flags ${CONFIG_${config}_FLAGS}")
  endif()
    
endforeach()
