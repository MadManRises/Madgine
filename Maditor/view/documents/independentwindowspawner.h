#pragma once

#include "mainwindow_ui.h"

namespace Maditor {
	namespace View {


		template <class Model, class View>
		class IndependentWindowSpawner
		{
		public:

			IndependentWindowSpawner() :
				mTabWidget(nullptr)
			{
			}

			void setupUi(const Ui::MainWindow * ui) {
				mTabWidget = ui->tabWidget;
				QObject::connect(ui->tabWidget, &QTabWidget::tabCloseRequested, std::bind(&IndependentWindowSpawner<Model, View>::onTabCloseRequest, this, std::placeholders::_1));
			}

			template <class... _Ty>
			void spawn(Model *model, _Ty&&... args) {
				assert(mTabWidget);
				auto it = mViews.find(model);
				if (it == mViews.end()) {
					View *win = new View(model, std::forward<_Ty>(args)...);
					mViews[model] = win;
					win->updateTitle();
					mTabWidget->setCurrentIndex(mTabWidget->addTab(win, win->windowTitle()));
					QObject::connect(win, &View::windowTitleChanged, [=](const QString &title) {mTabWidget->setTabText(mTabWidget->indexOf(win), title); });
				}
				else {
					mTabWidget->setCurrentWidget(it->second);
				}
			}

		protected:

			void onTabCloseRequest(int index) {
				if (View *w = dynamic_cast<View*>(mTabWidget->widget(index))) {
					auto it = std::find_if(mViews.begin(), mViews.end(), [=](const std::pair<Model * const, View *> &p) {return p.second == w; });
					if (it != mViews.end()) {
						if (w->requestClose(false) != QMessageBox::Abort) {
							w->deleteLater();
							mViews.erase(it);
						}
					}
				}
			}


		private:
			QTabWidget *mTabWidget;

			std::map<Model*, View*> mViews;

		};


	} // namespace View
} // namespace Maditor
