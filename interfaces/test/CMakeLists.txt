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

target_link_libraries(Interfaces_Test Interfaces ${GTEST_LIBS})

add_test(
	NAME Interfaces_Test
	COMMAND Interfaces_Test --gtest_output=xml:Interfaces.xml
	WORKING_DIRECTORY ${GTEST_WORK_DIR})
