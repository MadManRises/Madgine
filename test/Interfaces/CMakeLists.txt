cmake_policy(SET CMP0048 NEW)


project(Interfaces_Test)

set(sources "")
FolderSources(sources "" "" integrationtests.cpp)
	FolderSources(sources "" generic valuetypetest.cpp)
	FolderSources(sources "" network networkmanagertest.cpp)
	FolderSources(sources "" serialize)
		FolderSources(sources serialize container containertest.cpp)
		FolderSources(sources serialize streams)
			FolderSources(sources streams debugging streamdebuggingtest.cpp)

add_gtest(Interfaces_Test ${sources})

target_link_libraries(Interfaces_Test PRIVATE Interfaces)


