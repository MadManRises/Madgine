
typedef struct _frame PyFrameObject;



#if PY_MINOR_VERSION < 11
typedef PyFrameObject _PyInterpreterFrame;
#endif

int PyFrame_StackSize(_PyInterpreterFrame *frame);
PyObject *PyFrame_StackPeek(_PyInterpreterFrame *frame);
PyObject *PyFrame_StackPop(_PyInterpreterFrame *frame);
void PyFrame_StackPush(_PyInterpreterFrame *frame, PyObject *object);
MADGINE_PYTHON3_EXPORT PyCodeObject *PyFrame_GetCode2(_PyInterpreterFrame *frame);