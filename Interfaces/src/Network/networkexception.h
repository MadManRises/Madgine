#pragma once

namespace Engine {
	namespace Network {

		enum class NetworkError {
			UNKNOWN_ERROR
		};

		class NetworkException : public std::exception {
		public:
			NetworkException(int errorCode);
			NetworkException(NetworkError error);

			NetworkError convertWSAError(int errorCode);

			NetworkError errorCode();

		private:
			NetworkError mErrorCode;
		};

	}
}

