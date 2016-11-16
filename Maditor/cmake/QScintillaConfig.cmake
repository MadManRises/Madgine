# Module file for QScintilla - compiled with Qt5. These variables are available:
# QSCINTILLA2_FOUND = Status of QScintilla
# QSCINTILLA2_INCLUDE_DIR = QScintilla include dir
# QSCINTILLA2_LIBRARY = QScintilla library

# Check
if(${Qt5Widgets_FOUND})

    # Set as not found
    set(QSCINTILLA2_FOUND false)

    # Iterate over the include list of the Qt5Widgets module
    foreach(TEMPPATH in ${Qt5Widgets_INCLUDE_DIRS})

        # Check for a Qsci directory
        find_path(QSCINTILLA2_INCLUDE_DIR qsciglobal.h ${TEMPPATH}/Qsci)

        # Found - break loop
        if(QSCINTILLA2_INCLUDE_DIR)
            break()
        endif()

    endforeach()

    # Check
    if(QSCINTILLA2_INCLUDE_DIR)

        # Get Qt5Widgets library and cut off the library name
        get_target_property(QT5_WIDGETSLIBRARY Qt5::Widgets LOCATION)
        get_filename_component(QT5_WIDGETSLIBRARYPATH ${QT5_WIDGETSLIBRARY} PATH)

        # Add library
        set(LIBRARYPATH ${QT5_WIDGETSLIBRARYPATH} "/usr/lib/" "/usr/local/lib")
        find_library(QSCINTILLA2_LIBRARY NAMES libqscintilla2.a qscintilla2d.lib PATHS ${LIBRARYPATH})

        # Check
        if(QSCINTILLA2_LIBRARY)
            # Enable library
            set(QSCINTILLA2_FOUND true)
            mark_as_advanced(QSCINTILLA2_INCLUDE_DIR QSCINTILLA2_LIBRARY)
        else()
            message(FATAL_ERROR "QScintilla2 library not found")
        endif()

    else()
        message(FATAL_ERROR "Cannot find QScintilla2 header")
    endif()

else()
    message(FATAL_ERROR "Qt5Widgets module not found")
endif()