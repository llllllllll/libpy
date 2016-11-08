#include <array>
#include <tuple>
#include <typeinfo>

#include "gtest/gtest.h"
#include <Python.h>

#include "libpy/libpy.h"
#include "utils.h"

using py::operator""_p;

TEST(List, type) {
    ASSERT_EQ(static_cast<PyObject*>(py::list::type),
              reinterpret_cast<PyObject*>(&PyList_Type));
    auto t = py::list::type();

    EXPECT_EQ(static_cast<PyObject*>(t.type()),
              reinterpret_cast<PyObject*>(&PyList_Type));
}

TEST(List, object_indexing) {
    std::array<py::object, 3> expected({0_p, 1_p, 2_p});
    auto ob = py::list::pack(0_p, 1_p, 2_p);

    ASSERT_EQ(ob.len(), 3);
    for (const auto &n : expected) {
        EXPECT_IS(ob[n], n);
    }
}

TEST(List, ssize_t_indexing) {
    std::array<py::object, 3> expected({0_p, 1_p, 2_p});
    auto ob = py::list::pack(0_p, 1_p, 2_p);

    ASSERT_EQ(ob.len(), 3);
    for (ssize_t n : {0, 1, 2}) {
        EXPECT_TRUE(ob[n].is(expected[n]));
    }
}

TEST(List, iteration) {
    std::array<py::object, 3> expected({0_p, 1_p, 2_p});
    auto ob = py::list::pack(0_p, 1_p, 2_p);
    std::size_t n = 0;

    ASSERT_EQ(ob.len(), 3);
    for (const auto &e : ob) {
        ASSERT_TRUE((std::is_same<decltype(e), const py::object&>::value)) <<
            "const iteration over ob does not yield correct type";

        EXPECT_IS(e, expected[n++]);
    }
    EXPECT_EQ(n, 3u) << "ran through too many iterations";
}
