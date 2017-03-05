#pragma once

#include "UI/handler.h"
#include "UI/windownames.h"

#include "Scripting\Types\globalapi.h"

#include "ogreuniquecomponent.h"

namespace Engine {
namespace UI {


class OGREMADGINE_EXPORT GuiHandlerBase : public Handler {
public:
    enum class WindowType{
        MODAL_OVERLAY,
        NONMODAL_OVERLAY,
        ROOT_WINDOW
    };

    GuiHandlerBase(const std::string &windowName, WindowType type, const std::string &layoutFile = "", const std::string &parentName = WindowNames::rootWindow);


    bool init(int order);
    virtual bool init() override;
	void finalize(int order);
	virtual void finalize() override;

    virtual void open();
    virtual void close();
    bool isOpen();

    bool isRootWindow();

	App::ContextMask context();

protected:
	void setInitialisationOrder(int order);
	void setContext(App::ContextMask context);



private:


    //std::map<CEGUI::Window *, std::string> mTranslationKeys;

    const std::string mLayoutFile;
	const std::string mParentName;
    const WindowType mType;


	int mOrder;
	App::ContextMask mContext;

};

template <class T>
class GuiHandler : public OgreUniqueComponent<T, GuiHandlerBase>, public Scripting::GlobalAPI<T> {
	using OgreUniqueComponent<T, GuiHandlerBase>::OgreUniqueComponent;

	virtual const char *getName() override {
		return typeid(T).name();
	}
};

} // namespace GuiHandler
} // namespace Cegui

