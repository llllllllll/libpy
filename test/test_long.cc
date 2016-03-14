#include <limits>
#include <type_traits>
#include <typeinfo>

#include <gtest/gtest.h>

#include "libpy/libpy.h"
#include "utils.h"

using namespace py;

TEST(Long, default) {
    long_::object n;

    EXPECT_EQ((PyObject*) n, nullptr);
    EXPECT_FALSE(PyErr_Occurred());
}

#define FROM_NUMERIC_TEST_BASE(type) {          \
        long_::object n((type) 1);              \
                                                \
        EXPECT_EQ(n.as_long(), 1);              \
        EXPECT_NO_PYTHON_ERR();                 \
        n.decref();                             \
    }

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
            EXPECT_NO_PYTHON_ERR();             \
            m.decref();                         \
        }                                       \
    }

FROM_INTEGRAL_TEST(short)
FROM_INTEGRAL_TEST(int)
FROM_INTEGRAL_TEST(long)
FROM_FLOATING_TEST(float)
FROM_FLOATING_TEST(double)

#define AS_NUMERIC_TEST(method)                                         \
    TEST(Long, as_ ## method) {                                         \
        using R = decltype(long_::object().method());                   \
        std::numeric_limits<R> limits;                                  \
                                                                        \
        for (const auto &n : {0, 1, std::is_unsigned<R>::value ? 2 : -1}) { \
            long_::object ob(n);                                        \
                                                                        \
            EXPECT_EQ(ob.method(), n);                                  \
            EXPECT_NO_PYTHON_ERR();                                     \
            ob.decref();                                                \
        }                                                               \
        for (const auto &n : {limits.min(), limits.max()}) {            \
            long_::object ob(n);                                        \
                                                                        \
            EXPECT_LT(ob.method() - n, 0.00000001);                     \
            EXPECT_NO_PYTHON_ERR();                                     \
            ob.decref();                                                \
        }                                                               \
    }

AS_NUMERIC_TEST(as_long)
AS_NUMERIC_TEST(as_long_long)
AS_NUMERIC_TEST(as_ssize_t)
AS_NUMERIC_TEST(as_size_t)
AS_NUMERIC_TEST(as_unsigned_long)
AS_NUMERIC_TEST(as_unsigned_long_long)
AS_NUMERIC_TEST(as_double)


#define AS_NUMERIC_AND_OVERFLOW_TEST(method)                            \
    TEST(Long, as_ ## method) {                                         \
        using R = decltype(long_::object().method(std::declval<int&>())); \
        std::numeric_limits<R> limits;                                  \
                                                                        \
        for (const auto &n : {0, 1, std::is_unsigned<R>::value ? 2 : -1}) { \
            auto ob = long_::object(n).as_tmpref();                     \
            int overflow;                                               \
                                                                        \
            EXPECT_EQ((ob + 1_p).method(overflow), n + 1);              \
            EXPECT_NO_PYTHON_ERR();                                     \
            EXPECT_EQ(overflow, 0);                                     \
                                                                        \
            EXPECT_EQ((ob - 1_p).method(overflow), n - 1);              \
            EXPECT_NO_PYTHON_ERR();                                     \
            EXPECT_EQ(overflow, 0);                                     \
        }                                                               \
        {                                                               \
            auto ob = long_::object(limits.min()).as_tmpref() - 1_p;    \
            int overflow;                                               \
                                                                        \
            EXPECT_EQ(ob.method(overflow), -1);                         \
            EXPECT_NO_PYTHON_ERR();                                     \
            EXPECT_EQ(overflow, -1);;                                   \
        }                                                               \
                                                                        \
        {                                                               \
            auto ob = long_::object(limits.max()).as_tmpref() + 1_p;    \
            int overflow;                                               \
                                                                        \
            EXPECT_EQ(ob.method(overflow), -1);                        \
            EXPECT_NO_PYTHON_ERR();                                     \
            EXPECT_EQ(overflow, 1);                                     \
        }                                                               \
    }

AS_NUMERIC_AND_OVERFLOW_TEST(as_long_and_overflow)
AS_NUMERIC_AND_OVERFLOW_TEST(as_long_long_and_overflow)
