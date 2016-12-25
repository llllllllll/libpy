#include <gtest/gtest.h>
#include <Python.h>

#include "libpy/libpy.h"
#include "utils.h"

using py::operator""_p;

TEST(Err, raise_with_message) {
    py::err::raise(py::err::TypeError) << "ayy lmao: " << 1_p;
    EXPECT_PYTHON_ERR_MSG(py::err::TypeError, "ayy lmao: 1"_p);
}

TEST(Err, raise_value) {
    py::err::raise(py::err::TypeError("ayy lmao"_p));
    EXPECT_PYTHON_ERR_MSG(py::err::TypeError, "ayy lmao"_p);
}

TEST(Err, occurred) {
    PyErr_SetString(py::err::TypeError, "ayy lmao");
    EXPECT_IS(py::err::occurred(), py::err::TypeError);
    py::err::clear();
}
