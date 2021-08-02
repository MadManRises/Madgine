
file(GLOB_RECURSE files ${SOURCE}/*)

file(INSTALL ${files} DESTINATION ${TARGET})
