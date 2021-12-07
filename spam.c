#include <Python.h>

#include <stdlib.h>


static PyObject* spam_frame(PyThreadState* ts, PyFrameObject* f, int throwflag) {
    // Do stuff...
    return _PyEval_EvalFrameDefault(ts, f, throwflag);
}

static PyObject* spam_system(PyObject* self, PyObject* args) {
    const char* command;
    int status;

    if (!PyArg_ParseTuple(args, "s", &command)) {
        return NULL;
    }
    status = system(command);
    if (!WIFEXITED(status)) {
        return Py_BuildValue("i", -1);
    }
    return Py_BuildValue("i", WEXITSTATUS(status));
}

static PyObject* spam_check_system(PyObject* self, PyObject* args) {
    const char* command;
    int status;
    int exit_status;

    if (!PyArg_ParseTuple(args, "s", &command)) {
        return NULL;
    }
    status = system(command);
    if (!WIFEXITED(status)) {
        return PyErr_Format(SpamError, "Command could not be executed");
    }
    exit_status = WEXITSTATUS(status);
    if (exit_status != 0) {
        return PyErr_Format(
            SpamError, "Command returned non-zero exit status %d", exit_status);
    }
    Py_RETURN_NONE;
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

#if PY_MAJOR_VERSION >= 3
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
    return module;
}
#else
PyMODINIT_FUNC initspam() {
    PyObject* module;

    module = Py_InitModule3(
        "spam", SpamMethods, "An example Python C extension module.");
    if (module == NULL) {
        return;
    }
}
#endif
