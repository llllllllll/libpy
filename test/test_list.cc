#include <tuple>
#include <typeinfo>

#include <gtest/gtest.h>
#include <Python.h>

#include "libpy/libpy.h"

using py::operator""_p;

TEST(List, type) {
    ASSERT_EQ((PyObject*) py::list::type, (PyObject*) &PyList_Type);
    auto t = py::list::type();

    EXPECT_EQ((PyObject*) t.type(), (PyObject*) &PyList_Type);
}

TEST(List, object_indexing) {
    std::array<py::object, 3> expected = {0_p, 1_p, 2_p};
    auto ob = py::list::pack(0_p, 1_p, 2_p);

    ASSERT_EQ(ob.len(), 3);
    for (const auto &n : expected) {
        EXPECT_EQ((PyObject*) ob[n], (PyObject*) n);
    }
}

TEST(List, ssize_t_indexing) {
    std::array<py::object, 3> expected = {0_p, 1_p, 2_p};
    auto ob = py::list::pack(0_p, 1_p, 2_p);

    ASSERT_EQ(ob.len(), 3);
    for (ssize_t n : {0, 1, 2}) {
        EXPECT_TRUE(ob[n].is(expected[n]));
    }
}

TEST(List, iteration) {
    std::array<py::object, 3> expected = {0_p, 1_p, 2_p};
    auto ob = py::list::pack(0_p, 1_p, 2_p);
    std::size_t n = 0;

    ASSERT_EQ(ob.len(), 3);
    for (const auto &e : ob) {
        ASSERT_TRUE((std::is_same<decltype(e), const py::object&>::value)) <<
            "const iteration over ob does not yield correct type";

        EXPECT_TRUE(e.is(expected[n++]));
    }
    EXPECT_EQ(n, 3) << "ran through too many iterations";
}
