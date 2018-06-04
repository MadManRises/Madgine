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
	endif()
endif()