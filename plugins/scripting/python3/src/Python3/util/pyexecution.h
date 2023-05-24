#pragma once

#include "Generic/genericresult.h"

#include "Generic/execution/concepts.h"

#include "pyframeptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct PySenderBase {
            virtual ~PySenderBase() = default;

            void connectAndStart(KeyValueReceiver &receiver, std::streambuf *out, std::vector<PyFramePtr> outerFrames);
            void addFrame(PyFramePtr frame);

        private:
            virtual void start() = 0;

            PyObject *getSelf();

            friend struct PySenderReceiver;

            std::vector<PyFramePtr> mFrames;
            KeyValueReceiver *mReceiver = nullptr;
            std::streambuf *mOut;
        };

        struct PySenderReceiver {

            void set_value();
            void set_value(const ArgumentList &args);
            void set_error(GenericResult);
            void set_done();

            PySenderBase &mSender;
        };

        template <typename Sender>
        struct PySender : PySenderBase {
            PySender(Sender &&sender)
                : mState(Execution::connect(std::forward<Sender>(sender), PySenderReceiver { *this }))
            {
            }

            virtual void start() override
            {
                mState.start();
            }

        private:
            Execution::connect_result_t<Sender, PySenderReceiver> mState;
        };

        struct PySenderObject {
            PyObject_VAR_HEAD char mSender[sizeof(PySenderBase)];
        };

        template <typename Sender>
        struct PySenderObjectImpl {
            PyObject_VAR_HEAD
                PySender<Sender>
                    mSender;
        };

        extern PyTypeObject PySenderType;

        template <typename Sender>
        PyObject *toPySender(Sender &&sender)
        {
            PySenderObjectImpl<Sender> *state = PyObject_NewVar(PySenderObjectImpl<Sender>, &PySenderType, sizeof(PySender<Sender>));

            new (&state->mSender) PySender<Sender>(std::forward<Sender>(sender));

            return reinterpret_cast<PyObject *>(state);
        }
        PySenderBase &fromPySender(PyObject *sender);

        void setupExecution();

        bool stackUnwindable();

        PyObject *evalFrame(PyFrameObject *frame, int throwExc);
        void evalFrame(KeyValueReceiver &receiver, PyFramePtr frame);
        void evalFrames(KeyValueReceiver &receiver, std::vector<PyFramePtr> frames);

        PyObject *suspend(PyObject *sender);
        PyObject *suspend(auto &&sender)
        {
            return suspend(toPySender(std::forward<decltype(sender)>(sender)));
        }

    }
}
}