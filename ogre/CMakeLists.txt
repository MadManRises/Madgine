cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)


Project(OGREMADGINE)


set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
  "${OGREMADGINE_SOURCE_DIR}/CMake"
)                                                           

set(sources "")
FolderSources(sources "" "")
FolderSources(sources "" src forward.h madginelib.h ogreuniquecomponent.h ogreuniquecomponentcollector.h)
	FolderSources(sources src app ogreapplication.cpp ogreapplication.h ogreappsettings.h configset.cpp configset.h)
	FolderSources(sources src database messages.h translationkey.cpp translationkey.h translationunit.cpp translationunit.h)
	FolderSources(sources src gui guievents.h guisystem.cpp guisystem.h windownames.h)
		FolderSources(sources gui mygui myguibar.cpp myguibar.h myguibutton.cpp myguibutton.h myguicheckbox.cpp myguicheckbox.h myguicombobox.cpp myguicombobox.h myguilabel.cpp myguilabel.h myguilauncher.cpp myguilauncher.h myguitextbox.cpp myguitextbox.h myguitextureDrawer.cpp myguitextureDrawer.h myguiwindow.cpp myguiwindow.h)
		FolderSources(sources gui windows bar.h button.h checkbox.h combobox.h label.h tabwindow.h textbox.h texturedrawer.h window.cpp window.h windowcontainer.cpp windowcontainer.h )
	FolderSources(sources src input inputHandler.h oisinputhandler.cpp oisinputhandler.h)
	FolderSources(sources src math bounds.cpp bounds.h)
	FolderSources(sources src scene ingameoverlay.cpp ingameoverlay.h ogrelight.cpp ogrelight.h ogrescenemanager.cpp ogrescenemanager.h)
	    FolderSources(sources scene entity ogreentity.cpp ogreentity.h)
	FolderSources(sources src resources resourceLoader.cpp ResourceLoader.h ogretextureptr.cpp ogretextureptr.h)
	    FolderSources(sources resources shading shadercollector.cpp shadercollector.h shaderimplementation.h)
		FolderSources(sources resources imagesets imagesetmanager.cpp imagesetmanager.h)
	FolderSources(sources src scripting)
		FolderSources(sources scripting parsing ogrescriptparser.cpp ogrescriptparser.h)
	FolderSources(sources src serialize ogreserialize.cpp ogreserialize.h ogrestreamreader.cpp ogrestreamreader.h)
	FolderSources(sources src ui gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h uimanager.cpp uimanager.h)
	FolderSources(sources src util profiler.cpp profiler.h)

include_directories(${OGREMADGINE_SOURCE_DIR}/src)

add_library(OgreMadgine SHARED ${sources} madginelib.cpp)

add_precompiled_header(OgreMadgine src/madginelib.h FORCEINCLUDE SOURCE_CXX madginelib.cpp)

include_targets(OgreMadgine Base)
include_library(OgreMadgine Interfaces Interfaces)

find_package(MYGUI REQUIRED)
if (MYGUI_FOUND)
  target_include_directories(OgreMadgine PUBLIC ${MYGUI_INCLUDE_DIRS})
  MESSAGE(STATUS ${MYGUI_LIBRARIES})
  foreach(lib ${MYGUI_LIBRARIES})
	  target_link_libraries (OgreMadgine PRIVATE ${lib})
  endforeach()
endif (MYGUI_FOUND)

set(OGRE_HOME ${Workspace})
find_package(OGRE REQUIRED)
if (OGRE_FOUND)
  target_include_directories(OgreMadgine PUBLIC ${OGRE_INCLUDE_DIRS} ${OGRE_Terrain_INCLUDE_DIRS} ${OGRE_RTShaderSystem_INCLUDE_DIRS})
  MESSAGE(STATUS ${OGRE_LIBRARIES})
  set(buffered "")
  foreach(lib ${OGRE_LIBRARIES} ${OGRE_Terrain_LIBRARIES} ${OGRE_RTShaderSystem_LIBRARIES} ${OGRE_Overlay_LIBRARIES})
	  if (lib STREQUAL "optimized" OR lib STREQUAL "debug" OR lib STREQUAL "general")
		set(buffered ${lib})
	  else()
		target_link_libraries (OgreMadgine PUBLIC ${buffered} ${lib})
		set(buffered "")
	  endif()
  endforeach()
endif (OGRE_FOUND)

find_package(OIS REQUIRED)
if (OIS_FOUND)
  target_include_directories(OgreMadgine PUBLIC ${OIS_INCLUDE_DIRS})
  foreach(lib ${OIS_LIBRARIES})
	  if (lib STREQUAL "optimized" OR lib STREQUAL "debug" OR lib STREQUAL "general")
		set(buffered ${lib})
	  else()
		target_link_libraries (OgreMadgine PRIVATE ${buffered} ${lib})
		set(buffered "")
	  endif()
  endforeach()
endif (OIS_FOUND)


if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)


installToWorkspace(OgreMadgine Madgine)
