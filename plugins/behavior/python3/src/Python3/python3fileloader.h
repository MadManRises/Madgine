#pragma once

#include "Madgine/resources/resourceloader.h"

#include "util/pymoduleptr.h"

#include "Meta/keyvalue/objectptr.h"

#include "Meta/keyvalue/functiontable.h"

#include "Madgine/behaviorcollector.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT Python3FileLoader : Resources::ResourceLoader<Python3FileLoader, PyModulePtr, std::list<Placeholder<0>>> {

            using Base = Resources::ResourceLoader<Python3FileLoader, PyModulePtr, std::list<Placeholder<0>>>;

            Python3FileLoader();

            void setup();
            void cleanup();

            Threading::Task<bool> loadImpl(PyModulePtr &module, ResourceDataInfo &info, Filesystem::FileEventType event);
            void unloadImpl(PyModulePtr &module);

            void find_spec(ValueType &result, std::string_view name, std::optional<std::string_view> import_path, ObjectPtr target_module);

            void create_module(ValueType &result, ObjectPtr spec);
            void exec_module(ValueType &result, ObjectPtr module);

        private:
            struct Python3FunctionTable : FunctionTable{
                Python3FunctionTable(PyObjectPtr fn);
                ~Python3FunctionTable();

                std::vector<FunctionArgument> mArgumentsHolder;
                std::vector<std::string> mArgumentsNames;
                std::string mNameHolder;

                PyObjectPtr mFunctionObject;
            };
            std::list<Python3FunctionTable> mTables;
        };

        struct Python3BehaviorFactory : BehaviorFactory<Python3BehaviorFactory> {
            std::vector<std::string_view> names() const override;
            Behavior create(std::string_view name, const ParameterTuple &args) const override;
            Threading::TaskFuture<ParameterTuple> createParameters(std::string_view name) const override;
            bool isConstant(std::string_view name) const override;
            std::vector<ValueTypeDesc> parameterTypes(std::string_view name) const override;
            std::vector<ValueTypeDesc> resultTypes(std::string_view name) const override;   
        };

    }
}
}

DECLARE_BEHAVIOR_FACTORY(Engine::Scripting::Python3::Python3BehaviorFactory)


REGISTER_TYPE(Engine::Scripting::Python3::Python3FileLoader)