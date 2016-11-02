#pragma once


namespace Maditor {
	namespace Model {

		class Log : public QObject {
			Q_OBJECT

		public:
			virtual std::string getName() = 0;

		signals:
			void messageReceived(const QString &msg);

		};

	}
}