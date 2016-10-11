cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0049 OLD)

PROJECT(Maditor)

set(QT_PATH "" CACHE PATH "Path to Qt-find script")

set(CMAKE_PREFIX_PATH
	"${QT_PATH}"
)

add_definitions(-DCMAKE_PATH="${CMAKE_COMMAND}")
add_definitions(-DCMAKE_GENERATOR="${CMAKE_GENERATOR}")
add_definitions(-DQSCINTILLA_DLL)

set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTORCC ON)

set (sources "")
FolderSources(sources "" src main.cpp)
FolderSources(sources src Model editor.cpp editor.h editorforward.h TreeItem.cpp TreeItem.h)
FolderSources(sources Model Editors ScriptAPI.cpp ScriptAPI.h ScriptEditor.cpp ScriptEditor.h ScriptEditorModel.cpp ScriptEditorModel.h VSLink.cpp VSLink.h)
FolderSources(sources Model Engine ApplicationWrapper.cpp ApplicationWrapper.h InputWrapper.cpp InputWrapper.h ModuleLoader.cpp ModuleLoader.h )
FolderSources(sources Engine Watcher ApplicationWatcher.cpp ApplicationWatcher.h LogsWatcher.cpp LogsWatcher.h LogTableModel.cpp LogTableModel.h LogWatcher.cpp LogWatcher.h PerformanceWatcher.cpp PerformanceWatcher.h ProfilerNode.cpp ProfilerNode.h ResourceGroupItem.cpp ResourceGroupItem.h ResourceItem.cpp ResourceItem.h ResourceWatcher.cpp ResourceWatcher.h)
FolderSources(sources Model Project Module.cpp Module.h Project.cpp Project.h ProjectElement.cpp ProjectElement.h)
FolderSources(sources Project Generator ClassGenerator.cpp ClassGenerator.h ClassGeneratorFactory.cpp ClassGeneratorFactory.h CmakeGenerator.cpp CmakeGenerator.h CmakeProject.cpp CmakeProject.h CmakeSubProject.cpp CmakeSubProject.h CommandLine.cpp CommandLine.h DotGenerator.cpp DotGenerator.h EntityComponentGenerator.cpp EntityComponentGenerator.h GameHandlerGenerator.cpp GameHandlerGenerator.h GlobalAPIGenerator.cpp GlobalAPIGenerator.h GuiHandlerGenerator.cpp GuiHandlerGenerator.h HeaderGuardGenerator.cpp HeaderGuardGenerator.h OtherClassGenerator.cpp OtherClassGenerator.h SceneComponentGenerator.cpp SceneComponentGenerator.h Generator.cpp Generator.h)
FolderSources(sources src View mainwindow.cpp mainwindow.h resources.qrc)
FolderSources(sources View Dialogs DialogManager.cpp DialogManager.h ModulePropertiesDialog.cpp ModulePropertiesDialog.h NewClassDialog.cpp NewClassDialog.h NewGuiHandlerDialog.cpp NewGuiHandlerDialog.h NewModuleDialog.cpp NewModuleDialog.h)
FolderSources(sources View Editors scripteditortabwidget.cpp scripteditortabwidget.h)
FolderSources(sources View OgreWidgets LogsTabWidget.cpp LogsTabWidget.h OgreWindow.cpp OgreWindow.h RenderStatsWidget.cpp RenderStatsWidget.h)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)


include_directories(${MADGINE_SOURCE_DIR})
include_directories(${Maditor_SOURCE_DIR}/src)

if (OGRE_FOUND)
  include_directories(${OGRE_INCLUDE_DIRS} ${OGRE_Terrain_INCLUDE_DIRS} ${OGRE_RTShaderSystem_INCLUDE_DIRS})
endif (OGRE_FOUND)

add_executable(Maditor ${sources})



target_link_libraries(Maditor Madgine)

#Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(QScintilla REQUIRED)

# Use the Widgets module from Qt 5.
target_link_libraries(Maditor Qt5::Widgets Qt5::Xml ${QSCINTILLA2_LIBRARY})