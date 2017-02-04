#include <exception>

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

TEST(Err, from_cxx_err) {
    struct err_type : public std::exception {
        virtual const char* what() const noexcept {
            return "ayy lmao";
        }
    };

    try {
        py::err::raise(err_type{});
    }
    catch(const err_type &e) {
        // we are explicitly catching an `err_type` to test that this
        // polymorphically throws instead of upcasting to std::exception
        EXPECT_PYTHON_ERR_MSG(py::err::RuntimeError, "ayy lmao"_p);
        return;
    }

    FAIL() << "C++ exception wasn't thrown";
}

TEST(Err, occurred) {
    PyErr_SetString(py::err::TypeError, "ayy lmao");
    EXPECT_IS(py::err::occurred(), py::err::TypeError);
    py::err::clear();
}
