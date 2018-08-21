#include "maditorviewlib.h"

#include "inspectorwidget.h"

#include "../../qtimgui/QtImGui.h"
#include "../../qtimgui/imgui/imgui.h"

#include "../../../model/application/inspector/inspector.h"
#include "Madgine/generic/invscopeptr.h"
#include "inspectorlayoutdata.h"


namespace Maditor
{
	namespace View
	{
		InspectorWidget::InspectorWidget(InspectorLayoutData *data, QWidget* parent) :
		QOpenGLWindow(),
		mInspector(nullptr),
		mCurrentItem(nullptr),
		mData(data)
		{
			startTimer(0);
		}

		InspectorWidget::~InspectorWidget()
		{
			clearModel();
		}

		void InspectorWidget::setModel(Model::Inspector* inspector)
		{
			if (mInspector == inspector)
				return;
			clearModel();
			mInspector = inspector;
			mData->setModel(inspector);
			mDestroyConnection = connect(mInspector, &QObject::destroyed, this, &InspectorWidget::resetModel);
			mResetConnection = connect(mInspector, &Model::Inspector::modelReset, this, &InspectorWidget::reset);
			reset();
		}

		void InspectorWidget::setCurrentItem(Engine::InvScopePtr ptr)
		{
			mCurrentItem = mData->updateIndex(mCurrentItem->ptr(), ptr);
		}


		void InspectorWidget::clearModel()
		{
			if (mInspector) {
				mData->unregisterIndex(mCurrentItem->ptr());
				resetModel();
			}
		}

		void InspectorWidget::resetModel() {
			mInspector = nullptr;
			reset();
			disconnect(mDestroyConnection);
			disconnect(mResetConnection);
		}

		void InspectorWidget::reset()
		{
			if (mInspector) {
				mCurrentItem = mData->registerIndex(nullptr);
			}
			mTrace.clear();
			mTrace.emplace_back("root", nullptr);						
		}


		void InspectorWidget::drawContext()
		{
			for (const std::pair<const QString, Engine::InvScopePtr> &t : mTrace)
			{
				if (ImGui::Button(t.first.toStdString().c_str()))
				{
					setCurrentItem(t.second);
					return;
				}
			}
			ImGui::Separator();
			mData->drawScope(mCurrentItem);
		}





		void InspectorWidget::initializeGL()
		{
			QtImGui::initialize(this);
			initializeOpenGLFunctions();
		}

		void InspectorWidget::paintGL()
		{
			// you can do custom GL rendering as well in paintGL

			QtImGui::newFrame();

			ImGui::SetNextWindowSize(ImVec2(width(), height()), ImGuiSetCond_Always);
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
			ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

			if (mCurrentItem)
				drawContext();
			
			ImGui::End();

			// Do render before ImGui UI is rendered
			glViewport(0, 0, width(), height());
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui::Render();
		}

		void InspectorWidget::timerEvent(QTimerEvent* event)
		{
			update();
		}
	}
}
