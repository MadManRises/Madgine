#pragma once

#include "../toolbase.h"
#include "../toolscollector.h"

#include "Meta/keyvalue/valuetype.h"

namespace Engine {
namespace Tools {

    struct TestTool : Tool<TestTool> {

        TestTool(ImRoot &root);

        virtual void render() override;
        virtual void update() override;

        std::string_view key() const override;

        void logTest();
        void logValue(const ValueType &v);
        int dummy();

        ValueType mV;

    private:
        bool mRenderSphere = false;
        int mSphereDetail = 1;

        std::string m3DText = "Test_1234 Hello World!";
    };

}
}

REGISTER_TYPE(Engine::Tools::TestTool)