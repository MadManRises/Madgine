cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)

PROJECT(MADGINE)


set(MADGINE_MAJOR_VERSION 0)
set(MADGINE_MINOR_VERSION 1)
set(MADGINE_PATCH_VERSION 0)
set(MADGINE_VERSION
  ${MADGINE_MAJOR_VERSION}.${MADGINE_MINOR_VERSION}.${MADGINE_PATCH_VERSION})




set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
	"${MADGINE_SOURCE_DIR}/CMake"
)

FolderSources(sources "" "" forward.h libinclude.h libinclude.cpp Ogrememory.h singletonimpl.h uniquecomponent.h uniquecomponentcollector.h)
	FolderSources(sources "" App Application.cpp Application.h appsettings.h configset.cpp configset.h contextmasks.h)
	FolderSources(sources "" Database exceptionmessages.h message.h messages.h translationkey.cpp translationkey.h translationunit.cpp translationunit.h)
	FolderSources(sources "" GUI GUIEvents.cpp GUIEvents.h GUISystem.cpp GUISystem.h)
		FolderSources(sources GUI MyGUI MyGUIBar.cpp MyGUIBar.h MyGUIButton.cpp MyGUIButton.h MyGUICheckbox.cpp MyGUICheckbox.h MyGUICombobox.cpp MyGUICombobox.h MyGUILabel.cpp MyGUILabel.h MyGUILauncher.cpp MyGUILauncher.h MyGUILayout.cpp MyGUILayout.h MyGUITextbox.cpp MyGUITextbox.h MyGUITextureDrawer.cpp MyGUITextureDrawer.h MyGUIWindow.cpp MyGUIWindow.h)
		FolderSources(sources GUI Windows Bar.h Button.h Checkbox.h Combobox.h Label.h Layout.h TabWindow.h Textbox.h TextureDrawer.cpp TextureDrawer.h Window.cpp Window.h WindowContainer.cpp WindowContainer.h )
	FolderSources(sources "" Input InputHandler.h OISInputHandler.cpp OISInputHandler.h)
	FolderSources(sources "" Math bounds.cpp bounds.h)
	FolderSources(sources "" Ogre entityexception.h ingameoverlay.cpp ingameoverlay.h scenemanager.cpp scenemanager.h scenecomponent.cpp scenecomponent.h scenelistener.cpp scenelistener.h scenenames.h scenetexturecomponent.cpp scenetexturecomponent.h texturecomponent.cpp texturecomponent.h texturelistener.h)
	    FolderSources(sources Ogre Entity baseentitycomponent.cpp baseentitycomponent.h componentexception.h entity.cpp entity.h entitycomponent.h masks.h)
	FolderSources(sources "" Os os.h windows.cpp)
	FolderSources(sources "" Resources ResourceLoader.cpp ResourceLoader.h)
	    FolderSources(sources Resources Shading shadercollector.cpp shadercollector.h shaderimplementation.h)
		FolderSources(sources Resources ImageSets imagesetmanager.cpp imagesetmanager.h)
	FolderSources(sources "" Scripting scriptexception.h)
		FolderSources(sources Scripting Datatypes argumentlist.h fieldaccessor.cpp fieldaccessor.h valuetype.cpp valuetype.h varset.cpp varset.h)
			FolderSources(sources Datatypes Serialize filebuffer.cpp filebuffer.h fileexception.h levelserializer.cpp levelserializer.h OgreStreamReader.cpp OgreStreamReader.h serializable.cpp serializable.h serializeexception.h serializestream.cpp serializestream.h)
		FolderSources(sources Scripting Parsing entitynode.cpp entitynode.h entitynodeptr.cpp entitynodeptr.h methodnode.cpp methodnode.h methodnodeptr.cpp methodnodeptr.h parseexception.h scriptparser.cpp scriptparser.h textResource.cpp textResource.h)
		FolderSources(sources Scripting Statements accessor.cpp accessor.h ArithmeticalOperator.cpp ArithmeticalOperator.h assignment.cpp assignment.h comparator.cpp comparator.h constvalue.cpp constvalue.h for.cpp for.h if.cpp if.h logicaloperator.cpp logicaloperator.h methodcall.cpp methodcall.h negate.cpp negate.h return.cpp return.h scopesetter.cpp scopesetter.h statement.h while.cpp while.h)
		FolderSources(sources Scripting Types api.cpp api.h baseglobalapicomponent.cpp baseglobalapicomponent.h globalapi.h globalapicomponent.h globalscope.cpp globalscope.h level.cpp level.h list.cpp list.h refscope.cpp refscope.h scope.cpp scope.h scopefactory.cpp scopefactory.h scopefactoryimpl.h scopeimpl.h struct.cpp struct.h)
	FolderSources(sources "" UI gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h Process.cpp Process.h ProcessListener.h UIManager.cpp UIManager.h windownames.h)
	FolderSources(sources "" Util Profiler.cpp Profiler.h UtilMethods.cpp UtilMethods.h)

include_directories(${MADGINE_SOURCE_DIR})

add_library(Madgine SHARED ${sources})

add_precompiled_header(Madgine libinclude.h FORCEINCLUDE)


if (OGRE_FOUND)
  include_directories(${OGRE_INCLUDE_DIRS} ${OGRE_Terrain_INCLUDE_DIRS} ${OGRE_RTShaderSystem_INCLUDE_DIRS})
  target_link_libraries (Madgine ${OGRE_LIBRARIES} ${OGRE_Terrain_LIBRARIES} ${OGRE_RTShaderSystem_LIBRARIES})
endif (OGRE_FOUND)

find_package(OIS REQUIRED)
if (OIS_FOUND)
  include_directories(${OIS_INCLUDE_DIRS})
  target_link_libraries (Madgine ${OIS_LIBRARIES})
endif (OIS_FOUND)

find_package(MYGUI REQUIRED)
if (MYGUI_FOUND)
  include_directories(${MYGUI_INCLUDE_DIRS})
  MESSAGE(STATUS ${MYGUI_LIBRARIES})
  target_link_libraries (Madgine ${MYGUI_LIBRARIES})
endif (MYGUI_FOUND)


# Export Configuration
# ===============================
 
# Add all targets to the build-tree export set
export(TARGETS Madgine
  FILE "${PROJECT_BINARY_DIR}/MadgineTargets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE Madgine)
 
# Create the MadgineConfig.cmake and MadgineConfigVersion files
#file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
#   "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
set(CONF_INCLUDE_DIRS "${dirs}")
configure_file(CMake/MadgineConfig.cmake.in
  "${PROJECT_BINARY_DIR}/MadgineConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${MADGINE_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(CMake/MadgineConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MadgineConfig.cmake" @ONLY)
# ... for both
configure_file(CMake/MadgineConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/MadgineConfigVersion.cmake" @ONLY)
 
