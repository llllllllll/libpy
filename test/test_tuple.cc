#include <tuple>
#include <typeinfo>

#include <gtest/gtest.h>
#include <Python.h>

#include "libpy/libpy.h"

using namespace py;

TEST(Tuple, object_indexing) {
    std::array<object, 3> expected = {0_p, 1_p, 2_p};
    auto ob = tuple::pack(0_p, 1_p, 2_p).as_tmpref();

    ASSERT_EQ(ob.len(), 3);
    for (const auto &n : expected) {
        EXPECT_EQ((PyObject*) ob[n], (PyObject*) n);
    }
}

TEST(Tuple, ssize_t_indexing) {
    std::array<object, 3> expected = {0_p, 1_p, 2_p};
    auto ob = tuple::pack(0_p, 1_p, 2_p).as_tmpref();

    ASSERT_EQ(ob.len(), 3);
    for (ssize_t n : {0, 1, 2}) {
        EXPECT_EQ((PyObject*) ob[n], (PyObject*) expected[n]);
    }
}

TEST(Tuple, iteration) {
    std::array<object, 3> expected = {0_p, 1_p, 2_p};
    auto ob = tuple::pack(0_p, 1_p, 2_p).as_tmpref();
    std::size_t n = 0;

    ASSERT_EQ(ob.len(), 3);
    for (const auto &e : ob) {
        EXPECT_EQ((PyObject*) e, (PyObject*) expected[n++]);
    }
    EXPECT_EQ(n, 3) << "ran through too many iterations";
}
