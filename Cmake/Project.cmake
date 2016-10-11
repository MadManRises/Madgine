cmake_policy(SET CMP0011 OLD)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0049 NEW)

function(FolderSources VARNAME PATHVAR FOLDER )
  
  set (PATH ${${PATHVAR}})

  if("${FOLDER}" STREQUAL "")
	set (FULLFOLDER ${PATH})
  else()
	if("${PATH}" STREQUAL "")
	  set (FULLFOLDER ${FOLDER})
	else()
	  set (FULLFOLDER ${PATH}\\\\${FOLDER})
	endif()
  endif()

  if("${FULLFOLDER}" STREQUAL "")
    set (FULLPATH "")
  else()
	set (FULLPATH ${FULLFOLDER}\\\\) 
  endif()
  
  set (_sources "")

  foreach( src ${ARGN})
	set (_sources ${_sources} ${FULLPATH}${src})
  endforeach() 

  set (${VARNAME} ${${VARNAME}} ${_sources} PARENT_SCOPE)
  

  SOURCE_GROUP ("${FULLFOLDER}" FILES ${_sources})
  if (NOT "${FOLDER}" STREQUAL "")
	set (${FOLDER} ${FULLFOLDER} PARENT_SCOPE)
  endif()

  
 
endfunction()