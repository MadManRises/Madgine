#pragma once

#include "generators/cmakeproject.h"
#include "../platform/processlauncher.h"

namespace Maditor {
	namespace Model {

		class MADITOR_MODEL_EXPORT CmakeServer : public QObject {
			Q_OBJECT

			enum State
			{
				WAITING_FOR_HELLO,
				IDLE,
				CONFIGURING,
				GENERATING
			};

		public:
			CmakeServer(const QDir &root, const QDir &buildDir, const QString &name);

			void generate();

			void compute();

			Generators::CmakeProject *project();

		private slots:
			void receive(const std::string &msg);
			void handleMessage(const QJsonDocument &msg);
			void sendMessage(const QJsonDocument &msg);
			void sendRequest(State newState, const QJsonDocument &msg);

		signals:
			void statusUpdate(const QString &, int = 0);
			void processingStarted();
			void processingEnded();
			void progressUpdate(int);

		protected:
			virtual void timerEvent(QTimerEvent* event) override;

		private:
			Generators::CmakeProject mProject;
			Platform::ProcessLauncher mLauncher;
			State mState;
			bool mGotHeader;
			bool mDirty;
			std::string mMsgBuffer;

			static const std::string sHeader;
			static const std::string sFooter;
		};

		
	}
}
