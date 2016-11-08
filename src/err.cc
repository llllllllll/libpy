#include "libpy/err.h"

namespace {
namespace e = py::err;
}

const e::exctype e::BaseException(PyExc_BaseException);
const e::exctype e::Exception(PyExc_Exception);
const e::exctype e::ArithmeticError(PyExc_ArithmeticError);
const e::exctype e::LookupError(PyExc_LookupError);
const e::exctype e::AssertionError(PyExc_AssertionError);
const e::exctype e::AttributeError(PyExc_AttributeError);
const e::exctype e::BlockingIOError(PyExc_BlockingIOError);
const e::exctype e::BrokenPipeError(PyExc_BrokenPipeError);
const e::exctype e::ChildProcessError(PyExc_ChildProcessError);
const e::exctype e::ConnectionError(PyExc_ConnectionError);
const e::exctype e::ConnectionAbortedError(PyExc_ConnectionAbortedError);
const e::exctype e::ConnectionRefusedError(PyExc_ConnectionRefusedError);
const e::exctype e::ConnectionResetError(PyExc_ConnectionResetError);
const e::exctype e::FileExistsError(PyExc_FileExistsError);
const e::exctype e::FileNotFoundError(PyExc_FileNotFoundError);
const e::exctype e::EOFError(PyExc_EOFError);
const e::exctype e::FloatingPointError(PyExc_FloatingPointError);
const e::exctype e::ImportError(PyExc_ImportError);
#if LIBPY_HAVE_MODULE_NOT_FOUND_ERROR
const e::exctype e::ModuleNotFoundError(PyExc_ModuleNotFoundError);
#endif
const e::exctype e::IndexError(PyExc_IndexError);
#if LIBPY_HAVE_INTERRUPTED_ERROR
const e::exctype e::InterruptedError(PyExc_InterruptedError);
#endif
const e::exctype e::IsADirectoryError(PyExc_IsADirectoryError);
const e::exctype e::KeyError(PyExc_KeyError);
const e::exctype e::KeyboardInterrupt(PyExc_KeyboardInterrupt);
const e::exctype e::MemoryError(PyExc_MemoryError);
const e::exctype e::NameError(PyExc_NameError);
const e::exctype e::NotADirectoryError(PyExc_NotADirectoryError);
const e::exctype e::NotImplementedError(PyExc_NotImplementedError);
const e::exctype e::OSError(PyExc_OSError);
const e::exctype e::OverflowError(PyExc_OverflowError);
const e::exctype e::PermissionError(PyExc_PermissionError);
const e::exctype e::ProcessLookupError(PyExc_ProcessLookupError);
#if LIBPY_HAVE_RECURSION_ERROR
const e::exctype e::RecursionError(PyExc_RecursionError);
#endif
const e::exctype e::ReferenceError(PyExc_ReferenceError);
const e::exctype e::RuntimeError(PyExc_RuntimeError);
const e::exctype e::SyntaxError(PyExc_SyntaxError);
const e::exctype e::SystemError(PyExc_SystemError);
const e::exctype e::TimeoutError(PyExc_TimeoutError);
const e::exctype e::SystemExit(PyExc_SystemExit);
const e::exctype e::TypeError(PyExc_TypeError);
const e::exctype e::ValueError(PyExc_ValueError);
const e::exctype e::ZeroDivisionError(PyExc_ZeroDivisionError);
#if MS_WINDOWS
const e::exctype e::WindowsError(PyExc_WindowsError);
#endif

// OSError aliases
const py::err::exctype EnvironmentError(PyExc_EnvironmentError);
const py::err::exctype IOError(PyExc_IOError);

py::err::object::object() : py::object() {}

py::err::object::object(PyObject *pob) : py::object(pob) {
    exception_check();
}

py::err::object::object(const py::object &pob) : py::object(pob) {
    exception_check();
}

py::err::object::object(const py::err::object &cpfrom) :
    py::object(cpfrom.ob) {}

py::err::object::object(py::err::object &&mvfrom) noexcept :
    py::object(mvfrom.ob) {
    mvfrom.ob = nullptr;
}

void py::err::object::exception_check() {
    if (ob && !PyExceptionClass_Check(Py_TYPE(ob))) {
        ob = nullptr;
        if (!PyErr_Occurred()) {
            PyErr_SetString(TypeError,
                            "cannot make py::err::object from non exception");
        }
    }
}

py::tmpref<py::object> py::err::object::traceback() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return PyException_GetTraceback(ob);
}

int py::err::object::traceback(py::object tb) {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyException_SetTraceback(ob, tb);
}

py::tmpref<py::object> py::err::object::context() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return PyException_GetContext(ob);
}

void py::err::object::context(py::object tb) {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return;
    }
    PyException_SetContext(ob, tb);
}

py::tmpref<py::object> py::err::object::cause() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return PyException_GetCause(ob);
}

void py::err::object::cause(py::object cs) {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return;
    }
    PyException_SetCause(ob, cs);
}

py::err::msgbuilder::msgbuilder(py::err::exctype type) :
    type(type), fire(true) {}
py::err::msgbuilder::msgbuilder(msgbuilder &&mvfrom) :
    std::stringstream(std::forward<msgbuilder>(mvfrom)), type(mvfrom.type) {
    mvfrom.fire = false;
}
py::err::msgbuilder::~msgbuilder() {
    if (fire) {
        PyErr_SetString(type, str().c_str());
    }
}

py::err::msgbuilder py::err::raise(py::err::exctype type) {
    return msgbuilder(type);
}

void py::err::raise(py::err::object err) {
    PyErr_SetObject(err.type(), err);
}
