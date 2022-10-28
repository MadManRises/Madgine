

foreach (tool ${Tools})
	execute_process(
		COMMAND ${tool} ${InputFile} ${OutputFolder}
		COMMAND_ERROR_IS_FATAL ANY)
endforeach()