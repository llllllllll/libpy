#include <type_traits>

#include "gtest/gtest.h"

#include "libpy/libpy.h"

using py::operator""_p;

TEST(UserDefinedLiterals, char) {
    py::object c = 'c'_p;
    EXPECT_EQ(static_cast<PyObject*>(c.type()),
              reinterpret_cast<PyObject*>(&PyUnicode_Type));
    EXPECT_STREQ(PyUnicode_AsUTF8(c), "c");
    EXPECT_TRUE((c == 'c'_p).istrue());
    // char is equiv to string of length 1
    EXPECT_TRUE((c == "c"_p).istrue());
}

TEST(UserDefinedLiterals, string) {
    py::object cs = "test"_p;
    EXPECT_EQ(static_cast<PyObject*>(cs.type()),
              reinterpret_cast<PyObject*>(&PyUnicode_Type));
    EXPECT_STREQ(PyUnicode_AsUTF8(cs), "test");
    EXPECT_TRUE((cs == "test"_p).istrue());
}

TEST(UserDefinedLiterals, wchar_t) {
    py::object c = L'c'_p;
    EXPECT_EQ(static_cast<PyObject*>(c.type()),
              reinterpret_cast<PyObject*>(&PyUnicode_Type));
    EXPECT_STREQ(PyUnicode_AsUTF8(c), "c");
    // char is equiv to string of length 1
    EXPECT_TRUE((c == 'c'_p).istrue());
    EXPECT_TRUE((c == "c"_p).istrue());
}

TEST(UserDefinedLiterals, wstring) {
    py::object cs = L"test"_p;
    EXPECT_EQ(static_cast<PyObject*>(cs.type()),
              reinterpret_cast<PyObject*>(&PyUnicode_Type));
    EXPECT_STREQ(PyUnicode_AsUTF8(cs), "test");
    EXPECT_TRUE((cs == "test"_p).istrue());
}

TEST(UserDefinedLiterals, ull) {
    auto n = 10_p;

    // check that unsigned long long literals infer as long objects
    ASSERT_TRUE((std::is_same<decltype(n), py::long_::object>::value));

    EXPECT_EQ(PyLong_AS_LONG(n), 10);
    EXPECT_EQ(n.as_long(), 10);
    EXPECT_TRUE((n == 10_p).istrue());
}

TEST(UserDefinedLiterals, longdouble) {
    py::object n = 2.5_p;
    EXPECT_EQ(static_cast<PyObject*>(n.type()),
              reinterpret_cast<PyObject*>(&PyFloat_Type));
    EXPECT_EQ(PyFloat_AS_DOUBLE(static_cast<PyObject*>(n)), 2.5);
    EXPECT_TRUE((n == 2.5_p).istrue());
}
