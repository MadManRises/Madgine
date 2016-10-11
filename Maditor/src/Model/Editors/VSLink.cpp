#include "VSLink.h"


namespace Maditor {
	namespace Model {
		namespace Editors {

			VSLink::VSLink() :
				mIsRunning(false) {

			}

			void VSLink::openVS() {
				//if (!mIsRunning)
					createInstance();
			}


			void VSLink::createInstance()
			{
				// additional information
				STARTUPINFO si;
				PROCESS_INFORMATION pi;

				// set the size of the structures
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));

				// start the program up
				CreateProcess(NULL,   // the path
					"cmd.exe /c \"start C:\\Users\\schue\\Documents\\TT\\build\\TT.sln\"",
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					0,              // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&si,            // Pointer to STARTUPINFO structure
					&pi           // Pointer to PROCESS_INFORMATION structure
					);

				mProcessHandle = pi.hProcess;

				// Close process and thread handles. 
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);

				mIsRunning = true;
			}
		}
	}
}
