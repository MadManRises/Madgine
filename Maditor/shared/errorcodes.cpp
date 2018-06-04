#include "maditorsharedlib.h"

#include "errorcodes.h"

namespace Maditor {
	namespace Shared {

		std::string to_string(ErrorCode cause) {
			switch (cause) {
			case FAILED_START_SERVER:
				return "Failure starting debug-server";
			case MADITOR_CONNECTION_TIMEOUT:
				return "Timeout waiting for Maditor to connect";
			case DEBUGGER_CONNECTION_TIMEOUT:
				return "Timeout waiting for Debugger to connect";
			case MODULE_LOAD_FAILED:
				return "Failed to load Module-List";
			case APP_INIT_FAILED:
				return "Application-initialization failed";
			case MADITOR_DISCONNECTED:
				return "Maditor disconnected";
			case FAILED_CREATE_SERVER_CLASS:
				return "Could not create the serverclass-instance";
			case ILLEGAL_ARGUMENTS:
				return "Illegal arguments for launcher";
			case KILL_USER_REQUEST:
				return "User request";
			case KILL_PING_TIMEOUT:
				return "No Response from App";
			case KILL_FAILED_CONNECTION:
				return "Unable to connect to process";
			case KILL_NO_APPLICATION_NOTIFICATION:
				return "Application data not set up correctly";
			case KILL_CLEANUP:
				return "Cleaning up application";
			case APPLICATION_INFO_TIMEOUT:
				return "Timeout waiting for Application-Info";
			case UNSUPPORTED_LAUNCHER_TYPE:
				return "Unsupported Launcher-Type";
			default:
				return "unknown error";
			}
		}

	}
}
