include(ExternalProject)

set(dxCompilerSourceDir ${CMAKE_CURRENT_SOURCE_DIR}/ext/DirectXShaderCompiler)
set(dxCompilerBinaryDir ${CMAKE_CURRENT_BINARY_DIR}/DirectXShaderCompiler/build)

ExternalProject_Add(DirectXShaderCompiler
	PREFIX DirectXShaderCompiler 
	BINARY_DIR DirectXShaderCompiler/build
	SOURCE_DIR ${dxCompilerSourceDir}
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DHLSL_BUILD_DXILCONV=OFF -DHLSL_COPY_GENERATED_SOURCES=ON -DCMAKE_VERBOSE_MAKEFILE=ON -C ${dxCompilerSourceDir}/cmake/caches/PredefinedParams.cmake 
	BUILD_BYPRODUCTS
		${dxCompilerBinaryDir}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}dxcompiler${CMAKE_IMPORT_LIBRARY_SUFFIX}
		${dxCompilerBinaryDir}/bin/dxc${CMAKE_EXECUTABLE_SUFFIX}
	INSTALL_COMMAND "")

add_executable(dxc IMPORTED GLOBAL)
set_target_properties(dxc PROPERTIES 
	IMPORTED_LOCATION ${dxCompilerBinaryDir}/bin/dxc${CMAKE_EXECUTABLE_SUFFIX})
add_dependencies(dxc DirectXShaderCompiler)

add_library(dxcompiler SHARED IMPORTED GLOBAL)
set_target_properties(dxcompiler PROPERTIES 
	IMPORTED_LOCATION ${dxCompilerBinaryDir}/bin/dxcompiler${CMAKE_SHARED_LIBRARY_SUFFIX}
	IMPORTED_IMPLIB ${dxCompilerBinaryDir}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}dxcompiler${CMAKE_IMPORT_LIBRARY_SUFFIX}
	INTERFACE_INCLUDE_DIRECTORIES ${dxCompilerSourceDir}/include/dxc)
add_dependencies(dxcompiler DirectXShaderCompiler)



set(spirVCrossSourceDir ${CMAKE_CURRENT_SOURCE_DIR}/ext/SPIRV-Cross)
set(spirVCrossBinaryDir ${CMAKE_CURRENT_BINARY_DIR}/SPIRV-Cross/build)

if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(POSTFIX "d")
endif()

set(SPIRV_CROSS_CORE_NAME ${spirVCrossBinaryDir}/${CMAKE_SHARED_LIBRARY_PREFIX}spirv-cross-core${POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SPIRV_CROSS_HLSL_NAME ${spirVCrossBinaryDir}/${CMAKE_SHARED_LIBRARY_PREFIX}spirv-cross-hlsl${POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX})

set(SPIRV_CROSS_GLSL_NAME ${spirVCrossBinaryDir}/${CMAKE_SHARED_LIBRARY_PREFIX}spirv-cross-glsl${POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX})


ExternalProject_Add(SPIRV-Cross
	PREFIX SPIRV-Cross 
	BINARY_DIR SPIRV-Cross/build
	SOURCE_DIR ${spirVCrossSourceDir}
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DSPIRV_CROSS_STATIC=ON -DSPIRV_CROSS_SHARED=OFF -DSPIRV_CROSS_CLI=OFF -DCMAKE_VERBOSE_MAKEFILE=ON
	BUILD_BYPRODUCTS
		${SPIRV_CROSS_CORE_NAME}
		${SPIRV_CROSS_HLSL_NAME}
		${SPIRV_CROSS_GLSL_NAME}
	INSTALL_COMMAND "")

add_library(spirv-cross-core STATIC IMPORTED GLOBAL)
set_target_properties(spirv-cross-core PROPERTIES 
	IMPORTED_LOCATION ${SPIRV_CROSS_CORE_NAME}
	INTERFACE_INCLUDE_DIRECTORIES ${spirVCrossSourceDir})
add_dependencies(spirv-cross-core SPIRV-Cross)

add_library(spirv-cross-hlsl STATIC IMPORTED GLOBAL)
set_target_properties(spirv-cross-hlsl PROPERTIES 
	IMPORTED_LOCATION ${SPIRV_CROSS_HLSL_NAME}
	INTERFACE_INCLUDE_DIRECTORIES ${spirVCrossSourceDir})
add_dependencies(spirv-cross-hlsl SPIRV-Cross)

add_library(spirv-cross-glsl STATIC IMPORTED GLOBAL)
set_target_properties(spirv-cross-glsl PROPERTIES 
	IMPORTED_LOCATION ${SPIRV_CROSS_GLSL_NAME}
	INTERFACE_INCLUDE_DIRECTORIES ${spirVCrossSourceDir})
add_dependencies(spirv-cross-glsl SPIRV-Cross)


add_custom_target(ShaderGen)
set_target_properties(ShaderGen PROPERTIES ShaderGenTools "")

function(add_shadergen_target target)

	set_target_properties(${target} PROPERTIES 
		FOLDER "Util"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/util
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/util) 

	add_dependencies(ShaderGen ${target})

	get_target_property(tools ShaderGen ShaderGenTools)
	set (tools ${tools} $<TARGET_FILE:${target}>)
	set_target_properties(ShaderGen PROPERTIES ShaderGenTools "${tools}")

endfunction(add_shadergen_target)