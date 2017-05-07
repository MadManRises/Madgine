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
	FolderSources(sources src App OgreApplication.cpp OgreApplication.h ogreappsettings.h configset.cpp configset.h)
	FolderSources(sources src Database messages.h translationkey.cpp translationkey.h translationunit.cpp translationunit.h)
	FolderSources(sources src GUI GUIEvents.h GUISystem.cpp GUISystem.h windownames.h)
		FolderSources(sources GUI MyGUI MyGUIBar.cpp MyGUIBar.h MyGUIButton.cpp MyGUIButton.h MyGUICheckbox.cpp MyGUICheckbox.h MyGUICombobox.cpp MyGUICombobox.h MyGUILabel.cpp MyGUILabel.h MyGUILauncher.cpp MyGUILauncher.h MyGUITextbox.cpp MyGUITextbox.h MyGUITextureDrawer.cpp MyGUITextureDrawer.h MyGUIWindow.cpp MyGUIWindow.h)
		FolderSources(sources GUI Windows Bar.h Button.h Checkbox.h Combobox.h Label.h TabWindow.h Textbox.h TextureDrawer.h Window.cpp Window.h WindowContainer.cpp WindowContainer.h )
	FolderSources(sources src Input InputHandler.h OISInputHandler.cpp OISInputHandler.h)
	FolderSources(sources src Math bounds.cpp bounds.h)
	FolderSources(sources src Scene ingameoverlay.cpp ingameoverlay.h ogrelight.cpp ogrelight.h ogrescenemanager.cpp ogrescenemanager.h)
	    FolderSources(sources Scene Entity ogreentity.cpp ogreentity.h)
	FolderSources(sources src Resources ResourceLoader.cpp ResourceLoader.h ogretextureptr.cpp ogretextureptr.h)
	    FolderSources(sources Resources Shading shadercollector.cpp shadercollector.h shaderimplementation.h)
		FolderSources(sources Resources ImageSets imagesetmanager.cpp imagesetmanager.h)
	FolderSources(sources src Scripting)
		FolderSources(sources Scripting Parsing ogrescriptparser.cpp ogrescriptparser.h)
	FolderSources(sources src Serialize ogreSerialize.cpp ogreSerialize.h OgreStreamReader.cpp OgreStreamReader.h)
	FolderSources(sources src UI gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h UIManager.cpp UIManager.h)
	FolderSources(sources src Util Profiler.cpp Profiler.h)

include_directories(${OGREMADGINE_SOURCE_DIR}/src)

add_library(OgreMadgine SHARED ${sources} madginelib.cpp)

add_precompiled_header(OgreMadgine ${CMAKE_CURRENT_SOURCE_DIR}/src/madginelib.h FORCEINCLUDE SOURCE_CXX madginelib.cpp)

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
