#include "../interfaceslib.h"


#include "socketapi.h"

#undef NO_ERROR

namespace Engine
{
	namespace Network
	{

		std::string SocketAPI::getErrorMsg()
		{
			switch(getError())
			{
			case NO_ERROR:
				return "No Error";
				break;
			case WOULD_BLOCK: 
				return "Request would block an operation marked as non-blocking";
				break;
			case ALREADY_CONNECTED: 
				return "The manager is already connected";
				break;
			case TIMEOUT: 
				return "Timeout";
				break;
			case NO_SERVER: 
				return "The manager is not a server";
				break;
			case NO_CONNECTION: 
				return "Accepting the connection failed";
				break;
			case CONNECTION_REFUSED: 
				return "The connection was refused by the host";
				break;
			case UNKNOWN_ERROR: 
				return "Unknown Error (possibly native error code: "s + std::to_string(getAPIError()) + ")";
				break;
			default: 
				std::terminate();
			}
		}


	}
}

