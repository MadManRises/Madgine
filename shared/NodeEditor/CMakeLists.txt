project(imgui_node_editor)

add_library(imgui_canvas STATIC
    Source/imgui_canvas.cpp
    Source/imgui_canvas.h
)
target_include_directories(imgui_canvas PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_link_libraries(imgui_canvas PUBLIC ImGui)

set(_imgui_node_editor_Sources
    imgui_node_editor.h
    Source/crude_json.cpp
    Source/crude_json.h
    Source/imgui_bezier_math.h
    Source/imgui_bezier_math.inl
    Source/imgui_canvas.cpp
    Source/imgui_canvas.h
    Source/imgui_extra_math.h
    Source/imgui_extra_math.inl
    Source/imgui_node_editor_api.cpp
    Source/imgui_node_editor_internal.h
    Source/imgui_node_editor_internal.inl
    Source/imgui_node_editor.cpp
    Support/imgui_node_editor.natvis
)

add_library(${PROJECT_NAME} STATIC ${_imgui_node_editor_Sources})

target_link_libraries(${PROJECT_NAME} PUBLIC ImGui)

target_include_directories(${PROJECT_NAME} PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)
