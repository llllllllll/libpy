#pragma once

#include "gtest/gtest.h"
#include <Python.h>

#include "libpy/libpy.h"

/**
   Expectation that two object are the same object in memory.
*/
#define EXPECT_IS(a, b) EXPECT_EQ(static_cast<PyObject*>(a),    \
                                  static_cast<PyObject*>(b))

/**
   Expectation that two object are *not* the same object in memory.
*/
#define EXPECT_IS_NOT(a, b) EXPECT_NE(static_cast<PyObject*>(a),    \
                                      static_cast<PyObject*>(b))
/**
   Expectation that no python errors have been raised.
   When this fails the exception is printed with PyErr_Print and then it is
   cleared.
*/
#define EXPECT_NO_PYTHON_ERR() {                                        \
        py::object occurred(PyErr_Occurred());                          \
        EXPECT_FALSE(occurred) << "actually raised: " << occurred;      \
        if (occured) {                                                  \
            PyErr_Print();                                              \
            PyErr_Clear();                                              \
        }                                                               \
    }(void) 0

/**
   Expectation that a particular kind of python exception was raised.
   Any exceptions are cleared after this.

   @param type The python exception class to check against.
*/
#define EXPECT_PYTHON_ERR(type) {                       \
        bool matches = PyErr_ExceptionMatches(type);    \
        EXPECT_TRUE(matches);                           \
        PyErr_Clear();                                  \
    }(void) 0


/**
   Expectation that a particular kind of python exception was raised and that
   the message matches some text.
   Any exceptions are cleared after this.

   @param type The python exception class to check against.
   @param msg The message text to check for.
*/
#define EXPECT_PYTHON_ERR_MSG(type, msg) {                              \
        bool matches = PyErr_ExceptionMatches(type);                    \
        EXPECT_TRUE(matches);                                           \
        PyObject *type_;                                                \
        PyObject *value;                                                \
        PyObject *tb;                                                   \
        PyErr_Fetch(&type_, &value, &tb);                               \
        EXPECT_TRUE((py::object(value).str() == msg).istrue());         \
        Py_DECREF(type_);                                               \
        Py_DECREF(value);                                               \
        Py_XDECREF(tb);                                                 \
    }(void) 0

/**
   Demangle a name from typeid(T).name().

   @param name The name from typeid.
   @return     The demangled named.
*/
std::string demangle(const char *name);
