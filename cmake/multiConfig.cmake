include (Util)

include_guard()


cmake_minimum_Required(VERSION 3.0)


function(multiConfig name)

    if (NOT CONFIG_BUILD)

        include(Configs)
		include(ExternalProject)
    
		set(CMAKE_CACHE_ARGS)

		get_cmake_property(CACHE_VARS CACHE_VARIABLES)
		foreach(CACHE_VAR ${CACHE_VARS})
			if(CACHE_VAR MATCHES "^BUILD_")
				get_property(CACHE_VAR_TYPE CACHE ${CACHE_VAR} PROPERTY TYPE)
				if(CACHE_VAR_TYPE STREQUAL "UNINITIALIZED")
					set(CACHE_VAR_TYPE)
				else()
					set(CACHE_VAR_TYPE :${CACHE_VAR_TYPE})
				endif()
				list(APPEND CMAKE_CACHE_ARGS "-D${CACHE_VAR}${CACHE_VAR_TYPE}=${${CACHE_VAR}}")
			endif()
		endforeach()

		
        foreach (config ${configs})

            set (BUILD_${config} CACHE BOOL FALSE)
  
            if (${BUILD_${config}})

                MESSAGE(STATUS "Building Config: ${config}")

                string(REPLACE " " "_" config_cut "${config}")

                ExternalProject_Add(${name}_${config_cut}  

                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}                                                         
                    CMAKE_GENERATOR ${CONFIG_${config_cut}_GENERATOR} 
                    CMAKE_ARGS
                        -DCONFIG_BUILD:BOOL=TRUE
                        -DCONFIG:STRING=${config}
                        -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}/cross/${config_cut}
						-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
						${CONFIG_${config_cut}_FLAGS}						

					CMAKE_CACHE_ARGS
						${CMAKE_CACHE_ARGS}
						-DWorkspace:PATH=${Workspace}/cross/${config_cut}
						
                               
                )

				ExternalProject_Add_Step(${name}_${config_cut} forceconfigure
		             COMMAND ${CMAKE_COMMAND} -E echo "Force configure of ${name}_${config_cut}"

					 DEPENDEES update
					 DEPENDERS configure
					 ALWAYS 1)

				set_target_properties(${name}_${config_cut} PROPERTIES EXCLUDE_FROM_ALL TRUE)
 
            endif()

        endforeach()
    
	else()

    endif()

endfunction(multiConfig)
