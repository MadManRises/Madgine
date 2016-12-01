cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)

set(QT_PATH "" CACHE PATH "Path to Qt-find script")
set(BOOST_PATH "" CACHE PATH "Path to Boost")

set(CMAKE_PREFIX_PATH
	"${QT_PATH}"
	"${CMAKE_CURRENT_LIST_DIR}/CMake"
)

#Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(QScintilla REQUIRED)

add_subdirectory(Tools)

PROJECT(Maditor)

include_directories(${BOOST_PATH})

add_definitions(-DCMAKE_PATH="${CMAKE_COMMAND}")
add_definitions(-DCMAKE_GENERATOR="${CMAKE_GENERATOR}")
add_definitions(-DQSCINTILLA_DLL)

set (CMAKE_AUTOMOC_MOC_OPTIONS "-bmaditorlib.h")

set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTORCC ON)

set (sources "")
FolderSources(sources "" "" maditorlib.cpp)
	FolderSources(sources "" Common ApplicationInfo.h InputInfo.h ModuleLoaderInfo.h ObjectsInfo.h ProcessTalker.h Shared.h SharedBase.h StatsInfo.h)
	FolderSources(sources "" src maditorlib.h madgineinclude.h)
		FolderSources(sources src Addons Addon.cpp Addon.h)
		FolderSources(sources src Model editor.cpp editor.h editorforward.h log.h LogsModel.cpp LogsModel.h TreeItem.cpp TreeItem.h TreeModel.cpp TreeModel.h TreeSorter.cpp TreeSorter.h)
			FolderSources(sources Model Editors EditorManager.cpp EditorManager.h ScriptAPI.cpp ScriptAPI.h ScriptEditor.cpp ScriptEditor.h ScriptEditorModel.cpp ScriptEditorModel.h)
			FolderSources(sources Model Engine ApplicationWrapper.cpp ApplicationWrapper.h InputWrapper.cpp InputWrapper.h ModuleLoader.cpp ModuleLoader.h )
				FolderSources(sources Engine Watcher ApplicationWatcher.cpp ApplicationWatcher.h LogTableModel.cpp LogTableModel.h LogWatcher.cpp LogWatcher.h ObjectsWatcher.cpp ObjectsWatcher.h OgreNodeItem.cpp OgreNodeItem.h OgreSceneWatcher.cpp OgreSceneWatcher.h PerformanceWatcher.cpp PerformanceWatcher.h ProfilerNode.cpp ProfilerNode.h ResourceGroupItem.cpp ResourceGroupItem.h ResourceItem.cpp ResourceItem.h ResourceWatcher.cpp ResourceWatcher.h)
			FolderSources(sources Model Project Module.cpp Module.h ModuleList.cpp ModuleList.h Project.cpp Project.h ProjectElement.cpp ProjectElement.h)
				FolderSources(sources Project Generators ClassGenerator.cpp ClassGenerator.h ClassGeneratorFactory.cpp ClassGeneratorFactory.h CmakeGenerator.cpp CmakeGenerator.h CmakeProject.cpp CmakeProject.h CmakeSubProject.cpp CmakeSubProject.h CommandLine.cpp CommandLine.h DotGenerator.cpp DotGenerator.h EntityComponentGenerator.cpp EntityComponentGenerator.h GameHandlerGenerator.cpp GameHandlerGenerator.h GlobalAPIGenerator.cpp GlobalAPIGenerator.h GuiHandlerGenerator.cpp GuiHandlerGenerator.h HeaderGuardGenerator.cpp HeaderGuardGenerator.h OtherClassGenerator.cpp OtherClassGenerator.h SceneComponentGenerator.cpp SceneComponentGenerator.h Generator.cpp Generator.h)
		FolderSources(sources src View FilesView.cpp FilesView.h mainwindow.cpp mainwindow.h mainwindow.ui resources.qrc TreeView.cpp TreeView.h)
			FolderSources(sources View Dialogs DialogManager.cpp DialogManager.h loadprojectdialog.cpp loadprojectdialog.h loadprojectdialog.ui ModulePropertiesDialog.cpp ModulePropertiesDialog.h NewClassDialog.cpp NewClassDialog.h NewClassDialog.ui NewGuiHandlerDialog.cpp NewGuiHandlerDialog.h NewGuiHandlerDialog.ui NewModuleDialog.cpp NewModuleDialog.h NewModuleDialog.ui settingsdialog.cpp settingsdialog.h settingsdialog.ui)
			FolderSources(sources View Editors scripteditortabwidget.cpp scripteditortabwidget.h)
			FolderSources(sources View OgreWidgets LogsTabWidget.cpp LogsTabWidget.h LogWidget.cpp LogWidget.h LogWidget.ui OgreWindow.cpp OgreWindow.h RenderStatsWidget.cpp RenderStatsWidget.h)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)


include_directories(${MADGINE_SOURCE_DIR}/src)
include_directories(${Maditor_SOURCE_DIR}/src)

if (OGRE_FOUND)
  include_directories(${OGRE_INCLUDE_DIRS} ${OGRE_Terrain_INCLUDE_DIRS} ${OGRE_RTShaderSystem_INCLUDE_DIRS})
endif (OGRE_FOUND)

add_library(Maditor SHARED ${sources})

add_precompiled_header(Maditor src/maditorlib.h FORCEINCLUDE SOURCE_CXX maditorlib.cpp)

target_link_libraries(Maditor Madgine)

# Use the Widgets module from Qt 5.
target_link_libraries(Maditor Qt5::Widgets Qt5::Xml ${QSCINTILLA2_LIBRARY})


set (CMAKE_AUTOMOC OFF)

set (exesources "")
FolderSources(exesources "" "")
FolderSources(exesources "" src main.cpp)

add_executable(Maditor_main ${exesources})
target_link_libraries(Maditor_main Maditor ${TOOLS_LIBS})

set(TOOLS_LIBS ${TOOLS_LIBS} PARENT_SCOPE)

if (BUILD_TESTS)
	add_subdirectory(test)
endif(BUILD_TESTS)

add_subdirectory(Launcher)

add_dependencies(Maditor_main Maditor_Madgine_Launcher)