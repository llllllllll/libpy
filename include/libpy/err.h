#pragma once

#include <sstream>

#include "libpy/object.h"
#include "libpy/type.h"

#define LIBPY_HAVE_INTERRUPTED_ERROR (PY_VERSION_HEX >= 0x03500000)
#define LIBPY_HAVE_MODULE_NOT_FOUND_ERROR (PY_VERSION_HEX >= 0x03600000)
#define LIBPY_HAVE_RECURSION_ERROR (PY_VERSION_HEX >= 0x03500000)

namespace py {
namespace err {
/**
   A subclass of `py::object` for optional exceptions.
*/
class object : public py::object {
private:
    /**
       Function called to verify that `ob` is an exception and
       correctly raise a python exception otherwise.
    */
    void exception_check();
public:
    /**
       Default constructor. This will set `ob` to nullptr.
    */
    object();

    /**
       Constructor from `PyObject*`. If `pob` is not an `exception` then
       `ob` will be set to `nullptr`.
    */
    object(PyObject *pob);

    /**
       Constructor from `py::object`. If `pob` is not an `exception` then
       `ob` will be set to `nullptr`.
    */
    object(const py::object &pob);

    object(const object &cpfrom);
    object(object &&mvfrom) noexcept;

    using py::object::operator=;

    /**
       Get the traceback of the exception.

       @return The traceback associated with this exception if one exists,
               otherwise the returned object is `nullptr`.
    */
    py::tmpref<py::object> traceback() const;

    /**
       Set the traceback of the exception.

       @param tb The new traceback to set. Use `py::None` to clear it.
       @return Zero on success, non-zero on failure. This will raise a python
               exception on failure.
    */
    int traceback(py::object tb);

    /**
       Get the context of the exception, set when an exception is raised
       handling another exception.

       @return The context associated with this exception if one exists,
               otherwise the returned object is `nullptr`.
    */

    py::tmpref<py::object> context() const;

    /**
       Set the context of the exception.

       @param ctx The new context to set. Use `nullptr` to clear it. This steals
                  a reference to `ctx`.
    */
    void context(py::object ctx);

    /**
       Get the cause of the exception set by a `raise ... from ...`.

       @return The cause associated with this exception if one exists,
               otherwise the returned object is `py::None`.
    */
    py::tmpref<py::object> cause() const;

    /**
       Set the cause of the exception.

       @param cs The new cause to set. Use `nullptr` to clear it. This steals a
                 reference to `cs`.
    */
    void cause(py::object cs);
};

typedef py::type::object<object> exctype;

extern const exctype BaseException;
extern const exctype Exception;
extern const exctype ArithmeticError;
extern const exctype LookupError;
extern const exctype AssertionError;
extern const exctype AttributeError;
extern const exctype BlockingIOError;
extern const exctype BrokenPipeError;
extern const exctype ChildProcessError;
extern const exctype ConnectionError;
extern const exctype ConnectionAbortedError;
extern const exctype ConnectionRefusedError;
extern const exctype ConnectionResetError;
extern const exctype FileExistsError;
extern const exctype FileNotFoundError;
extern const exctype EOFError;
extern const exctype FloatingPointError;
extern const exctype ImportError;
#if LIBPY_HAVE_MODULE_NOT_FOUND_ERROR
extern const exctype ModuleNotFoundError;
#endif
extern const exctype IndexError;
#if LIBPY_HAVE_INTERRUPTED_ERROR
extern const exctype InterruptedError;
#endif
extern const exctype IsADirectoryError;
extern const exctype KeyError;
extern const exctype KeyboardInterrupt;
extern const exctype MemoryError;
extern const exctype NameError;
extern const exctype NotADirectoryError;
extern const exctype NotImplementedError;
extern const exctype OSError;
extern const exctype OverflowError;
extern const exctype PermissionError;
extern const exctype ProcessLookupError;
#if LIBPY_HAVE_RECURSION_ERROR
extern const exctype RecursionError;
#endif
extern const exctype ReferenceError;
extern const exctype RuntimeError;
extern const exctype SyntaxError;
extern const exctype SystemError;
extern const exctype TimeoutError;
extern const exctype SystemExit;
extern const exctype TypeError;
extern const exctype ValueError;
extern const exctype ZeroDivisionError;
#ifdef MS_WINDOWS
extern const exctype WindowsError;
#endif

// OSError aliases
extern const exctype EnvironmentError;
extern const exctype IOError;

/**
   Helper class used to raise exceptions with error messages.
*/
class msgbuilder : public std::stringstream {
private:
    exctype type;
    bool fire;
protected:
    friend msgbuilder raise(exctype type);
    msgbuilder(exctype type);
    msgbuilder(msgbuilder &&type) noexcept;
public:
    ~msgbuilder();
};

/**
   Get the currently raised exception. This object will be wrapping `nullptr`
   if there is no active exception.

   @return The currently raised exception if any.
*/
inline exctype occurred() {
    return PyErr_Occurred();
}

/**
   Clear the active exception. If there is no active exception, this is a nop.
*/
inline void clear() {
    PyErr_Clear();
}

/**
   Raise an exception with a message.

   @param type The type of exception to raise.
   @return An `ostream` which will accumulate the message to send.
*/
msgbuilder raise(exctype type);

/**
   Raise an exception value.
*/
void raise(py::err::object err);

/**
   Raise `py::err::MemoryError` and return `nullptr`.

   @return Always `nullptr` so users may write: `return no_memory()`.
*/
inline std::nullptr_t no_memory() {
    PyErr_NoMemory();
    return nullptr;
}

/**
   Raise `type` with the errno integer and message from `strerror()`.

   @param type The type of exception to raise.

   @return Always `nullptr` so users may write: `return set_from_errno(...)`.
*/
inline std::nullptr_t set_from_errno(exctype type) {
    PyErr_SetFromErrno(type);
    return nullptr;
}

/**
   Raise `type` with the errno integer and message from `strerror()`.

   @param type The type of exception to raise.
   @param filename If not null, `filename` is passed as the third argument to
                   the constructor of `type` after `errno` and `strerror()`.

   @return Always `nullptr` so users may write: `return set_from_errno(...)`.
*/
inline std::nullptr_t set_from_errno(exctype type, py::object filename) {
    PyErr_SetFromErrnoWithFilenameObject(type, filename);
    return nullptr;
}


/**
   Raise `type` with the errno integer and message from `strerror()`.

   @param type The type of exception to raise.
   @param filename1 Passed as the third argument to the constructor of `type`
                    after `errno` and `strerror()`.
   @param filename2 Passed as the fourth argument to the constructor of `type`
                    after `errno`, `strerror()`, and `filename1`.

   @return Always `nullptr` so users may write: `return set_from_errno(...)`.
*/
inline std::nullptr_t set_from_errno(exctype type,
                                     py::object filename1,
                                     py::object filename2) {
    PyErr_SetFromErrnoWithFilenameObjects(type, filename1, filename2);
    return nullptr;
}
}
}
