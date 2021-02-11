Project(MadgineObject)
	
add_workspace_interface_library(MadgineObject SOURCE_ROOT ..)

install_interface_to_workspace(Madgine TARGETS MadgineObject SOURCE_ROOT ..)