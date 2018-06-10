cmake_minimum_required (VERSION 2.6)

cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0020 OLD)
cmake_policy(SET CMP0049 OLD)
cmake_policy(SET CMP0022 NEW)



set(QT_PATH "" CACHE PATH "Path to Qt-find script")

set(CMAKE_PREFIX_PATH
  ${CMAKE_PREFIX_PATH}
	"${QT_PATH}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
)


#Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(QScintilla REQUIRED)
find_package(Qt5Network REQUIRED)


include_directories(${Maditor_SOURCE_DIR} ${Madgine_SOURCE_DIR}/Madgine/src)

add_subdirectory(shared)

add_subdirectory(launcher)

add_subdirectory(model)

add_subdirectory(tools)

add_subdirectory(view)


if (BUILD_TESTING)
	add_subdirectory(test)
endif(BUILD_TESTING)

add_executable(Maditor main.cpp)
add_dependencies(Maditor Embedded_Client_Launcher Embedded_Server_Launcher ${TOOLS_LIBS})
target_link_libraries(Maditor Maditor_Model Maditor_View)





file(GENERATE OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/addons.txt CONTENT "${TOOLS_BINS}")


get_filename_component(CMAKE_BIN ${CMAKE_COMMAND} NAME)
get_filename_component(CMAKE_BIN_PATH ${CMAKE_COMMAND} DIRECTORY)

add_custom_command(TARGET Maditor POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/templateproject
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/templates
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/cmake
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/templateproject ${CMAKE_CURRENT_BINARY_DIR}/templateproject
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/templates ${CMAKE_CURRENT_BINARY_DIR}/templates
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_CURRENT_BINARY_DIR}/cmake
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_COMMAND} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/../share
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/../doc
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BIN_PATH}/../share ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/../share
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BIN_PATH}/../doc ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/../doc
)




install(PROGRAMS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/${CMAKE_BIN} DESTINATION bin COMPONENT Maditor)

install(DIRECTORY ${CMAKE_BIN_PATH}/../share DESTINATION . COMPONENT Maditor)
install(DIRECTORY ${CMAKE_BIN_PATH}/../doc DESTINATION . COMPONENT Maditor)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/templateproject DESTINATION Maditor COMPONENT Maditor)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/templates DESTINATION Maditor COMPONENT Maditor)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmake DESTINATION Maditor COMPONENT Maditor)




# Windows specific build steps
if(WIN32)
    # Run winddeployqt if it can be found
    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${QT_PATH} ENV QTDIR PATH_SUFFIXES bin)
    add_custom_command(TARGET Maditor POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:Maditor> --dir ${CMAKE_BINARY_DIR}/qt_dep/bin)

	install(DIRECTORY ${CMAKE_BINARY_DIR}/qt_dep/bin DESTINATION . COMPONENT Maditor)
endif()

install(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/addons.txt DESTINATION bin COMPONENT Maditor)

install_to_workspace(Maditor TARGETS Maditor)

cpack_add_component(Maditor)