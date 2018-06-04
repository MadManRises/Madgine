#ifndef INSPECTORVIEW_H
#define INSPECTORVIEW_H



namespace Maditor {
	namespace View {

		namespace Ui {
			class InspectorView;
		}

		struct InspectorRow {
			QDataWidgetMapper mMapper;
		};

class InspectorView : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit InspectorView(QWidget *parent = 0);
    ~InspectorView();

public:
	virtual QRect visualRect(const QModelIndex &index) const;
	virtual void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
	virtual QModelIndex indexAt(const QPoint &point) const;

	void setModel(Model::Inspector *inspector);
	void clearModel();


private slots:
	void setIndex(Engine::InvScopePtr ptr, const QString &trace = "");
	void followLink(int index);

	virtual void reset() override;


private:
	using QAbstractItemView::setModel;

	void clearLayout(QLayout *layout);
	void resetModel();

protected:
	virtual QModelIndex moveCursor(CursorAction cursorAction,
		Qt::KeyboardModifiers modifiers);

	virtual int horizontalOffset() const;
	virtual int verticalOffset() const;

	virtual bool isIndexHidden(const QModelIndex &index) const;

	virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
	virtual QRegion visualRegionForSelection(const QItemSelection &selection) const;

	virtual void rowsInserted(const QModelIndex &parent, int start, int end) override;
	virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) override;

	virtual void resizeEvent(QResizeEvent *e) override;

	virtual bool eventFilter(QObject *o, QEvent *e) override;

	virtual void scrollContentsBy(int, int) override;

private:
	void addRow();
	void refresh();
	void updateView();

private:
    Ui::InspectorView *ui;

	std::vector<std::unique_ptr<InspectorRow>> mRows;

	Model::Inspector *mInspector;

	QModelIndex mCurrentRoot;

	std::vector<std::pair<QString, Engine::InvScopePtr>> mTrace;

	QSignalMapper *mLinkMapper;

	QGridLayout *mItemsLayout;
	QHBoxLayout *mTraceLayout;

	QMetaObject::Connection mDestroyConnection, mResetConnection;

};


	}
}

#endif // INSPECTORVIEW_H
