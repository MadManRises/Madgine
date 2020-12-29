
add_subdirectory(ini)
set_target_properties(Ini PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(xml)
set_target_properties(Xml PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(network)
set_target_properties(Network PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(filesystem)
set_target_properties(Filesystem PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(memory)
set_target_properties(Memory PROPERTIES EXCLUDE_FROM_ALL TRUE)
