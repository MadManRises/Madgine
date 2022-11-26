include(ExternalProject)

set(shadergenSourceDir ${CMAKE_CURRENT_SOURCE_DIR}/shadergen)
set(shadergenBinaryDir ${CMAKE_CURRENT_BINARY_DIR}/shadergen)


ExternalProject_Add(ShaderGenExt
	PREFIX shadergen 
	BINARY_DIR shadergen/build
	SOURCE_DIR ${shadergenSourceDir}
	CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
	BUILD_BYPRODUCTS ${shadergenBinaryDir}/ShaderGen${HOST_EXECUTABLE_SUFFIX}
	BUILD_COMMAND ${CMAKE_COMMAND} --build . --target ShaderGen
	INSTALL_COMMAND "")

	
add_executable(ShaderGen IMPORTED GLOBAL)
add_dependencies(ShaderGen ShaderGenExt)
set_target_properties(ShaderGen 
	PROPERTIES ShaderGenTargets ""
	IMPORTED_LOCATION ${shadergenBinaryDir}/ShaderGen${HOST_EXECUTABLE_SUFFIX})

function(add_shadergen_target target flag)

	add_dependencies(${target} ShaderGen)

	get_target_property(targets ShaderGen ShaderGenTargets)
	set (targets ${targets} ${flag})
	set_target_properties(ShaderGen PROPERTIES ShaderGenTargets "${targets}")

endfunction(add_shadergen_target)