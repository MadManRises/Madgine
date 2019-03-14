

Project(AndroidInput)

set(sources "")
FolderSources(sources "" src androidinputhandler.cpp androidinputhandler.h androidinputlib.h)


add_plugin(AndroidInput Madgine Input ${sources} PRECOMPILED_HEADER src/androidinputlib.h)



target_link_libraries(AndroidInput PUBLIC Client PRIVATE android)
