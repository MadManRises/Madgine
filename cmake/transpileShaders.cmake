
set(debugFlag "")
if (Debug)
	set(debugFlag "-g")
endif()

foreach (tool ${Tools})
	execute_process(
		COMMAND ${tool} ${SourceFile} ${InputFile} ${OutputFolder} ${debugFlag}
		COMMAND_ERROR_IS_FATAL ANY)
endforeach()