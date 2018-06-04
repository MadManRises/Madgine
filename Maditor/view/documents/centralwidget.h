#pragma once

namespace Maditor {
	namespace View {

		class CentralWidget : public QWidget {
			Q_OBJECT
		public:
			CentralWidget(QWidget *parent = nullptr);

			void setWidget(QWidget *w);

			void addTabWidget(DocumentTabWidget *w);
			void focus(DocumentTabWidget *w);
			const std::list<DocumentTabWidget*> &tabWidgets() const;

			void addTab(QWidget *w);

			void setCurrentDocumentWidget(QWidget *w);			

			static DocumentTabWidget *parentTabWidget(QWidget *w);

		private slots:
			void removeTabWidget(QObject *obj);
			void onCurrentTabChanged(int index);
			void onTabCloseRequested(int index);

		signals:
			void tabCloseRequested(QWidget *);
			void currentChanged(QWidget *);
			

		private:
			DocumentTabWidget *mCurrentTabWidget;
			std::list<DocumentTabWidget*> mTabWidgets;

		};

	}
}
