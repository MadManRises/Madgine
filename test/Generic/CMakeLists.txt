
project(Generic_Test)

set(sources "")
FolderSources(sources "" "" Generic_gtest_main.cc)
	FolderSources(sources "" generic areaviewtest.cpp bitstest.cpp tupleunpackertest.cpp)
		FolderSources(sources generic container generationvectortest.cpp)
	

add_gtest(Generic_Test ${sources})

target_link_libraries(Generic_Test PRIVATE Interfaces CLI)
