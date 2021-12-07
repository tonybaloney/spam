#include <Python.h>

#include <stdlib.h>


static PyObject* spam_frame(PyThreadState* ts, PyFrameObject* f, int throwflag) {
    // Do stuff...
    return _PyEval_EvalFrameDefault(ts, f, throwflag);
}

static PyInterpreterState* inter() {
    return PyInterpreterState_Main();
}

static PyObject* spam_enable(PyObject* self, PyObject* args) {
    auto prev = _PyInterpreterState_GetEvalFrameFunc(inter());
    _PyInterpreterState_SetEvalFrameFunc(inter(), spam_frame);
    if (prev == spam_frame) {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject* spam_disable(PyObject* self, PyObject* args) {
    auto prev = _PyInterpreterState_GetEvalFrameFunc(inter());
    _PyInterpreterState_SetEvalFrameFunc(inter(), _PyEval_EvalFrameDefault);
    if (prev == PyJit_EvalFrame) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

static PyMethodDef SpamMethods[] = {
        {"enable",
         spam_enable,
         METH_NOARGS,
         "Enable."},
        {"disable",
         spam_disable,
         METH_NOARGS,
         "Disable."},
    {NULL, NULL, 0, NULL},  // sentinel
};

static PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",
    "An example Python C extension module.",
    -1,
    SpamMethods,
};

PyMODINIT_FUNC PyInit_spam() {
    PyObject* module;

    module = PyModule_Create(&spammodule);
    if (module == NULL) {
        return NULL;
    }
    spam_enable();
    return module;
}

