#pragma once

#include <qobject.h>

#include "forward.h"


namespace Maditor {
	namespace Model {

		class Log : public QObject {
			Q_OBJECT

		public:
			virtual std::string getName() = 0;

		signals:
			void openScriptFile(const QString &path, int line);
			void messageReceived(const QString &msg);

		};

	}
}