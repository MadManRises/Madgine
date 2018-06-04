PROJECT(Maditor_View)

set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTORCC ON)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set (CMAKE_AUTOMOC_MOC_OPTIONS "-bmaditorviewlib.h")


set (sources "")
FolderSources(sources "" "" componentview.h editorsettingswidget.cpp editorsettingswidget.h editorsettingswidget.ui maditorview.cpp maditorview.h maditorviewforward.h maditorviewlib.cpp maditorviewlib.h mainwindow.cpp mainwindow.h mainwindow.ui resources.qrc treeview.cpp treeview.h)
	FolderSources(sources "" application applicationlog.cpp applicationlog.h applicationlog.ui applicationview.cpp applicationview.h applicationwindow.cpp applicationwindow.h appstatswidget.cpp appstatswidget.h appstatswidget.ui configwidget.cpp configwidget.h configwidget.ui)
		FolderSources(sources application inspector inspectorview.cpp inspectorview.h inspectorview.ui valuetypeedit.cpp valuetypeedit.h)
	FolderSources(sources "" dialogs dialogmanager.cpp dialogmanager.h newclassdialog.cpp newclassdialog.h newclassdialog.ui newguihandlerdialog.cpp newguihandlerdialog.h newguihandlerdialog.ui newmoduledialog.cpp newmoduledialog.h newmoduledialog.ui newotherclassdialog.cpp newotherclassdialog.h newotherclassdialog.ui newprojectdialog.cpp newprojectdialog.h newprojectdialog.ui settingsdialog.cpp settingsdialog.h settingsdialog.ui settingstab.h)
	FolderSources(sources "" documents centralwidget.cpp centralwidget.h documenttabbar.cpp documenttabbar.h documenttabwidget.cpp documenttabwidget.h documentview.cpp documentview.h independentwindowspawner.h windowspawner.h)
	FolderSources(sources "" logs logsview.cpp logsview.h logwidget.cpp logwidget.h)
	FolderSources(sources "" project filesview.cpp filesview.h modulepropertieswidget.cpp modulepropertieswidget.h modulepropertieswidget.ui projectview.cpp projectview.h propertieswidget.cpp propertieswidget.h propertieswidget.ui)

add_library(Maditor_View SHARED ${sources})

add_precompiled_header(Maditor_View maditorviewlib.h FORCEINCLUDE SOURCE_CXX maditorviewlib.cpp)

target_link_libraries(Maditor_View PRIVATE Maditor_Model)

install_to_workspace(Maditor TARGETS Maditor_View)



