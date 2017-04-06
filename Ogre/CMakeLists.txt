cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)



Project(OGREMADGINE)

set(OGRE_SDK "" CACHE PATH "Path to Ogre install directory")

set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
  "${OGREMADGINE_SOURCE_DIR}/CMake"
  "${OGRE_SDK}/CMake"
)                                                           

set(sources "")
FolderSources(sources "" "")
FolderSources(sources "" src forward.h madginelib.h ogreuniquecomponent.h ogreuniquecomponentcollector.h)
	FolderSources(sources src App OgreApplication.cpp OgreApplication.h ogreappsettings.h configset.cpp configset.h)
	FolderSources(sources src Database messages.h translationkey.cpp translationkey.h translationunit.cpp translationunit.h)
	FolderSources(sources src GUI GUIEvents.h GUISystem.cpp GUISystem.h)
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
	FolderSources(sources src UI gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h UIManager.cpp UIManager.h windownames.h)
	FolderSources(sources src Util Profiler.cpp Profiler.h)

include_directories(${OGREMADGINE_SOURCE_DIR}/src ${INTERFACES_INCLUDE_DIRS} ${BASE_SOURCE_DIR}/src)

add_library(OgreMadgine SHARED ${sources} madginelib.cpp)

add_precompiled_header(OgreMadgine src/madginelib.h FORCEINCLUDE SOURCE_CXX madginelib.cpp)

target_link_libraries (OgreMadgine Interfaces Base)


find_package(MYGUI REQUIRED)
if (MYGUI_FOUND)
  include_directories(${MYGUI_INCLUDE_DIRS})
  MESSAGE(STATUS ${MYGUI_LIBRARIES})
  target_link_libraries (OgreMadgine ${MYGUI_LIBRARIES})
endif (MYGUI_FOUND)

find_package(OGRE REQUIRED)
if (OGRE_FOUND)
  include_directories(${OGRE_INCLUDE_DIRS} ${OGRE_Terrain_INCLUDE_DIRS} ${OGRE_RTShaderSystem_INCLUDE_DIRS})
  target_link_libraries (OgreMadgine ${OGRE_LIBRARIES} ${OGRE_Terrain_LIBRARIES} ${OGRE_RTShaderSystem_LIBRARIES} ${OGRE_Overlay_LIBRARIES})
endif (OGRE_FOUND)

find_package(OIS REQUIRED)
if (OIS_FOUND)
  include_directories(${OIS_INCLUDE_DIRS})
  target_link_libraries (OgreMadgine ${OIS_LIBRARIES})
endif (OIS_FOUND)


if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)