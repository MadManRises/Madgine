cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)


 
if(CMAKE_CXX_FLAGS_DEBUG MATCHES "/RTC1")
   string(REPLACE "/RTC1" " " CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()
 
if(CMAKE_CXX_FLAGS MATCHES "/EHsc")
   string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

Project(UNITYMADGINE)


set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
  "${UNITYMADGINE_SOURCE_DIR}/CMake"
)                                                           

set(sources "")
FolderSources(sources "" "")
FolderSources(sources "" src unityforward.h unitylib.h test.cpp test.h)
	#FolderSources(sources src App OgreApplication.cpp OgreApplication.h ogreappsettings.h configset.cpp configset.h)
	#FolderSources(sources src Database messages.h translationkey.cpp translationkey.h translationunit.cpp translationunit.h)
	#FolderSources(sources src GUI GUIEvents.h GUISystem.cpp GUISystem.h)
	#	FolderSources(sources GUI MyGUI MyGUIBar.cpp MyGUIBar.h MyGUIButton.cpp MyGUIButton.h MyGUICheckbox.cpp MyGUICheckbox.h MyGUICombobox.cpp MyGUICombobox.h MyGUILabel.cpp MyGUILabel.h MyGUILauncher.cpp MyGUILauncher.h MyGUITextbox.cpp MyGUITextbox.h MyGUITextureDrawer.cpp MyGUITextureDrawer.h MyGUIWindow.cpp MyGUIWindow.h)
	#	FolderSources(sources GUI Windows Bar.h Button.h Checkbox.h Combobox.h Label.h TabWindow.h Textbox.h TextureDrawer.h Window.cpp Window.h WindowContainer.cpp WindowContainer.h )
	#FolderSources(sources src Input InputHandler.h OISInputHandler.cpp OISInputHandler.h)
	#FolderSources(sources src Math bounds.cpp bounds.h)
	#FolderSources(sources src Scene ingameoverlay.cpp ingameoverlay.h ogrescenemanager.cpp ogrescenemanager.h scenetexturecomponent.cpp scenetexturecomponent.h)
	#    FolderSources(sources Scene Entity ogreentity.cpp ogreentity.h)
	#FolderSources(sources src Resources ResourceLoader.cpp ResourceLoader.h ogretextureptr.cpp ogretextureptr.h)
	#    FolderSources(sources Resources Shading shadercollector.cpp shadercollector.h shaderimplementation.h)
	#	FolderSources(sources Resources ImageSets imagesetmanager.cpp imagesetmanager.h)
	#FolderSources(sources src Scripting)
	#	FolderSources(sources Scripting Parsing ogrescriptparser.cpp ogrescriptparser.h)
	#FolderSources(sources src Serialize ogreSerialize.cpp ogreSerialize.h OgreStreamReader.cpp OgreStreamReader.h)
	#FolderSources(sources src UI gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h UIManager.cpp UIManager.h windownames.h)
	#FolderSources(sources src Util AppStats.cpp AppStats.h OgreUtilLog.cpp OgreUtilLog.h Profiler.cpp Profiler.h Util.cpp Util.h )

include_directories(${UNITYMADGINE_SOURCE_DIR}/src ${INTERFACES_INCLUDE_DIRS} ${BASE_SOURCE_DIR}/src)

add_library(UnityMadgine SHARED ${sources} unitylib.cpp)

add_precompiled_header(UnityMadgine src/unitylib.h FORCEINCLUDE SOURCE_CXX unitylib.cpp)

target_link_libraries (UnityMadgine Interfaces Base)

add_subdirectory(cSharp-Wrapper)

if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)