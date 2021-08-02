
file(GLOB_RECURSE files ${SOURCE}/*)

foreach(f ${files})

	file(COPY ${f} DESTINATION ${TARGET})

	MESSAGE(STATUS "Copying file ${f} to ${TARGET}")

endforeach()
