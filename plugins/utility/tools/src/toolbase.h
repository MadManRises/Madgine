#pragma once

#include "Modules/reflection/classname.h"

#include "Madgine/core/madgineobject.h"

#include "Modules/keyvalue/scopebase.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Tools {

    class MADGINE_TOOLS_EXPORT ToolBase : public Core::MadgineObject, public ScopeBase {
    public:
        ToolBase(ImGuiRoot &root);
        virtual ~ToolBase() = default;

        virtual void render();
        virtual void renderMenu();
        virtual void update();

        virtual const char *key() const = 0;

        bool isVisible();
        void setVisible(bool v);

		ToolBase &getToolComponent(size_t index, bool = true);
        template <typename T>
        T &getTool()
        {
            return static_cast<T &>(getToolComponent(component_index<T>()));
        }
        ToolBase &getSelf(bool = true);

    protected:
        ImGuiRoot &root();

        virtual const MadgineObject *parent() const override;
        virtual App::Application &app(bool = true) override;
        virtual bool init() override;
        virtual void finalize() override;

    private:
        ImGuiRoot &mRoot;
        bool mVisible;
    };

}
}