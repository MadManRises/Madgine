
set(MADGINE_AS_SDK TRUE)

if (NOT MADGINE_BINARY_NAME)
	MESSAGE(WARNING "When using the Madgine as SDK please provide a 'MADGINE_BINARY_NAME'!")
	set(MADGINE_BINARY_NAME "MadgineApp")
endif ()

if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/../CMakeLists.txt)
#SDK from Source

	add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/.. Madgine)

else()
#installed SDK

	

endif()