cmake_minimum_required (VERSION 2.6)


set(BUILD_OGRE FALSE CACHE BOOL "Build Ogre-Plugin")

if (BUILD_OGRE)

	cmake_policy(SET CMP0049 OLD)
	cmake_policy(SET CMP0048 NEW)
	cmake_policy(SET CMP0022 NEW)
	cmake_policy(SET CMP0057 NEW)


	Project(OgreMadgine)


	set(CMAKE_MODULE_PATH
		"${CMAKE_MODULE_PATH}"
		"${OGRE_HOME}/cmake"
		"${OgreMadgine_SOURCE_DIR}/cmake"
	)                         

	set(CMAKE_PREFIX_PATH
		"${CMAKE_PREFIX_PATH}"
	    "${OGRE_HOME}")

	load_workspace_package(assimp https://github.com/assimp/assimp.git GIT_TAG 80799bd)

	link_directories(${ASSIMP_LIBRARY_DIRS})

	set(sources "")
	FolderSources(sources "" src)
		FolderSources(sources src OgreMadgine)
			FolderSources(sources OgreMadgine input OIS.h OISConfig.h OISEffect.cpp OISEffect.h OISEvents.h OISException.cpp OISException.h OISFactoryCreator.h OISForceFeedback.cpp OISForceFeedback.h oisinputhandler.cpp oisinputhandler.h OISInputManager.cpp OISInputManager.h OISInterface.h OISJoyStick.cpp OISJoyStick.h OISKeyboard.cpp OISKeyboard.h OISMouse.h OISMultiTouch.h OISObject.cpp OISObject.h OISPrereqs.h)
			if (WIN32)
				FolderSources(sources input win32 Win32ForceFeedback.cpp Win32InputManager.cpp Win32JoyStick.cpp Win32KeyBoard.cpp Win32Mouse.cpp)
			endif()
			FolderSources(sources OgreMadgine mygui myguibar.cpp myguibar.h myguibutton.cpp myguibutton.h myguicheckbox.cpp myguicheckbox.h myguicombobox.cpp myguicombobox.h myguiemptywidget.cpp myguiemptywidget.h myguiimage.cpp myguiimage.h myguilabel.cpp myguilabel.h myguiscenewindow.cpp myguiscenewindow.h myguisystem.cpp myguisystem.h myguitabwidget.cpp myguitabwidget.h myguitextbox.cpp myguitextbox.h myguitoplevelwindow.cpp myguitoplevelwindow.h myguiwidget.cpp myguiwidget.h)
			FolderSources(sources OgreMadgine resources ogremeshloader.cpp ogremeshloader.h)
			FolderSources(sources OgreMadgine scene ogreanimationsystem.cpp ogreanimationsystem.h ogrecamera.cpp ogrecamera.h ogrecameramanager.cpp ogrecameramanager.h ogrelight.cpp ogrelight.h ogrelightmanager.cpp ogrelightmanager.h ogrescenerenderer.cpp ogrescenerenderer.h)
				FolderSources(sources scene entitycomponents ogreanimation.cpp ogreanimation.h ogremesh.cpp ogremesh.h ogreskeletonvisualizer.cpp ogreskeletonvisualizer.h ogretransform.cpp ogretransform.h)


	add_plugin(OgreMadgine Madgine Renderer ${sources})

	add_precompiled_header(OgreMadgine src/OgreMadgine/ogrelib.h FORCEINCLUDE)
	
	target_include_directories(OgreMadgine PRIVATE ${ASSIMP_INCLUDE_DIRS})	
				

	target_link_libraries(OgreMadgine PUBLIC Client PRIVATE ${ASSIMP_LIBRARIES})

	find_package(OGRE REQUIRED)
	if (OGRE_FOUND)
		foreach(dir ${OGRE_INCLUDE_DIRS})
		get_filename_component(dir_name ${dir} NAME)
		if ("${dir_name}" STREQUAL "OGRE")
			target_include_directories(OgreMadgine PUBLIC 
			$<BUILD_INTERFACE:${dir}>
			$<INSTALL_INTERFACE:OgreMadgine/include/OGRE> 
		    )
			install(DIRECTORY ${dir} DESTINATION OgreMadgine/include COMPONENT OgreMadgine)					
		endif()
		endforeach()
		  
  
		set(buffered_config_1 "")
		set(buffered_config_2 "")
		foreach(lib ${OGRE_LIBRARIES})		  
		if (lib STREQUAL "optimized")
			set(buffered_config_1 $<$<CONFIG:Release>:)
			set(buffered_config_2 >)
		elseif(lib STREQUAL "debug")
			set(buffered_config_1 $<$<CONFIG:Debug>:)
			set(buffered_config_2 >)
		elseif(lib STREQUAL "general")
			set(buffered_config_1 "")
			set(buffered_config_2 "")
		else()
		    target_link_libraries (OgreMadgine PUBLIC ${buffered_config_1}$<BUILD_INTERFACE:${lib}>$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/OgreMadgine/lib/${name}>${buffered_config_2})
			install(FILES ${buffered_config_1}${lib}${buffered_config_2} DESTINATION OgreMadgine/lib COMPONENT OgreMadgine)
			  
			set(buffered_config_1 "")
			set(buffered_config_2 "")
		endif()
		endforeach()
	endif (OGRE_FOUND)

	set (GuiSystems )

	find_package(MYGUI REQUIRED)
	if (MYGUI_FOUND)
		MESSAGE(STATUS ${MYGUI_INCLUDE_DIRS})
		foreach(dir ${MYGUI_INCLUDE_DIRS})
		get_filename_component(dir_name ${dir} NAME)
		target_include_directories(OgreMadgine PUBLIC 
    	    $<BUILD_INTERFACE:${dir}>
			$<INSTALL_INTERFACE:OgreMadgine/include> 
		)
		install(DIRECTORY ${dir}/MYGUI DESTINATION OgreMadgine/include COMPONENT OgreMadgine)					
		endforeach()

		foreach(lib ${MYGUI_LIBRARIES})		  
		if (lib STREQUAL "optimized")
			set(buffered ${lib})
			set(buffered_config_1 $<$<CONFIG:Release>:)
			set(buffered_config_2 >)
		elseif(lib STREQUAL "debug")
			set(buffered ${lib})
			set(buffered_config_1 $<$<CONFIG:Debug>:)
			set(buffered_config_2 >)
		elseif(lib STREQUAL "general")
			set(buffered ${lib})
			set(buffered_config_1 "")
			set(buffered_config_2 "")
		else()
			if (NOT IS_ABSOLUTE ${lib})
			find_library(lib ${lib} HINTS ${MYGUI_LIBRARY_DIRS})
			if (NOT ${lib}_path)
				MESSAGE(SEND_ERROR "Could not find ${lib}.")
			endif()
			endif()
			get_filename_component(name ${lib} NAME)
		    target_link_libraries (OgreMadgine PUBLIC ${buffered_config_1}$<BUILD_INTERFACE:${lib}>$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/OgreMadgine/lib/${name}>${buffered_config_2})
			install(FILES ${buffered_config_1}${lib}${buffered_config_2} DESTINATION OgreMadgine/lib COMPONENT OgreMadgine)
			  
			set(buffered_config_1 "")
			set(buffered_config_2 "")
			  
		endif()
		endforeach()

	endif (MYGUI_FOUND)

	if (WIN32)
		find_package(WindowsSDK)
	    if (CMAKE_CL_64)
		    set(DIRECTX_ARCH x64)
		else()
			set(DIRECTX_ARCH x86)
		endif()

		if (WINDOWSSDK_FOUND)
			target_link_libraries(OgreMadgine "dinput8.lib" "dxguid.lib")
	    else()
		    target_link_libraries(OgreMadgine "${DXSDK_DIR}/Lib/${DIRECTX_ARCH}/dinput8.lib" "${DXSDK_DIR}/Lib/${DIRECTX_ARCH}/dxguid.lib")
		endif()
	endif()

	install_to_workspace(OgreMadgine TARGETS OgreMadgine EXPORT_LIB)
	export_to_workspace(OgreMadgine)

	cpack_add_component(OgreMadgine DISPLAY_NAME Ogre GROUP Renderer)

		
	function(read_files var_name filename path)
			
		file(STRINGS ${filename} lines)

		set(plugins "")

		set(plugin_folder "")

		foreach(line ${lines})

			string(REPLACE " " "" line "${line}")
				
			string(FIND "${line}" "#" out)
			if(NOT "${out}" EQUAL -1)
				string(SUBSTRING "${line}" 0 ${out} line)
			endif()

			if (NOT "${line}" STREQUAL "")

				if ("${line}" MATCHES "^([0-9a-zA-Z._]*)=([0-9a-zA-Z._]*)$")
				
					if ("${CMAKE_MATCH_1}" STREQUAL "PluginFolder")
						set(plugin_folder "${CMAKE_MATCH_2}")
					elseif ("${CMAKE_MATCH_1}" STREQUAL "Plugin")
						list(APPEND plugins "${path}/${plugin_folder}/${CMAKE_MATCH_2}${CMAKE_SHARED_LIBRARY_SUFFIX}")
					else()
						MESSAGE(SEND_ERROR "Unknown key in config-file:" "${CMAKE_MATCH_1}")
					endif ()
				
				else()
					
					MESSAGE(SEND_ERROR "Invalid config-file syntax:" "${line}")

				endif()			

			endif()

		endforeach()

		set(${var_name} ${plugins} PARENT_SCOPE)

	endfunction(read_files)

	find_file(plugins_d plugins_d.cfg PATHS ${OGRE_BINARY_DBG} ${OGRE_CONFIG_DIR})
		
	if (plugins_d)
		read_files(debug_plugins ${plugins_d} ${OGRE_PLUGIN_DIR_DBG} ${OGRE_CONFIG_DIR})
	else()
		MESSAGE(WARNING "File plugins_d.cfg could not be found!")
	endif()

	find_file(plugins plugins.cfg PATHS ${OGRE_BINARY_DBG} ${OGRE_CONFIG_DIR})
		
	if (plugins)
		read_files(release_plugins ${plugins} ${OGRE_PLUGIN_DIR_REL} ${OGRE_CONFIG_DIR})
	else()
		MESSAGE(WARNING "File plugins.cfg could not be found!")
	endif()
		

	collect_target_dependencies(OgreMadgine)

	install(FILES $<$<CONFIG:Debug>:${plugins_d}> $<$<CONFIG:Release>:${plugins}> DESTINATION bin COMPONENT OgreMadgine)

	foreach(debug_file ${debug_plugins})

		install(FILES $<$<CONFIG:Debug>:${debug_file}> DESTINATION bin COMPONENT OgreMadgine)

	endforeach()

	foreach(release_file ${release_plugins})

		install(FILES $<$<CONFIG:Release>:${release_file}> DESTINATION bin COMPONENT OgreMadgine)

	endforeach()

endif()
