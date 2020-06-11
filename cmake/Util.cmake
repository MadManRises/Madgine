
if (NOT COMMAND include_guard)

	macro(include_guard)

		set(__filename "${CMAKE_CURRENT_LIST_FILE}")
		get_property(already_included GLOBAL PROPERTY "pr_${__filename}")
		if(already_included)
			return()
		endif()
		set_property(GLOBAL PROPERTY "pr_${__filename}" TRUE)

	endmacro(include_guard)

endif()

if (NOT COMMAND once)

	macro(once)
		include_guard(GLOBAL)
	endmacro(once)

endif()

once()

if (CMAKE_ANDROID_ARCH_ABI)
	set (ANDROID 1)
endif ()
  
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(GCC 1)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CLANG 1)
endif()

set (CMAKE_CXX_VISIBILITY_PRESET hidden)
set (CMAKE_C_VISIBILITY_PRESET hidden)

if (GCC OR CLANG)
	add_compile_options(-Wno-extra-qualification -Wno-instantiation-after-specialization -Wno-dll-attribute-on-redeclaration -Wno-pragma-pack -Wno-undefined-var-template)	
	if (NOT MSVC)
		add_compile_options(-Wall -fpermissive)
	else ()
		add_compile_options(-Wno-microsoft-cast)
	endif()
	if (EMSCRIPTEN) #TODO add more
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
	endif()
endif ()

macro(cmake_log)
	if (USE_CMAKE_LOG)
		MESSAGE(STATUS "cmake diagnostics: " ${ARGN})
	endif()
endmacro(cmake_log)