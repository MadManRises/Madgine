#pragma once

#include "UI/handler.h"
#include "UI/windownames.h"
#include "uniquecomponent.h"
#include "App\contextmasks.h"
#include "Scripting\Types\globalapi.h"

namespace Engine {
namespace UI {


class MADGINE_EXPORT GuiHandlerBase : public Handler {
public:
    enum class WindowType{
        MODAL_OVERLAY,
        NONMODAL_OVERLAY,
        ROOT_WINDOW
    };

    GuiHandlerBase(const std::string &windowName, WindowType type, const std::string &layoutFile = "", const std::string &parentName = WindowNames::rootWindow);

	virtual void sizeChanged();

    void init(int order);
    virtual void init() override;

    virtual void open();
    virtual void close();
    bool isOpen();

    bool isRootWindow();

	App::ContextMask context();

protected:
	void setInitialisationOrder(int order);
	void setContext(App::ContextMask context);


protected:


    template <class T, class... _Ty>
    void registerButton(const std::string &name,
                        void (T::*f)(_Ty...), _Ty... args)
    {
        registerWindow({name, {
                {GUI::EventType::ButtonClick, event(f, args...)}
            }
        });
    }

    template <class T, class... _Ty>
    void registerButton(const std::string &name, Engine::GUI::Button **var,
                        void (T::*f)(_Ty...), std::remove_const_t<_Ty>... args)
    {
        registerWindow({name, var, {
                { GUI::EventType::ButtonClick, event(f, args...)}
            }
        });
    }

private:


    //std::map<CEGUI::Window *, std::string> mTranslationKeys;

    const std::string mLayoutFile;
	const std::string mParentName;
    const WindowType mType;

	bool mInitialized;
	int mOrder;
	App::ContextMask mContext;

};

template <class T>
class GuiHandler : public UniqueComponent<T, GuiHandlerBase>, public Scripting::GlobalAPI<T> {
	using UniqueComponent::UniqueComponent;
};

} // namespace GuiHandler
} // namespace Cegui

