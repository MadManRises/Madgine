#pragma once

#include "Generic/genericresult.h"

#include "Madgine/behavior.h"
#include "Madgine/behaviorcollector.h"

#include "nodegraphloader.h"

#include "Meta/keyvalue/argumentlist.h"

namespace Engine {
namespace NodeGraph {

    struct NodeDebugLocation : Debug::DebugLocation {
        NodeDebugLocation(NodeInterpreterStateBase *interpreter)
            : mInterpreter(interpreter)
        {
        }

        std::string toString() const override;
        std::map<std::string_view, ValueType> localVariables() const override;
        bool wantsPause() const override;

        const NodeBase *mNode = nullptr;
        NodeInterpreterStateBase *mInterpreter;
    };

    struct NodeInterpreterData {
        virtual ~NodeInterpreterData() = default;

        virtual bool readVar(ValueType &ref, std::string_view name) { return false; }
        virtual bool writeVar(std::string_view name, const ValueType &v) { return false; }
        virtual std::vector<std::string_view> variables() { return {}; }
    };

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpreterStateBase : Execution::VirtualReceiverBase<BehaviorError> {
        NodeInterpreterStateBase(const NodeGraph *graph, NodeGraphLoader::Handle handle);
        NodeInterpreterStateBase(const NodeInterpreterStateBase &) = delete;
        NodeInterpreterStateBase(NodeInterpreterStateBase &&) = default;
        virtual ~NodeInterpreterStateBase() = default;

        NodeInterpreterStateBase &operator=(const NodeInterpreterStateBase &) = delete;
        NodeInterpreterStateBase &operator=(NodeInterpreterStateBase &&) = default;

        void interpretImpl(Execution::VirtualReceiverBase<BehaviorError> &receiver, uint32_t flowIn);
        void interpretImpl(Execution::VirtualReceiverBase<BehaviorError> &receiver, Pin pin);

        ASYNC_STUB(interpret, interpretImpl, Execution::make_simple_virtual_sender<BehaviorError>);
        ASYNC_STUB(interpretSubGraph, branch, Execution::make_simple_virtual_sender<BehaviorError>);

        void branch(Execution::VirtualReceiverBase<BehaviorError> &receiver, Pin pin);

        void read(ValueType &retVal, Pin pin);
        void write(Pin pin, const ValueType &v);

        void read(ValueType &retVal, uint32_t dataProvider);
        void write(uint32_t dataReceiver, const ValueType &v);

        const NodeGraph *graph() const;

        const ArgumentList &arguments() const;

        std::unique_ptr<NodeInterpreterData> &data(uint32_t index);

        virtual bool readVar(ValueType &result, std::string_view name, bool recursive = true);
        virtual bool writeVar(std::string_view name, const ValueType &v);
        virtual std::vector<std::string_view> variables();

        void start();

        Debug::DebugLocation *debugLocation();

    protected:
        virtual Debug::ParentLocation *parentDebugLocation() = 0;
        virtual std::stop_token parentStopToken() = 0;

        NodeDebugLocation mDebugLocation;
    private:
        ArgumentList mArguments;

        const NodeGraph *mGraph;

        NodeGraphLoader::Handle mHandle;

        std::vector<std::unique_ptr<NodeInterpreterData>> mData;        
    };

    template <typename _Rec>
    struct NodeInterpreterState : NodeInterpreterStateBase {

        using Rec = _Rec;

        NodeInterpreterState(Rec &&rec, const NodeGraph *graph, NodeGraphLoader::Handle handle)
            : NodeInterpreterStateBase { graph, std::move(handle) }
            , mRec(std::forward<Rec>(rec))
        {
        }

        virtual void set_done() override
        {
            mDebugLocation.stepOut(parentDebugLocation());
            mRec.set_done();
        }
        virtual void set_error(BehaviorError r) override
        {
            mDebugLocation.stepOut(parentDebugLocation());
            this->mRec.set_error(std::move(r));
        }
        virtual void set_value() override
        {
            mDebugLocation.stepOut(parentDebugLocation());
            this->mRec.set_value(ArgumentList {});
        }

        bool readVar(ValueType &result, std::string_view name, bool recursive = true) override
        {
            if (NodeInterpreterStateBase::readVar(result, name, recursive))
                return true;

            if (Execution::resolve_var_d(mRec, name, result))
                return true;

            return false;
        }

        friend Rec &tag_invoke(Execution::get_receiver_t, NodeInterpreterState &state)
        {
            return state.mRec;
        }

        Debug::ParentLocation *parentDebugLocation() override
        {
            return Execution::get_debug_location(mRec);
        }

        std::stop_token parentStopToken() override
        {
            return Execution::get_stop_token(mRec);
        }

        Rec mRec;
    };

    struct NodeInterpreterSender : Execution::base_sender {

        NodeInterpreterSender(const NodeGraph *graph)
            : mGraph(graph)
        {
        }

        NodeInterpreterSender(NodeGraphLoader::Handle handle)
            : mHandle(std::move(handle))
            , mGraph(mHandle)
        {
        }

        using result_type = BehaviorError;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<ArgumentList>;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, NodeInterpreterSender &&sender, Rec &&rec)
        {
            return NodeInterpreterState<Rec> { std::forward<Rec>(rec), sender.mGraph, std::move(sender.mHandle) };
        }

        template <typename F>
        friend void tag_invoke(Execution::visit_state_t, NodeInterpreterSender &sender, F &&f)
        {
            f(Execution::State::SubLocation {});
        }

        NodeGraphLoader::Handle mHandle;
        const NodeGraph *mGraph;
    };

}
}

REGISTER_TYPE(Engine::NodeGraph::NodeInterpreterStateBase);