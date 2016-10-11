#pragma once

#include <qobject.h>

#include <Windows.h>

namespace Maditor {
	namespace Model {
		namespace Editors {
			class VSLink : public QObject {
				Q_OBJECT

			public:
				VSLink();


			public slots :
				void openVS();

			private:
				void createInstance();

				bool mIsRunning;

				HANDLE mProcessHandle;

			};
		}
	}
}