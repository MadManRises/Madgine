

namespace Maditor {
	namespace View {

		class DocumentTabBar : public QTabBar {

		public:
			DocumentTabBar(QWidget *parent = nullptr);

		private:
			virtual void mousePressEvent(QMouseEvent * event) override;
			virtual void mouseReleaseEvent(QMouseEvent * event) override;
			virtual void mouseMoveEvent(QMouseEvent * event) override;

			virtual void dragMoveEvent(QDragMoveEvent *event) override;
			virtual void dragEnterEvent(QDragEnterEvent *event) override;
			virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

			int mPressedIndex;
			QPoint mDragStartPosition;
		

		};

	}
}
