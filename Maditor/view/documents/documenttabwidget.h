

namespace Maditor {
	namespace View {

		class DocumentTabWidget : public QTabWidget {
		public:
			DocumentTabWidget(CentralWidget *centralWidget, QWidget *parent = nullptr);

			void focus();

		protected:
			virtual void dragMoveEvent(QDragMoveEvent *event) override;
			virtual void dragEnterEvent(QDragEnterEvent *event) override;
			virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
			virtual void dropEvent(QDropEvent *event) override;

			virtual void mousePressEvent(QMouseEvent *event) override;

			virtual void tabRemoved(int index) override;

			static void removeWidget(QWidget *w);
			static void replaceSplitter(QSplitter *splitter, QWidget *w);


		private slots:
			virtual void tabInserted(int index) override;

		private:
			CentralWidget *mCentralWidget;
		};

	}
}
