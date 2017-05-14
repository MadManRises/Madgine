#pragma once

#include "handler.h"
#include "GUI\windownames.h"

#include "ogreuniquecomponent.h"

#include "Scripting\Types\scope.h"

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

	Scene::ContextMask context();

protected:
	void setInitialisationOrder(int order);
	void setContext(Scene::ContextMask context);



private:


    //std::map<CEGUI::Window *, std::string> mTranslationKeys;

    const std::string mLayoutFile;
	const std::string mParentName;
    const WindowType mType;


	int mOrder;
	Scene::ContextMask mContext;

};

template <class T>
using GuiHandler = Scripting::Scope<T, OgreUniqueComponent<T, GuiHandlerBase>>;

} // namespace GuiHandler
} // namespace Cegui

