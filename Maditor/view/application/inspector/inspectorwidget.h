#pragma once
#include <QOpenGLExtraFunctions>


namespace Maditor
{
	namespace View
	{

		class InspectorWidget : public QOpenGLWindow, private QOpenGLExtraFunctions
		{
		public:
			InspectorWidget(InspectorLayoutData *data, QWidget *parent = nullptr);
			~InspectorWidget();

			void setModel(Model::Inspector *inspector);

			void clearModel();

		private:
			void resetModel();

			void reset();

			void setCurrentItem(Engine::InvScopePtr ptr);
			
			void drawContext();
			

		protected:
			void initializeGL() override;

			void paintGL() override;

			void timerEvent(QTimerEvent* event) override;

		private:
			Model::Inspector *mInspector;

			std::vector<std::pair<QString, Engine::InvScopePtr>> mTrace;

			QMetaObject::Connection mDestroyConnection, mResetConnection;

			Model::ScopeWrapperItem* mCurrentItem;

			InspectorLayoutData *mData;
		};
		
	}
}
