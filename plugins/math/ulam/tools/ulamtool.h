#pragma once
#include "toolscollector.h"
#include "toolbase.h"

//#include "inspectorlayout.h"

#include "ulam.h"

namespace Engine {
namespace Tools {

    struct MADGINE_ULAMTOOLS_EXPORT UlamTool : Tool<UlamTool> {
        UlamTool(ImRoot &root);
        UlamTool(const UlamTool &) = delete;        

        virtual void render() override;

        virtual std::string_view key() const override;

        Ulam mUlam = { 1 };        
        const char *mMode = "text";
        void (*mFunction)(Ulam &, int &);

        int mHoveredLine = -1;

    };
}
}

RegisterType(Engine::Tools::UlamTool);