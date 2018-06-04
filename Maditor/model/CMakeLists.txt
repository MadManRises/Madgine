PROJECT(Maditor_Model)

find_package(Threads REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set (CMAKE_AUTOMOC_MOC_OPTIONS "-bmaditormodellib.h")


add_definitions(-DCMAKE_PATH="${CMAKE_COMMAND}")
add_definitions(-DCMAKE_GENERATOR="${CMAKE_GENERATOR}")
add_definitions(-DQSCINTILLA_DLL)


set (sources "")
FolderSources(sources "" "" dialogmanager.h maditormodelforward.h maditormodellib.cpp maditormodellib.h maditor.cpp maditor.h maditorlog.cpp maditorlog.h tablemodel.cpp tablemodel.h treeitem.cpp treeitem.h treemodel.cpp treemodel.h treesorter.cpp treesorter.h)
	FolderSources(sources "" addons addon.cpp addon.h)
	FolderSources(sources "" application applicationlauncher.cpp applicationlauncher.h embeddedlauncher.cpp embeddedlauncher.h inputwrapper.cpp inputwrapper.h logreceiver.cpp logreceiver.h moduleloader.cpp moduleloader.h ogrewindow.cpp ogrewindow.h profilermodel.cpp profilermodel.h standalonelauncher.cpp standalonelauncher.h statsmodel.cpp statsmodel.h tableunit.cpp tableunit.h treeunit.cpp treeunit.h utilmodel.cpp utilmodel.h)
		FolderSources(sources application inspector inspector.cpp inspector.h scopewrapper.cpp scopewrapper.h)
	FolderSources(sources "" documents document.cpp document.h documentstore.h)
	FolderSources(sources "" logs logsorterfilter.cpp logsorterfilter.h logtablemodel.cpp logtablemodel.h)
	FolderSources(sources "" project applicationconfig.cpp applicationconfig.h configlist.cpp configlist.h module.cpp module.h modulelist.cpp modulelist.h moduleselection.cpp moduleselection.h project.cpp project.h projectelement.cpp projectelement.h)
		FolderSources(sources project generators classgenerator.cpp classgenerator.h classgeneratorfactory.cpp classgeneratorfactory.h cmakegenerator.cpp cmakegenerator.h cmakeproject.cpp cmakeproject.h commandline.cpp commandline.h dotgenerator.cpp dotgenerator.h entitycomponentgenerator.cpp entitycomponentgenerator.h gamehandlergenerator.cpp gamehandlergenerator.h generator.cpp generator.h globalapigenerator.cpp globalapigenerator.h guihandlergenerator.cpp guihandlergenerator.h headerguardgenerator.cpp headerguardgenerator.h otherclassgenerator.cpp otherclassgenerator.h scenecomponentgenerator.cpp scenecomponentgenerator.h serverclassgenerator.cpp serverclassgenerator.h)


add_library(Maditor_Model SHARED ${sources})

add_precompiled_header(Maditor_Model maditormodellib.h FORCEINCLUDE SOURCE_CXX maditormodellib.cpp)

target_link_libraries(Maditor_Model PUBLIC Maditor_Shared Qt5::Xml Qt5::Gui Qt5::Widgets ${QSCINTILLA2_LIBRARY})

target_link_libraries(Maditor_Model PRIVATE ${CMAKE_THREAD_LIBS_INIT})

if (CMAKE_COMPILER_IS_GNUCXX)
	target_link_libraries(Maditor_Model PRIVATE dl)
endif (CMAKE_COMPILER_IS_GNUCXX)

install_to_workspace(Maditor TARGETS Maditor_Model)


