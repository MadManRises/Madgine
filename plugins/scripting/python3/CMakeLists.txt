project(Python3)

set(PYTHON3_DIR "C:\\Python39" CACHE PATH "Root-folder of Python3 installation to use")



set(sources "")
FolderSources(sources "" src)
FolderSources(sources src Python3 python3env.cpp python3env.h python3fileloader.cpp python3fileloader.h python3lib.h python3forward.h python3streamredirect.cpp python3streamredirect.h)
	FolderSources(sources Python3 util pyapifunction.cpp pyapifunction.h pyboundapifunction.cpp pyboundapifunction.h pymoduleptr.cpp pymoduleptr.h pyobjectptr.cpp pyobjectptr.h pyobjectutil.cpp pyobjectutil.h pyownedscopeptr.cpp pyownedscopeptr.h pyscopeiterator.cpp pyscopeiterator.h python3lock.cpp python3lock.h pytypedscopeptr.cpp pytypedscopeptr.h pyvirtualiterator.cpp pyvirtualiterator.h pyvirtualrange.cpp pyvirtualrange.h)
		FolderSources(sources util math pyquaternion.cpp pyquaternion.h pyvector3.cpp pyvector3.h)

add_plugin(Python3 Madgine Scripting ${sources} PRECOMPILED_HEADER src/Python3/python3lib.h)

target_link_plugins(Python3 Base)

target_link_libraries(Python3 PUBLIC Meta)

project(Python3Nodes)
set(sources "")
FolderSources(sources "" nodes)
	FolderSources(sources nodes Madgine python3nodesforward.h python3nodeslib.h)
		FolderSources(sources Madgine scripting)
			FolderSources(sources scripting Python3)
				FolderSources(sources Python3 nodes python3functionnode.cpp python3functionnode.h)

add_plugin(Python3Nodes Madgine Scripting ${sources} PRECOMPILED_HEADER nodes/Madgine/python3nodeslib.h)

target_link_plugins(Python3Nodes Python3 NodeGraph)


set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Madgine_Tools)
		FolderSources(sources Madgine_Tools Python3 python3immediatewindow.cpp python3immediatewindow.h)

add_tools(Python3 ${sources} PRECOMPILED_HEADER tools/Madgine_Tools/Python3/python3toolslib.h)

target_link_plugins(Python3Tools Tools)


if (NOT WINDOWS)
	find_file (PYTHON3_LIB "python3.so" ${PYTHON3_DIR}/libs)

	if (NOT PYTHON3_LIB)
		message(SEND_ERROR "Could not find file '${PYTHON3_LIB_NAME}' in PYTHON3_DIR (${PYTHON3_DIR})!")
	endif()

	target_link_libraries(Python3 PUBLIC ${PYTHON3_LIB})
else()
	target_link_directories(Python3 PUBLIC ${PYTHON3_DIR}/libs)
endif()


find_path (PYTHON3_INCLUDE "Python.h" ${PYTHON3_DIR}/include)

if (NOT PYTHON3_INCLUDE)
	message(SEND_ERROR "Could not find file 'Python.h' in PYTHON3_DIR (${PYTHON3_DIR})!")
endif()

target_include_directories(Python3 PUBLIC ${PYTHON3_INCLUDE})

