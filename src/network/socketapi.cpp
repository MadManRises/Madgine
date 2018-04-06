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
			case Serialize::NO_ERROR:
				return "No Error";
				break;
			case Serialize::WOULD_BLOCK: 
				return "Request would block an operation marked as non-blocking";
				break;
			case Serialize::ALREADY_CONNECTED: 
				return "The manager is already connected";
				break;
			case Serialize::TIMEOUT: 
				return "Timeout";
				break;
			case Serialize::NO_SERVER: 
				return "The manager is not a server";
				break;
			case Serialize::NO_CONNECTION: 
				return "Accepting the connection failed";
				break;
			case Serialize::CONNECTION_REFUSED: 
				return "The connection was refused by the host";
				break;
			case Serialize::UNKNOWN_ERROR: 
				return std::string("Unknown Error (possibly native error code: ") + std::to_string(getAPIError()) + ")";
				break;
			default: 
				throw 0;
			}
		}


	}
}

