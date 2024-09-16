#include "../toolslib.h"

#include "dialogs.h"

#include "imgui/imgui.h"

namespace Engine {
namespace Tools {

    void DialogContainer::push(DialogStateBase *dialog)
    {
        dialog->mNext = mHead;
        mHead = dialog;
    }

    void DialogContainer::pop(DialogStateBase *dialog)
    {
        assert(mHead == dialog);
        mHead = dialog->mNext;
        dialog->mNext = nullptr;
    }

    void DialogContainer::render()
    {
        DialogStateBase *dialog = mHead;
        while (dialog) {
            DialogStateBase *next = dialog->mNext;
            dialog->render();
            dialog = next;
        }
    }

    DialogStateBase::DialogStateBase(DialogSettings settings, DialogContainer *targetContainer)
        : mSettings(std::move(settings))
        , mTargetContainer(targetContainer)
    {
    }

    void DialogStateBase::start()
    {
        mTargetContainer->push(this);
    }

    void DialogStateBase::render()
    {
        std::string header = mSettings.header.empty() ? " " : mSettings.header;

        ImGui::PushID(this);
        ImGui::OpenPopup(header.c_str());

        ImGui::SetNextWindowSize({ 500, 400 }, ImGuiCond_FirstUseEver);
        if (ImGui::BeginPopupModal(header.c_str())) {

            ImGui::BeginVertical("Main");

            DialogFlags flags = renderContent();

            ImGui::BeginHorizontal("Buttons");

            ImGui::Spring();

            bool cancelled = false;
            std::optional<DialogResult> result;

            if (flags.implicitlyAccepted)
                result = DialogResult::Accepted;

            if (mSettings.showAccept) {
                if (!flags.acceptPossible)
                    ImGui::BeginDisabled();
                if (ImGui::Button(mSettings.acceptText.c_str())) {
                    result = DialogResult::Accepted;
                }
                if (!flags.acceptPossible)
                    ImGui::EndDisabled();
            }

            if (mSettings.showDecline) {
                if (ImGui::Button(mSettings.declineText.c_str())) {
                    result = DialogResult::Declined;
                }
            }

            if (mSettings.showCancel) {
                if (ImGui::Button(mSettings.cancelText.c_str())) {
                    cancelled = true;
                }
            }

            if (result) {
                close(*result);
            }
            if (cancelled) {
                cancel();
            }

            ImGui::EndHorizontal();
            ImGui::EndVertical();

            ImGui::EndPopup();
        }
        ImGui::PopID();
    }

    void DialogStateBase::close(DialogResult)
    {
        mTargetContainer->pop(this);
    }

    void DialogStateBase::cancel()
    {
        mTargetContainer->pop(this);
    }

}
}
