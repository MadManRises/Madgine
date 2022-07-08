#include "../../../python3nodeslib.h"

#include "python3functionnode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalueutil/valuetypeserialize.h"

#include "Madgine/nodegraph/nodeinterpreter.h"

#include "Python3/python3env.h"

#include "Python3/util/pyobjectutil.h"

#include "Meta/keyvalue/valuetype.h"

#include "Python3/util/python3lock.h"

#include <iostream>

NODE(Python3FunctionNode, Engine::Scripting::Python3::Python3FunctionNode)

METATABLE_BEGIN_BASE(Engine::Scripting::Python3::Python3FunctionNode, Engine::NodeGraph::NodeBase)
PROPERTY(File, getFile, setFile)
METATABLE_END(Engine::Scripting::Python3::Python3FunctionNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Scripting::Python3::Python3FunctionNode, Engine::NodeGraph::NodeBase)
ENCAPSULATED_FIELD(File, getName, setName)
SERIALIZETABLE_END(Engine::Scripting::Python3::Python3FunctionNode)

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3FunctionNode::Python3FunctionNode(NodeGraph::NodeGraph &graph)
            : Node(graph)
            , mReturnType(toValueTypeDesc<std::monostate>())
        {
            setup();
        }

        Python3FunctionNode::Python3FunctionNode(const Python3FunctionNode &other, NodeGraph::NodeGraph &graph)
            : Node(other, graph)
            , mFile(other.mFile)
            , mArguments(other.mArguments)
            , mReturnType(other.mReturnType)
            , mName(other.mName)
        {
        }

        std::string_view Python3FunctionNode::name() const
        {
            return mName;
        }

        size_t Python3FunctionNode::flowInCount() const
        {
            return 1;
        }

        std::string_view Python3FunctionNode::flowInName(uint32_t index) const
        {
            return "execute";
        }

        size_t Python3FunctionNode::flowOutCount() const
        {
            return 1;
        }

        std::string_view Python3FunctionNode::flowOutName(uint32_t index) const
        {
            return "return";
        }

        size_t Python3FunctionNode::dataOutCount() const
        {
            return 1;
        }

        std::string_view Python3FunctionNode::dataOutName(uint32_t index) const
        {
            return "out";
        }

        ExtendedValueTypeDesc Python3FunctionNode::dataOutType(uint32_t index, bool bidir) const
        {
            return mReturnType;
        }

        size_t Python3FunctionNode::dataInCount() const
        {
            return mArguments.size();
        }

        std::string_view Python3FunctionNode::dataInName(uint32_t index) const
        {
            return mArguments[index].first;
        }

        ExtendedValueTypeDesc Python3FunctionNode::dataInType(uint32_t index, bool bidir) const
        {
            return mArguments[index].second;
        }

        void Python3FunctionNode::interpret(NodeGraph::NodeInterpreter &interpreter, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const
        {
            ValueType retVal;
            {
                Python3Lock lock { std::cout.rdbuf() };
                PyObjectPtr args = PyTuple_New(mArguments.size());
                for (size_t i = 0; i < mArguments.size(); ++i) {
                    ValueType v;
                    interpreter.read(v, i);
                    PyTuple_SET_ITEM(static_cast<PyObject *>(args), i, toPyObject(v));
                }
                fromPyObject(retVal, mFile->get("__call__").call(args, PyObjectPtr{}));
            }

            interpreter.write(0, retVal);
            flowInOut = 0;
        }

        Python3FileLoader::ResourceType *Python3FunctionNode::getFile() const
        {
            return mFile.resource();
        }

        void Python3FunctionNode::setFile(Python3FileLoader::ResourceType *file)
        {
            mFile = file;
            updatePins();
        }

        std::string_view Python3FunctionNode::getName() const
        {
            return mFile.name();
        }

        void Python3FunctionNode::setName(std::string_view name)
        {
            mFile.load(name);
            updatePins();
        }

        void Python3FunctionNode::updatePins()
        {
            mArguments.clear();

            Python3FileLoader::HandleType signature = Python3FileLoader::load("signature");

            mName = std::string { mFile.name() } + " [Python3]";

            {
                Python3Lock lock { std::cout.rdbuf() };
                PyObjectPtr fn = mFile->get("__call__");
                if (fn) {
                    PyObjectPtr spec = signature->call("get", "(O)", (PyObject *)fn);

                    PyObject *key = NULL, *value = NULL;
                    Py_ssize_t pos = 0;

                    const char *argName;

                    while (PyDict_Next(PyTuple_GetItem(spec, 0), &pos, &key, &value)) {
                        if (!PyArg_Parse(key, "s", &argName))
                            throw 0;

                        mArguments.emplace_back(argName, PyToValueTypeDesc(value));
                    }

                    mReturnType = PyToValueTypeDesc(PyTuple_GetItem(spec, 1));
                }
            }
            setup();
        }

    }
}
}
