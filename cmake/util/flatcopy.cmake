
file(GLOB_RECURSE files ${SOURCE}/*)

foreach(f ${files})

	file(COPY ${f} DESTINATION ${TARGET})

endforeach()
