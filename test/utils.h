#pragma once

#include <gtest/gtest.h>
#include <Python.h>

/**
   Expectation that no python errors have been raised.
   When this fails the exception is printed with PyErr_Print and then it is
   cleared.
*/
#define EXPECT_NO_PYTHON_ERR() {                \
        PyObject *occured = PyErr_Occurred();   \
        EXPECT_FALSE(occured);                  \
        if (occured) {                          \
            PyErr_Print();                      \
            PyErr_Clear();                      \
        }                                       \
    }(void) 0

/**
   Expectation that a particular kind of python exception was raised.

   If the correct type was raised the exception is cleared.

   @param type The python exception class to check against.
*/
#define EXPECT_PYTHON_ERR(type) {                       \
        bool matches = PyErr_ExceptionMatches(type);    \
        EXPECT_TRUE(matches);                           \
        if (matches) {                                  \
            PyErr_Clear();                              \
        }                                               \
    }(void) 0
