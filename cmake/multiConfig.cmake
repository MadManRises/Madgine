
cmake_minimum_Required(VERSION 3.0)


function(multiConfig name)

    if (NOT CONFIG_BUILD)

        include(Configs)
        include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)    
    
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
                        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} 
                        -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}/Libs/${config_cut}
                               
                )
 
            endif()

        endforeach()
        
    endif()

endfunction(multiConfig)
