#include "interfaceslib.h"

#include "networkexception.h"

#include <WinSock2.h>

namespace Engine {
	namespace Network {

		
		NetworkException::NetworkException(int errorCode) :
			mErrorCode(convertWSAError(errorCode)) {
		}

		NetworkException::NetworkException(NetworkError error) :
			mErrorCode(error) {
		}

		NetworkError NetworkException::convertWSAError(int errorCode) {
			switch (errorCode) {
			default:
				return NetworkError::UNKNOWN_ERROR;
			}
		}

		NetworkError NetworkException::errorCode() {
			return mErrorCode;
		}

	}
}
