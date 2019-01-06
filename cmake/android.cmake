include(Util)

include_guard()

include (Workspace)

if (ANDROID)

	set (Android_List_dir ${CMAKE_CURRENT_LIST_DIR})

	if (NOT ANDROID_SDK)
		MESSAGE(SEND_ERROR "No ANDROID_SDK location provided!")
	endif()


	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gradle)

	add_custom_command(OUTPUT gradle/gradlew
			COMMAND gradle wrapper --gradle-version=4.10.2 --distribution-type=all
			WORKING_DIRECTORY gradle)

	function(add_workspace_executable target)

		string(REGEX REPLACE "\\\\" "\\\\\\\\" ANDROID_SDK_ESCAPED "${ANDROID_SDK}")

		configure_file(${Android_List_dir}/android/build.gradle.in build.gradle @ONLY)
		configure_file(${Android_List_dir}/android/local.properties.in local.properties @ONLY)
		configure_file(${Android_List_dir}/android/AndroidManifest.xml.in AndroidManifest.xml @ONLY)
		configure_file(${Android_List_dir}/android/launch.vs.json.in ${CMAKE_SOURCE_DIR}/.vs/launch.vs.json @ONLY)
		configure_file(${Android_List_dir}/android/debug_apk.bat.in debug_${target}_apk.bat @ONLY)
		configure_file(${Android_List_dir}/android/launch_apk.sh.in launch_${target}_apk.sh @ONLY)

		add_library(${target} SHARED gradle/gradlew ${ARGN})

		_add_workspace_executable(${target}_apk android/dummy.cpp)
		target_link_libraries(${target}_apk PRIVATE ${target})

		add_custom_command(
			TARGET ${target}
			POST_BUILD				
			COMMAND gradle/gradlew assembleDebug
			COMMAND gradle/gradlew --stop
			COMMENT "Build APK - ${target}"			
			BYPRODUCTS apk/${target}-debug.apk
		)
		
		target_link_libraries(${target} PUBLIC android log)				

	endfunction(add_workspace_executable)

endif()