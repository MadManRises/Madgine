#include "maditorviewlib.h"

#include "inspectorview.h"
#include "ui_inspectorview.h"

#include "model/application/inspector/inspector.h"

#include "valuetypeedit.h"

namespace Maditor {
	namespace View {


		InspectorView::InspectorView(QWidget *parent) :
			QAbstractItemView(parent),
			ui(new Ui::InspectorView),
			mInspector(nullptr),
			mLinkMapper(new QSignalMapper(this))
		{
			ui->setupUi(this);

			setViewport(ui->viewport);
			ui->itemsWidget->installEventFilter(this);

			mItemsLayout = static_cast<QGridLayout*>(ui->itemsWidget->layout());
			mTraceLayout = static_cast<QHBoxLayout*>(ui->traceWidget->layout());

			connect(mLinkMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &InspectorView::followLink);

			verticalScrollBar()->setSingleStep(10);
		}

		InspectorView::~InspectorView()
		{
			delete ui;
		}

		QRect InspectorView::visualRect(const QModelIndex & index) const
		{
			return mItemsLayout->cellRect(index.row(), index.column());
		}

		void InspectorView::scrollTo(const QModelIndex & index, ScrollHint hint)
		{
		}

		QModelIndex InspectorView::indexAt(const QPoint & point) const
		{
			QLayoutItem *item = mItemsLayout->itemAtPosition(point.x(), point.y());
			if (!item)
				return QModelIndex();
			int index = mItemsLayout->indexOf(item->widget());
			int row, rowSpan;
			int col, colSpan;
			mItemsLayout->getItemPosition(index, &row, &col, &rowSpan, &colSpan);
			return QModelIndex();
		}

		void InspectorView::followLink(int index)
		{
			if (index >= 0) {
				setIndex(mInspector->data(mInspector->index(index, 1, mCurrentRoot)).value<Engine::ValueType>().as<Engine::InvScopePtr>(), mInspector->data(mInspector->index(index, 0, mCurrentRoot)).toString());
			}
			else {
				mTrace.resize(-index);
				setIndex(mTrace.at(-index - 1).second);
			}
		}

		void InspectorView::setModel(Model::Inspector * inspector)
		{
			if (mInspector == inspector)
				return;
			clearModel();
			mInspector = inspector;
			mCurrentRoot = mInspector->registerIndex(this, nullptr);
			mDestroyConnection = connect(mInspector, &QObject::destroyed, this, &InspectorView::resetModel);
			mResetConnection = connect(mInspector, &Model::Inspector::modelReset, this, &InspectorView::reset);
			QAbstractItemView::setModel(inspector);			

			refresh();
		}

		void InspectorView::clearModel()
		{
			if (mInspector) {
				mInspector->unregisterIndex(this);
				resetModel();
			}
		}

		void InspectorView::resetModel() {
			mInspector = nullptr;
			reset();
			disconnect(mDestroyConnection);
			disconnect(mResetConnection);			
		}

		void InspectorView::addRow()
		{
			std::unique_ptr<InspectorRow> row = std::make_unique<InspectorRow>();

			row->mMapper.setModel(mInspector);
			row->mMapper.setRootIndex(mCurrentRoot);


			QLabel *label = new QLabel(this);
			label->update();
			//label->setText(mInspector->data(mInspector->index(i, 0)).toString());
			mItemsLayout->addWidget(label, mRows.size(), 0);
			row->mMapper.addMapping(label, 0, "text");

			ValueTypeEdit *edit = new ValueTypeEdit(this);
			mItemsLayout->addWidget(edit, mRows.size(), 1);
			row->mMapper.addMapping(edit, 1);
			edit->setReadOnly((mInspector->flags(mInspector->index(mRows.size(), 1)) & Qt::ItemIsEditable) == 0);
			connect(edit, &ValueTypeEdit::scopeLinkClicked, mLinkMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
			mLinkMapper->setMapping(edit, mRows.size());

			row->mMapper.setCurrentIndex(mRows.size());

			mRows.emplace_back(std::move(row));
		}

void InspectorView::refresh() {
	clearLayout(mItemsLayout);
	clearLayout(mTraceLayout);
	setRootIndex(mCurrentRoot);
	mRows.clear();
	if (mInspector) {
		for (int i = 0; i < mInspector->rowCount(mCurrentRoot); ++i) {
			addRow();
		}
	}

	int i = -1;
	for (const std::pair<QString, Engine::InvScopePtr> &trace : mTrace) {
		QPushButton *button = new QPushButton(trace.first, this);
		mTraceLayout->addWidget(button);
		connect(button, &QPushButton::clicked, mLinkMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
		mLinkMapper->setMapping(button, i);
		--i;
	}

	mTraceLayout->addStretch();

}

void InspectorView::clearLayout(QLayout * layout)
{
	QLayoutItem* item;
	while ((item = layout->takeAt(0)) != nullptr)
	{
		delete item->widget();
		delete item;
	}
}

void InspectorView::setIndex(Engine::InvScopePtr ptr, const QString &trace) {
	mCurrentRoot = mInspector->updateIndex(this, ptr);
	if (!trace.isEmpty())
		mTrace.emplace_back(trace, ptr);
	refresh();
}

QModelIndex InspectorView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	return QModelIndex();
}

int InspectorView::horizontalOffset() const
{
	return 0;
}

int InspectorView::verticalOffset() const
{
	return 0;
}

bool InspectorView::isIndexHidden(const QModelIndex & index) const
{
	return false;
}

void InspectorView::setSelection(const QRect & rect, QItemSelectionModel::SelectionFlags command)
{
}

QRegion InspectorView::visualRegionForSelection(const QItemSelection & selection) const
{
	return QRegion();
}

void InspectorView::rowsInserted(const QModelIndex & parent, int start, int end)
{
	if (parent == mCurrentRoot) {
		for (int i = start; i < mRows.size(); ++i) {
			mRows[i]->mMapper.setCurrentIndex(i);
		}

		for (int i = start; i <= end; ++i) {
			addRow();
		}
		
	}
}

void InspectorView::rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end)
{
	if (!parent.isValid()) {
		//root
	}
	else if (parent == mCurrentRoot) {
		for (int i = start; i <= end; ++i) {			
			for (int c = 0; c < mItemsLayout->columnCount(); ++c)
			{
				QLayoutItem* item = mItemsLayout->itemAtPosition(i, c);
				QWidget* itemWidget = item->widget();
				if (itemWidget)
				{
					mItemsLayout->removeWidget(itemWidget);
					delete itemWidget; 
				}
			}
		}
		mRows.erase(mRows.begin() + start, mRows.begin() + end + 1);
	}
}

void InspectorView::updateView()
{
	QSize areaSize = ui->scrollarea->size();
	QSize  widgetSize = ui->itemsWidget->size();

	verticalScrollBar()->setPageStep(areaSize.height());
	horizontalScrollBar()->setPageStep(areaSize.width());
	verticalScrollBar()->setRange(0, widgetSize.height() - areaSize.height());
	horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());

}

void InspectorView::resizeEvent(QResizeEvent * e)
{
	QAbstractItemView::resizeEvent(e);
	updateView();
}

bool InspectorView::eventFilter(QObject * o, QEvent * e)
{
	if (e->type() == QEvent::Resize) {
		updateView();
	}
	return false;
}

void InspectorView::scrollContentsBy(int, int)
{
	int hvalue = horizontalScrollBar()->value();
	int vvalue = verticalScrollBar()->value();
	QPoint topLeft = viewport()->rect().topLeft();

	ui->itemsWidget->move(topLeft.x() - hvalue, topLeft.y() - vvalue);
}

void InspectorView::reset()
{
	if (mInspector)
		mCurrentRoot = mInspector->updateIndex(this, nullptr);
	mTrace.clear();
	mTrace.emplace_back("root", nullptr);
	refresh();
	QAbstractItemView::reset();
}

	}
}
