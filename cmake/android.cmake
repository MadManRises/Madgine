
if (CMAKE_ANDROID_ARCH_ABI)

	configure_file(${CMAKE_CURRENT_LIST_DIR}/android/build.gradle.in build.gradle @ONLY)
	configure_file(${CMAKE_CURRENT_LIST_DIR}/android/AndroidManifest.xml.in AndroidManifest.xml @ONLY)

	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gradle)

	add_custom_command(OUTPUT gradlew
			COMMAND gradle wrapper --gradle-version=4.10.2 --distribution-type=all
			WORKING_DIRECTORY gradle)

	add_library(native_app_glue STATIC ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

	target_include_directories(native_app_glue PUBLIC ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue)

	target_link_libraries(native_app_glue PUBLIC android log)



	function(build_apk target)

		add_custom_target(
				build_apk ALL
				COMMAND gradle/gradlew assembleDebug
				COMMENT "Build APK - ${target}"
				DEPENDS gradlew
		)

		target_link_libraries(${target} PRIVATE native_app_glue)

		add_dependencies(build_apk ${target})

	endfunction(build_apk)

endif()