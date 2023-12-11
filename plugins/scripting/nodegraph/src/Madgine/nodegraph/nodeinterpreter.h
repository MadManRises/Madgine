#pragma once

#include "Generic/genericresult.h"

#include "Meta/keyvalue/keyvaluereceiver.h"

#include "Madgine/behavior.h"
#include "Madgine/behaviorcollector.h"

namespace Engine {
namespace NodeGraph {

    struct NodeInterpreterData {
        virtual ~NodeInterpreterData() = default;

        virtual bool readVar(ValueType &ref, std::string_view name) { return false; }
        virtual bool writeVar(std::string_view name, const ValueType &v) { return false; }
        virtual std::vector<std::string_view> variables() { return {}; }
    };

    struct MADGINE_NODEGRAPH_EXPORT NodeInterpreterStateBase : Execution::VirtualReceiverBase<InterpretResult> {
        NodeInterpreterStateBase(const NodeGraph *graph);
        NodeInterpreterStateBase(const NodeInterpreterStateBase &) = delete;
        NodeInterpreterStateBase(NodeInterpreterStateBase &&) = default;
        virtual ~NodeInterpreterStateBase() = default;

        NodeInterpreterStateBase &operator=(const NodeInterpreterStateBase &) = delete;
        NodeInterpreterStateBase &operator=(NodeInterpreterStateBase &&) = default;

        void interpretImpl(Execution::VirtualReceiverBase<InterpretResult> &receiver, uint32_t flowIn);
        void interpretImpl(Execution::VirtualReceiverBase<InterpretResult> &receiver, Pin pin);

        ASYNC_STUB(interpret, interpretImpl, Execution::make_simple_virtual_sender<InterpretResult>);
        ASYNC_STUB(interpretSubGraph, branch, Execution::make_simple_virtual_sender<InterpretResult>);

        void branch(Execution::VirtualReceiverBase<InterpretResult> &receiver, Pin pin);

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

        void start(ArgumentList args);

    private:
        ArgumentList mArguments;

        const NodeGraph *mGraph;

        std::vector<std::unique_ptr<NodeInterpreterData>> mData;
    };

    struct NodeInterpreterReceiver {

        void set_value(ArgumentList arguments)
        {
            mState->start(std::move(arguments));
        }

        void set_done()
        {
            mState->set_done();
        }
        void set_error(InterpretResult r)
        {
            mState->set_error(r);
        }

        NodeInterpreterStateBase *mState;
    };

    template <Execution::Sender Inner, typename _Rec>
    struct NodeInterpreterState : NodeInterpreterStateBase {

        using Rec = _Rec;

        NodeInterpreterState(Inner &&inner, Rec &&rec, const NodeGraph *graph)
            : NodeInterpreterStateBase { graph }
            , mState(Execution::connect(std::forward<Inner>(inner), NodeInterpreterReceiver { this }))
            , mRec(std::forward<Rec>(rec))
        {
        }

        void start() {
            mState.start();
        }

        virtual void set_done() override
        {
            mRec.set_done();
        }
        virtual void set_error(InterpretResult r) override
        {
            this->mRec.set_error(r);
        }
        virtual void set_value() override
        {
            this->mRec.set_value(ArgumentList{});
        }

        bool readVar(ValueType &result, std::string_view name, bool recursive = true) override
        {
            if (NodeInterpreterStateBase::readVar(result, name, recursive))
                return true;

            if (Execution::resolve_var_d(mRec, name, result))
                return true;

            return false;
        }

        template <typename F>
        friend void tag_invoke(Execution::visit_state_t, NodeInterpreterState &state, F &&f)
        {
            throw 0;
        }

        friend std::string tag_invoke(get_behavior_name_t, const NodeInterpreterState &state)
        {
            throw 0;
        }

        Execution::connect_result_t<Inner, NodeInterpreterReceiver> mState;
        Rec mRec;
    };

    template <Execution::Sender Inner>
    struct NodeInterpreterSender : Execution::algorithm_sender<Inner> {
        using result_type = InterpretResult;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<ArgumentList>;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, NodeInterpreterSender &&sender, Rec &&rec)
        {
            return NodeInterpreterState<Inner, Rec> { std::forward<Inner>(sender.mInner), std::forward<Rec>(rec), sender.mGraph };
        }

        Inner mInner;
        const NodeGraph *mGraph;
    };

    struct NodeInterpreter {

        template <Execution::Sender Inner>
        auto operator()(Inner &&inner)
        {
            return NodeInterpreterSender<Inner> { {}, std::forward<Inner>(inner), mGraph };
        }

        template <Execution::Sender Inner>
        friend auto operator|(Inner &&inner, NodeInterpreter &&interpreter)
        {
            return NodeInterpreterSender<Inner> { {}, std::forward<Inner>(inner), interpreter.mGraph };
        }

        const NodeGraph *mGraph;
    };

}
}

REGISTER_TYPE(Engine::NodeGraph::NodeInterpreterStateBase);