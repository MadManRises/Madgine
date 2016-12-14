cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)




Project(MADGINE)

set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
  "${MADGINE_SOURCE_DIR}/CMake"
)


set(MADGINE_MAJOR_VERSION 0)
set(MADGINE_MINOR_VERSION 1)
set(MADGINE_PATCH_VERSION 0)
set(MADGINE_VERSION
  ${MADGINE_MAJOR_VERSION}.${MADGINE_MINOR_VERSION}.${MADGINE_PATCH_VERSION})


set(sources "")
FolderSources(sources "" "")
FolderSources(sources "" src forward.h madginelib.h Ogrememory.h singletonimpl.h templates.cpp templates.h uniquecomponent.h uniquecomponentcollector.h valuetype.cpp valuetype.h)
	FolderSources(sources src App Application.cpp Application.h appsettings.h configset.cpp configset.h contextmasks.h)
	FolderSources(sources src Database exceptionmessages.h message.h messages.h translationkey.cpp translationkey.h translationunit.cpp translationunit.h)
	FolderSources(sources src GUI GUIEvents.cpp GUIEvents.h GUISystem.cpp GUISystem.h)
		FolderSources(sources GUI MyGUI MyGUIBar.cpp MyGUIBar.h MyGUIButton.cpp MyGUIButton.h MyGUICheckbox.cpp MyGUICheckbox.h MyGUICombobox.cpp MyGUICombobox.h MyGUILabel.cpp MyGUILabel.h MyGUILauncher.cpp MyGUILauncher.h MyGUITextbox.cpp MyGUITextbox.h MyGUITextureDrawer.cpp MyGUITextureDrawer.h MyGUIWindow.cpp MyGUIWindow.h)
		FolderSources(sources GUI Windows Bar.h Button.h Checkbox.h Combobox.h Label.h TabWindow.h Textbox.h TextureDrawer.h Window.cpp Window.h WindowContainer.cpp WindowContainer.h )
	FolderSources(sources src Input InputHandler.h OISInputHandler.cpp OISInputHandler.h)
	FolderSources(sources src Math bounds.cpp bounds.h)
	FolderSources(sources src Network networkbuffer.cpp networkbuffer.h networkexception.cpp networkexception.h networkmanager.cpp networkmanager.h networkstream.cpp networkstream.h)
	FolderSources(sources src Scene ingameoverlay.cpp ingameoverlay.h scenemanager.cpp scenemanager.h scenecomponent.cpp scenecomponent.h scenelistener.cpp scenelistener.h scenetexturecomponent.cpp scenetexturecomponent.h)
	    FolderSources(sources Scene Entity baseentitycomponent.cpp baseentitycomponent.h componentexception.h entity.cpp entity.h entitycomponent.h masks.h)
	FolderSources(sources src Os os.h windows.cpp)
	FolderSources(sources src Resources ResourceLoader.cpp ResourceLoader.h)
	    FolderSources(sources Resources Shading shadercollector.cpp shadercollector.h shaderimplementation.h)
		FolderSources(sources Resources ImageSets imagesetmanager.cpp imagesetmanager.h)
	FolderSources(sources src Scripting scriptingexception.h)
		FolderSources(sources Scripting Datatypes fieldaccessor.cpp fieldaccessor.h)
		FolderSources(sources Scripting Parsing entitynode.cpp entitynode.h entitynodeptr.cpp entitynodeptr.h methodnode.cpp methodnode.h methodnodeptr.cpp methodnodeptr.h parseexception.h prototype.cpp prototype.h scriptparser.cpp scriptparser.h textResource.cpp textResource.h textresourceptr.cpp textresourceptr.h)
		FolderSources(sources Scripting Statements accessor.cpp accessor.h ArithmeticalOperator.cpp ArithmeticalOperator.h assignment.cpp assignment.h comparator.cpp comparator.h constvalue.cpp constvalue.h for.cpp for.h if.cpp if.h logicaloperator.cpp logicaloperator.h methodcall.cpp methodcall.h negate.cpp negate.h return.cpp return.h scopesetter.cpp scopesetter.h statement.h while.cpp while.h)
		FolderSources(sources Scripting Types api.cpp api.h array.cpp array.h baseglobalapicomponent.cpp baseglobalapicomponent.h globalapi.h globalapicomponent.h globalscope.cpp globalscope.h list.cpp list.h refscope.cpp refscope.h refscopetoplevelserializableunit.cpp refscopetoplevelserializableunit.h scene.cpp scene.h scope.cpp scope.h scope_cast.h scopeimpl.h scriptingmanager.cpp scriptingmanager.h struct.cpp struct.h)
	FolderSources(sources src Serialize fileexception.h observable.cpp observable.h serializable.cpp serializable.h serializablebase.h serializableunit.cpp serializableunit.h serializeexception.h serializemanager.cpp serializemanager.h toplevelserializableunit.cpp toplevelserializableunit.h)
		FolderSources(sources Serialize Container action.h container.h list.h map.h observed.h serialized.h set.h)
		FolderSources(sources Serialize Streams filebuffer.cpp filebuffer.h OgreStreamReader.cpp OgreStreamReader.h serializestream.cpp serializestream.h)
	FolderSources(sources src UI gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h Process.cpp Process.h ProcessListener.h UIManager.cpp UIManager.h windownames.h)
	FolderSources(sources src Util MadgineObject.cpp MadgineObject.h MadgineObjectCollector.cpp MadgineObjectCollector.h ObjectState.h Profiler.cpp Profiler.h Traceback.h UtilMethods.cpp UtilMethods.h)

include_directories(${MADGINE_SOURCE_DIR}/src)

add_library(Madgine SHARED ${sources} madginelib.cpp)

add_precompiled_header(Madgine src/madginelib.h FORCEINCLUDE SOURCE_CXX madginelib.cpp)

target_link_libraries (Madgine wsock32 ws2_32)

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
 
if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)