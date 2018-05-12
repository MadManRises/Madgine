cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)
cmake_policy(SET CMP0057 NEW)


Project(Client)      

set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
  "${Client_SOURCE_DIR}/../ogre/cmake"
)       

set(sources "")
FolderSources(sources "" "..")
FolderSources(sources ".." src clientforward.h clientlib.h)
	FolderSources(sources src app ogreapplication.cpp ogreapplication.h ogreappsettings.h configset.cpp configset.h)
	FolderSources(sources src database messages.h translationkey.cpp translationkey.h translationunit.cpp translationunit.h)
	FolderSources(sources src gui guievents.h guisystem.cpp guisystem.h windownames.h)
		FolderSources(sources gui mygui myguibar.cpp myguibar.h myguibutton.cpp myguibutton.h myguicheckbox.cpp myguicheckbox.h myguicombobox.cpp myguicombobox.h myguilabel.cpp myguilabel.h myguilauncher.cpp myguilauncher.h myguitextbox.cpp myguitextbox.h myguitextureDrawer.cpp myguitextureDrawer.h myguiwindow.cpp myguiwindow.h)
		FolderSources(sources gui windows bar.h button.h checkbox.h combobox.h label.h tabwindow.h textbox.h texturedrawer.h window.cpp window.h windowcontainer.cpp windowcontainer.h )
	FolderSources(sources src input inputhandler.cpp inputHandler.h oisinputhandler.cpp oisinputhandler.h)
	FolderSources(sources src math bounds.cpp bounds.h)
	FolderSources(sources src scene ingameoverlay.cpp ingameoverlay.h ogrelight.cpp ogrelight.h ogrescenemanager.cpp ogrescenemanager.h)
	    FolderSources(sources scene entity)
			FolderSources(sources entity components ogreanimation.cpp ogreanimation.h ogremesh.cpp ogremesh.h ogreskeletonvisualizer.cpp ogreskeletonvisualizer.h ogretransform.cpp ogretransform.h)
			FolderSources(sources entity systems ogreanimationsystem.cpp ogreanimationsystem.h)
	FolderSources(sources src resources resourceLoader.cpp ResourceLoader.h ogretextureptr.cpp ogretextureptr.h)
	    FolderSources(sources resources shading shadercollector.cpp shadercollector.h shaderimplementation.h)
		FolderSources(sources resources imagesets imagesetmanager.cpp imagesetmanager.h)
	FolderSources(sources src scripting)
		FolderSources(sources scripting parsing ogrescriptparser.cpp ogrescriptparser.h)
	FolderSources(sources src serialize ogrestreamreader.cpp ogrestreamreader.h)
	FolderSources(sources src ui gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h uimanager.cpp uimanager.h)



add_library(Client SHARED ${sources} clientlib.cpp)

add_precompiled_header(Client ../src/clientlib.h FORCEINCLUDE SOURCE_CXX clientlib.cpp)

target_link_libraries(Client PUBLIC Base)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()



find_package(OGRE REQUIRED)
if (OGRE_FOUND)
  target_include_directories(Client PUBLIC ${OGRE_INCLUDE_DIRS} ${OGRE_Terrain_INCLUDE_DIRS} ${OGRE_RTShaderSystem_INCLUDE_DIRS})
  
  set(buffered "")
  foreach(lib ${OGRE_LIBRARIES} ${OGRE_Terrain_LIBRARIES} ${OGRE_Overlay_LIBRARIES})
	if (lib STREQUAL "optimized" OR lib STREQUAL "debug" OR lib STREQUAL "general")
		set(buffered ${lib})
    else()
	  if (IS_ABSOLUTE ${lib})
	    target_link_libraries (Client PUBLIC ${buffered} ${lib})
	  else ()
        find_library(${lib}_path ${lib} HINTS ${OGRE_LIBRARY_DIRS})
	    if (NOT ${lib}_path)
	      target_link_libraries(Client PUBLIC ${buffered} ${lib})
	    else ()
	      target_link_libraries (Client PUBLIC ${buffered} ${${lib}_path})
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
    target_include_directories(Client PUBLIC ${${GuiSystem}_INCLUDE_DIRS})
    #foreach(lib ${${GuiSystem}_LIBRARIES})
	#  target_link_libraries (OgreMadgine PRIVATE ${lib})
    #endforeach()
	target_link_libraries(Client PUBLIC ${${GuiSystem}_LIBRARIES})
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
    target_include_directories(Client PUBLIC ${${InputSystem}_INCLUDE_DIRS})
	message(STATUS ${${InputSystem}_LIBRARIES})
	set(buffered "")
    foreach(lib ${${InputSystem}_LIBRARIES})
	  if (lib STREQUAL "optimized" OR lib STREQUAL "debug" OR lib STREQUAL "general")
		set(buffered ${lib})
	  else()
		target_link_libraries (Client PRIVATE ${buffered} ${lib})
		set(buffered "")
	  endif()
    endforeach()
  endif()
endif ()


install_to_workspace(Madgine Client)
