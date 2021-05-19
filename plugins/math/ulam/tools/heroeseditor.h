#pragma once
#include "toolscollector.h"
#include "toolbase.h"

//#include "inspectorlayout.h"

#include "heroes2map.h"

namespace Engine {
namespace Tools {

    struct MADGINE_ULAMTOOLS_EXPORT HeroesEditor : Tool<HeroesEditor> {
        HeroesEditor(ImRoot &root);
        HeroesEditor(const HeroesEditor &) = delete;        

        virtual void render() override;
        virtual void renderMenu() override;

        void draw(const char *name, std::vector<unsigned char> &buffer);

        virtual std::string_view key() const override;

        Heroes2Map mMap;

        int mOffset = 428;
        int mStride = 20;
        int mLimitx = 109;
        int mLimity = 109;
        int mModifyX;
        int mModifyY;
        static constexpr size_t sMapSize = 107;

    };
}
}

RegisterType(Engine::Tools::HeroesEditor);