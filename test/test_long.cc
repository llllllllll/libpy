#include <type_traits>

#include <gtest/gtest.h>

#include "libpy/libpy.h"

using namespace py;

TEST(Long, default) {
    long_::object n;

    EXPECT_EQ((PyObject*) n, nullptr);
    EXPECT_FALSE(PyErr_Occurred());
}

#define FROM_NUMERIC_TEST_BASE(type)            \
        long_::object n((type) 1);              \
                                                \
        EXPECT_EQ(n.as_long(), 1);              \
        EXPECT_FALSE(PyErr_Occurred());         \

#define FROM_FLOATING_TEST(type)                \
    TEST(Long, from_ ## type) {                 \
        FROM_NUMERIC_TEST_BASE(type)            \
    }

#define FROM_INTEGRAL_TEST(type)                \
    TEST(Long, from_ ## type) {                 \
        FROM_NUMERIC_TEST_BASE(type)            \
                                                \
        {                                       \
            long_::object m((unsigned type) 1); \
            EXPECT_EQ(m.as_long(), 1);          \
            EXPECT_FALSE(PyErr_Occurred());     \
        }                                       \
    }

FROM_INTEGRAL_TEST(short)
FROM_INTEGRAL_TEST(int)
FROM_INTEGRAL_TEST(long)

namespace {
    typedef long long long_long;
}

FROM_INTEGRAL_TEST(long_long)

FROM_FLOATING_TEST(float)
FROM_FLOATING_TEST(double)
