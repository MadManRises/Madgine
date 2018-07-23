cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)
cmake_policy(SET CMP0057 NEW)


Project(Client)      
  

set(sources "")
FolderSources(sources "" "../../src")
FolderSources(sources "../../src" Madgine clientforward.h clientlib.h)
	FolderSources(sources Madgine app clientapplication.cpp clientapplication.h clientappsettings.h)
	#FolderSources(sources Madgine gui guievents.h guisystem.cpp guisystem.h windownames.h)
	#	FolderSources(sources gui mygui myguibar.cpp myguibar.h myguibutton.cpp myguibutton.h myguicheckbox.cpp myguicheckbox.h myguicombobox.cpp myguicombobox.h myguilabel.cpp myguilabel.h myguilauncher.cpp myguilauncher.h myguitextbox.cpp myguitextbox.h myguitextureDrawer.cpp myguitextureDrawer.h myguiwindow.cpp myguiwindow.h)
	#	FolderSources(sources gui windows bar.h button.h checkbox.h combobox.h label.h tabwindow.h textbox.h texturedrawer.h window.cpp window.h windowcontainer.cpp windowcontainer.h )
	FolderSources(sources Madgine gui guisystem.cpp guisystem.h)
		FolderSources(sources gui windows bar.h button.cpp button.h checkbox.h classid.h combobox.h image.h label.h scenewindow.h tabwindow.h textbox.h window.cpp window.h windowclass.h)
	FolderSources(sources Madgine input inputhandler.cpp inputHandler.h)
	FolderSources(sources Madgine scene camera.h cameramanager.h cameramanager.cpp)
	FolderSources(sources Madgine ui gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h uimanager.cpp uimanager.h)



add_library(Client SHARED ${sources} clientlib.cpp)

add_precompiled_header(Client ../../src/Madgine/clientlib.h FORCEINCLUDE SOURCE_CXX clientlib.cpp)

target_link_libraries(Client PUBLIC Base)

target_compile_definitions(Client INTERFACE MADGINE_CLIENT_BUILD)

target_compile_definitions(Client PUBLIC SHARED_LIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}")

if (BUILD_TESTING)
	add_subdirectory(test)
endif()


install_to_workspace(Madgine TARGETS Client EXPORT_LIB)
