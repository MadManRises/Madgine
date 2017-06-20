cmake_policy(SET CMP0048 NEW)

if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

project(Interfaces_Test)


set(sources "")
FolderSources(sources "" "" integrationtests.cpp)
FolderSources(sources "" network networkmanagertest.cpp)

add_executable(Interfaces_Test ${sources})

add_dependencies(Interfaces_Test googletest)

target_link_libraries(Interfaces_Test Interfaces ${GTEST_LIBS})

MESSAGE(STATUS ${Interfaces_BINARY_DIR})
set_target_properties(Interfaces_Test PROPERTIES 

RUNTIME_OUTPUT_DIRECTORY_DEBUG "$<TARGET_FILE_DIR:Interfaces>")

add_test(
	NAME Interfaces_Test
	COMMAND Interfaces_Test --gtest_output=xml:Interfaces.xml
	WORKING_DIRECTORY ${GTEST_WORK_DIR}
)
