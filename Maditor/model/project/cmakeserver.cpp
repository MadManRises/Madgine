#include "maditormodellib.h"

#include "cmakeserver.h"

#include "Madgine/plugins/plugin.h"
#include "dialogmanager.h"


namespace Maditor
{
	namespace Model
	{
		const std::string CmakeServer::sHeader = "\n[== \"CMake Server\" ==[\n";
		const std::string CmakeServer::sFooter = "\n]== \"CMake Server\" ==]\n";

		CmakeServer::CmakeServer(const QDir& root, const QDir& buildDir, const QString& name) :
			mProject(root, buildDir, name),
			mState(WAITING_FOR_HELLO),
			mGotHeader(false),
			mDirty(true)
		{
			connect(&mLauncher, &Platform::ProcessLauncher::outputReceived, this, &CmakeServer::receive);

			std::string cmd = QString("\"%1/cmake\" -E server --debug --experimental").arg(
				QString::fromStdString(Engine::Plugins::Plugin::runtimePath().generic_string())
			).toStdString();
						

			assert(mLauncher.launch(cmd));

			startTimer(10);

		}

		void CmakeServer::generate()
		{
			mProject.generate();
		}

		void CmakeServer::compute()
		{
			assert(mState == IDLE);

			QJsonObject msg;

			msg.insert("type", "configure");
			
			sendRequest(CONFIGURING, QJsonDocument(msg));

		}

		Generators::CmakeProject* CmakeServer::project()
		{
			return &mProject;
		}

		void CmakeServer::receive(const std::string& msg)
		{
			mMsgBuffer += msg;
		}

		void CmakeServer::handleMessage(const QJsonDocument& msg)
		{
			QJsonObject o = msg.object();

			QString type = o["type"].toString();

			if (type == "signal")
			{
				QString name = o["name"].toString();
				if (name == "fileChange")
				{
					
				}
				else if (name == "dirty")
				{
					mDirty = true;
				}
				else
				{
					LOG(QString(msg.toJson()).toStdString());
				}				
			}
			else if (type == "hello") {
				int highestMajorVersion;
				for (QJsonValueRef v : o["supportedProtocolVersions"].toArray())
				{
					highestMajorVersion = std::max(highestMajorVersion, v.toObject()["major"].toInt());
				}
				QJsonObject response;

				QJsonObject protocolVersion;
				protocolVersion.insert("major", highestMajorVersion);
				response.insert("protocolVersion", protocolVersion);

				response.insert("type", "handshake");
				response.insert("buildDirectory", mProject.buildDir().path());
				response.insert("sourceDirectory", mProject.root().path());
				response.insert("generator", CMAKE_GENERATOR);

				sendMessage(QJsonDocument(response));
			}
			else if (type == "reply")
			{
				QString inReplyTo = o["inReplyTo"].toString();
				if (inReplyTo == "handshake") {
					assert(mState == WAITING_FOR_HELLO);
					mState = IDLE;
				}
				else if (inReplyTo == "configure") {
					assert(mState == CONFIGURING);

					QJsonObject msg;

					msg.insert("type", "compute");

					sendRequest(GENERATING, QJsonDocument(msg));
				}
				else if (inReplyTo == "compute")
				{
					assert(mState == GENERATING);

					emit processingEnded();
					mDirty = false;
					mState = IDLE;
				}
				else
				{
					LOG(QString(msg.toJson()).toStdString());
				}

			}
			else if (type == "progress")
			{
				emit statusUpdate(o["progressMessage"].toString());
				int min = o["progressMinimum"].toInt();
				int max = o["progressMaximum"].toInt();
				int cur = o["progressCurrent"].toInt();
				emit progressUpdate((1000 * (cur - min)) / (max - min));
			}
			else if (type == "message")
			{
				QString message = o["message"].toString();
				QString title = o["title"].toString();
				if (title == "Warning")
				{
					LOG_WARNING(message.toStdString());
				}
				else if (title.isEmpty())
				{
					emit statusUpdate(message);
				}
				else
				{
					LOG(QString(msg.toJson()).toStdString());
				}
			}
			else
			{
				LOG(QString(msg.toJson()).toStdString());
			}

		}

		void CmakeServer::sendRequest(State newState, const QJsonDocument& msg)
		{
			mState = newState;

			sendMessage(msg);

			emit processingStarted();
			emit progressUpdate(0);
		}

		void CmakeServer::sendMessage(const QJsonDocument& msg)
		{
			mLauncher.sendInput(sHeader + msg.toJson().toStdString() + sFooter);		
		}

		void CmakeServer::timerEvent(QTimerEvent* event)
		{			
			if (!mGotHeader)
			{
				if (mMsgBuffer.size() < sHeader.size())
					return;
				assert(mMsgBuffer.substr(0, sHeader.size()) == sHeader);
				mMsgBuffer = mMsgBuffer.substr(sHeader.size());
				mGotHeader = true;
			}
			if (mGotHeader)
			{
				size_t i = mMsgBuffer.find(sFooter);
				if (i == std::string::npos)
					return;
				handleMessage(QJsonDocument::fromJson(QString::fromStdString(mMsgBuffer.substr(0, i)).toUtf8()));
				mMsgBuffer = mMsgBuffer.substr(i + sFooter.size());
				mGotHeader = false;
			}

			if (mMsgBuffer.empty())
			{
				if (mDirty && mState == IDLE)
				{
					compute();
				}
			}

		}
	}
}
