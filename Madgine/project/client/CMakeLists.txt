cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)
cmake_policy(SET CMP0057 NEW)


Project(Client)      
  

  	set(CMAKE_MODULE_PATH
		"${CMAKE_MODULE_PATH}"
		"${Client_SOURCE_DIR}/cmake"
	)  

set(sources "")
FolderSources(sources "" "../../src")
FolderSources(sources "../../src" Madgine clientforward.h clientlib.h)
	FolderSources(sources Madgine app clientapplication.cpp clientapplication.h clientappsettings.h)
	#FolderSources(sources Madgine gui guievents.h guisystem.cpp guisystem.h windownames.h)
	#	FolderSources(sources gui mygui myguibar.cpp myguibar.h myguibutton.cpp myguibutton.h myguicheckbox.cpp myguicheckbox.h myguicombobox.cpp myguicombobox.h myguilabel.cpp myguilabel.h myguilauncher.cpp myguilauncher.h myguitextbox.cpp myguitextbox.h myguitextureDrawer.cpp myguitextureDrawer.h myguiwindow.cpp myguiwindow.h)
	#	FolderSources(sources gui windows bar.h button.h checkbox.h combobox.h label.h tabwindow.h textbox.h texturedrawer.h window.cpp window.h windowcontainer.cpp windowcontainer.h )
	FolderSources(sources Madgine gui guisystem.cpp guisystem.h)
		FolderSources(sources gui widgets bar.h button.cpp button.h checkbox.h classid.h combobox.h image.h label.cpp label.h scenewindow.h tabwidget.h textbox.h toplevelwindow.cpp toplevelwindow.h widget.cpp widget.h widgetclass.h)
	FolderSources(sources Madgine input inputhandler.cpp inputhandler.h)
		FolderSources(sources input OIS OIS.h OISConfig.h OISEffect.cpp OISEffect.h OISEvents.h OISException.cpp OISException.h OISFactoryCreator.h OISForceFeedback.cpp OISForceFeedback.h oisinputhandler.cpp oisinputhandler.h OISInputManager.cpp OISInputManager.h OISInterface.h OISJoyStick.cpp OISJoyStick.h OISKeyboard.cpp OISKeyboard.h OISMouse.h OISMultiTouch.h OISObject.cpp OISObject.h OISPrereqs.h)
		if (WIN32)
			FolderSources(sources OIS win32 Win32ForceFeedback.cpp Win32InputManager.cpp Win32JoyStick.cpp Win32KeyBoard.cpp Win32Mouse.cpp)
		endif()
	FolderSources(sources Madgine scene camera.h cameramanager.h cameramanager.cpp)
	FolderSources(sources Madgine ui gamehandler.cpp gamehandler.h guihandler.cpp guihandler.h handler.cpp handler.h uimanager.cpp uimanager.h)



add_workspace_library(Client  ${LIBRARY_TYPE} ${sources})

add_precompiled_header(Client ../../src/Madgine/clientlib.h FORCEINCLUDE)

target_link_libraries(Client PUBLIC Base)

target_compile_definitions(Client INTERFACE MADGINE_CLIENT_BUILD)

target_compile_definitions(Client PUBLIC SHARED_LIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}")



if (WIN32)
		find_package(WindowsSDK)
	    if (CMAKE_CL_64)
		    set(DIRECTX_ARCH x64)
		else()
			set(DIRECTX_ARCH x86)
		endif()

		if (WINDOWSSDK_FOUND)
			target_link_libraries(Client PRIVATE "dinput8.lib" "dxguid.lib")
	    else()
		    target_link_libraries(Client PRIVATE "${DXSDK_DIR}/Lib/${DIRECTX_ARCH}/dinput8.lib" "${DXSDK_DIR}/Lib/${DIRECTX_ARCH}/dxguid.lib")
		endif()
	endif()


if (BUILD_TESTING)
	add_subdirectory(test)
endif()


install_to_workspace(Madgine TARGETS Client EXPORT_LIB)
