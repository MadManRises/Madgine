#pragma once
#include <QOpenGLExtraFunctions>


namespace Maditor
{
	namespace View
	{

		class InspectorWidget : public QOpenGLWindow, private QOpenGLExtraFunctions
		{
		public:
			InspectorWidget(QWidget *parent = nullptr);



		protected:
			void initializeGL() override;

			void paintGL() override;

			void timerEvent(QTimerEvent* event) override;
		};
		
	}
}
