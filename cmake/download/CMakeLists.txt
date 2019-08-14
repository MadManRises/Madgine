cmake_minimum_required(VERSION 3.8)

Project(${lib}_download)


include(ExternalProject)

if (THIRD_PARTY)

	set (additional_args SOURCE_SUBDIR ..)

endif()

message(STATUS "Configuring download: ${lib}")


ExternalProject_Add(Update_${lib}
	PREFIX ${Workspace}/${lib}/download
	CMAKE_GENERATOR ${CMAKE_GENERATOR}
	CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
	GIT_REPOSITORY    ${url}
	GIT_TAG           ${GIT_TAG}
	INSTALL_DIR       ${Workspace}
	${additional_args}
	CMAKE_CACHE_ARGS
		-DCMAKE_INSTALL_PREFIX:PATH=${Workspace}
		-DWorkspace:PATH=${Workspace}
		-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}
		${DEFINITIONS}
)
