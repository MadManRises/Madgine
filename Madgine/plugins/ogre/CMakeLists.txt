cmake_minimum_required (VERSION 2.6)


if (BUILD_CLIENT AND NOT CONFIG_BUILD)

	set(BUILD_OGRE FALSE CACHE BOOL "Build Ogre-Plugin")

	if (BUILD_OGRE)

		cmake_policy(SET CMP0049 OLD)
		cmake_policy(SET CMP0048 NEW)
		cmake_policy(SET CMP0022 NEW)
		cmake_policy(SET CMP0057 NEW)


		Project(OgreMadgine)


		set(CMAKE_MODULE_PATH
		  "${CMAKE_MODULE_PATH}"
		  "${OgreMadgine_SOURCE_DIR}/cmake"
		)                         



		set(sources "")
		FolderSources(sources "" src)
			FolderSources(sources src OgreMadgine ogrescenerenderer.cpp ogrescenerenderer.h ogreanimation.cpp ogreanimation.h ogremesh.cpp ogremesh.h ogreskeletonvisualizer.cpp ogreskeletonvisualizer.h ogretransform.cpp ogretransform.h ogreanimationsystem.cpp ogreanimationsystem.h)


		add_plugin(OgreMadgine Madgine Renderer ${sources})

		#add_precompiled_header(OgreMadgine ../src/ogrelib.h FORCEINCLUDE SOURCE_CXX madginelib.cpp)

		target_link_libraries(OgreMadgine PUBLIC Client)

		find_package(OGRE REQUIRED)
		if (OGRE_FOUND)
		  target_include_directories(OgreMadgine PUBLIC ${OGRE_INCLUDE_DIRS} ${OGRE_Terrain_INCLUDE_DIRS} ${OGRE_RTShaderSystem_INCLUDE_DIRS})
  
		  set(buffered "")
		  foreach(lib ${OGRE_LIBRARIES} ${OGRE_Terrain_LIBRARIES} ${OGRE_Overlay_LIBRARIES})
		  MESSAGE(STATUS ${lib})
			if (lib STREQUAL "optimized" OR lib STREQUAL "debug" OR lib STREQUAL "general")
				set(buffered ${lib})
			else()
			  if (IS_ABSOLUTE ${lib})
				target_link_libraries (OgreMadgine PUBLIC ${buffered} ${lib})
			  else ()
				find_library(${lib}_path ${lib} HINTS ${OGRE_LIBRARY_DIRS})
				if (NOT ${lib}_path)
				  target_link_libraries(OgreMadgine PUBLIC ${buffered} ${lib})
				else ()
				  target_link_libraries (OgreMadgine PUBLIC ${buffered} ${${lib}_path})
				endif()
			  endif()
			  set(buffered "")
			endif()
		  endforeach()
		endif (OGRE_FOUND)

		set (GuiSystems )

		find_package(MYGUI)
		if (MYGUI_FOUND)
		  set (GuiSystems ${GuiSystems} MYGUI)
		endif (MYGUI_FOUND)

		if ("${GuiSystems}" STREQUAL "")
		  MESSAGE (SEND_ERROR "No Gui-System found!")
		else()
		  list(GET GuiSystems 0
			 DefaultGuiSystem)
		  set (GuiSystem "${DefaultGuiSystem}" CACHE STRING "Used Gui-System")
		  set_property(CACHE GuiSystem PROPERTY STRINGS ${GuiSystems})

		  if (NOT "${GuiSystem}" IN_LIST GuiSystems)
			  MESSAGE(SEND_ERROR "No valid Gui-System selected!")
		  else()
			target_include_directories(OgreMadgine PUBLIC ${${GuiSystem}_INCLUDE_DIRS})
			#foreach(lib ${${GuiSystem}_LIBRARIES})
			#  target_link_libraries (OgreMadgine PRIVATE ${lib})
			#endforeach()
			target_link_libraries(OgreMadgine PUBLIC ${${GuiSystem}_LIBRARIES})
		  endif()

		endif()


		set(InputSystems )

		find_package(OIS)
		if (OIS_FOUND)
		  set (InputSystems ${InputSystems} OIS)
		endif (OIS_FOUND)

		if ("${InputSystems}" STREQUAL "")
		  MESSAGE (SEND_ERROR "No Input-System found!")
		else()
		  list (GET InputSystems 0
			  DefaultInputSystem)
		  set (InputSystem "${DefaultInputSystem}" CACHE STRING "Used Input-System")
		  set_property(CACHE InputSystem PROPERTY STRINGS ${InputSystems})

		  if (NOT "${InputSystem}" IN_LIST InputSystems)
			MESSAGE(SEND_ERROR "No valid Input-System selected!")
		  else()
			target_include_directories(OgreMadgine PUBLIC ${${InputSystem}_INCLUDE_DIRS})
			message(STATUS ${${InputSystem}_LIBRARIES})
			set(buffered "")
			foreach(lib ${${InputSystem}_LIBRARIES})
			  if (lib STREQUAL "optimized" OR lib STREQUAL "debug" OR lib STREQUAL "general")
				set(buffered ${lib})
			  else()
				target_link_libraries (OgreMadgine PRIVATE ${buffered} ${lib})
				set(buffered "")
			  endif()
			endforeach()
		  endif()
		endif ()


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

		
		find_file(plugins_d plugins_d.cfg PATHS ${OGRE_BINARY_DBG})
		
		if (plugins_d)
			read_files(debug_plugins ${plugins_d} ${OGRE_PLUGIN_DIR_DBG})
		else()
			MESSAGE(SEND_ERROR "File plugins_d.cfg could not be found!")
		endif()

		find_file(plugins plugins.cfg PATHS ${OGRE_BINARY_DBG})
		
		if (plugins)
			read_files(release_plugins ${plugins} ${OGRE_PLUGIN_DIR_REL})
		else()
			MESSAGE(SEND_ERROR "File plugins.cfg could not be found!")
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
endif()