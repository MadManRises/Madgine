project(Python3)

find_package (Python3 COMPONENTS Development.Embed)

if (NOT Python_Development.Module_FOUND)
	return()
endif()


set(sources "")
FolderSources(sources "" src)
FolderSources(sources src Python3 python3env.cpp python3env.h python3fileloader.cpp python3fileloader.h python3lib.h python3forward.h python3streamredirect.cpp python3streamredirect.h)
	FolderSources(sources Python3 util pyapifunction.cpp pyapifunction.h pyboundapifunction.cpp pyboundapifunction.h pymoduleptr.cpp pymoduleptr.h pyobjectptr.cpp pyobjectptr.h pyobjectutil.cpp pyobjectutil.h pyownedscopeptr.cpp pyownedscopeptr.h pyscopeiterator.cpp pyscopeiterator.h python3lock.cpp python3lock.h pytypedscopeptr.cpp pytypedscopeptr.h pyvirtualiterator.cpp pyvirtualiterator.h pyvirtualrange.cpp pyvirtualrange.h)
		FolderSources(sources util math pyquaternion.cpp pyquaternion.h pyvector3.cpp pyvector3.h)

add_plugin(Python3 Madgine Scripting ${sources} PRECOMPILED_HEADER src/Python3/python3lib.h)

target_link_plugins(Python3 Base)

target_link_libraries(Python3 PUBLIC Meta Python3::Python)


set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools)
		FolderSources(sources Madgine_Tools Python3 python3immediatewindow.cpp python3immediatewindow.h)

add_tools(Python3 ${sources} PRECOMPILED_HEADER tools/Madgine_Tools/Python3/python3toolslib.h)

target_link_plugins(Python3Tools Tools)


project(Python3Nodes)

set(sources "")
FolderSources(sources "" nodes)
	FolderSources(sources nodes Madgine python3nodesforward.h python3nodeslib.h)
		FolderSources(sources Madgine scripting)
			FolderSources(sources scripting Python3)
				FolderSources(sources Python3 nodes python3functionnode.cpp python3functionnode.h)

add_plugin(Python3Nodes Madgine Scripting ${sources} PRECOMPILED_HEADER nodes/Madgine/python3nodeslib.h)

target_link_plugins(Python3Nodes Python3 NodeGraph)





