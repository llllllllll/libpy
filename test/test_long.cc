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

#define FROM_NUMERIC_TEST_BASE(type) {                  \
        auto n = long_::object((type) 1).as_tmpref();   \
                                                        \
        EXPECT_EQ(n.as_long(), 1);                      \
        EXPECT_NO_PYTHON_ERR();                         \
    }

#define FROM_FLOATING_TEST(type)                \
    TEST(Long, from_ ## type) {                 \
        FROM_NUMERIC_TEST_BASE(type)            \
    }

#define FROM_INTEGRAL_TEST(type)                                        \
    TEST(Long, from_ ## type) {                                         \
        FROM_NUMERIC_TEST_BASE(type)                                    \
                                                                        \
        {                                                               \
            auto n = long_::object((unsigned type) 1).as_tmpref();      \
            EXPECT_EQ(n.as_long(), 1);                                  \
            EXPECT_NO_PYTHON_ERR();                                     \
        }                                                               \
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
            auto ob = long_::object(n).as_tmpref();                     \
                                                                        \
            EXPECT_EQ(ob.method(), n);                                  \
            EXPECT_NO_PYTHON_ERR();                                     \
        }                                                               \
        for (const auto &n : {limits.min(), limits.max()}) {            \
            auto ob = long_::object(n).as_tmpref();                     \
                                                                        \
            EXPECT_LT(ob.method() - n, 0.00000001);                     \
            EXPECT_NO_PYTHON_ERR();                                     \
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
            EXPECT_EQ(ob.method(overflow), -1);                         \
            EXPECT_NO_PYTHON_ERR();                                     \
            EXPECT_EQ(overflow, 1);                                     \
        }                                                               \
    }

AS_NUMERIC_AND_OVERFLOW_TEST(as_long_and_overflow)
AS_NUMERIC_AND_OVERFLOW_TEST(as_long_long_and_overflow)

TEST(Long, check) {
    auto n = 1_p;
    auto m = long_::object(1).as_tmpref();

    EXPECT_TRUE(long_::check(n));
    EXPECT_TRUE(long_::checkexact(n));
    EXPECT_TRUE(long_::check(n.as_nonnull()));
    EXPECT_TRUE(long_::checkexact(n.as_nonnull()));

    // tmpref
    EXPECT_TRUE(long_::check(m));
    EXPECT_TRUE(long_::checkexact(m));
    EXPECT_TRUE(long_::check(m.as_nonnull()));
    EXPECT_TRUE(long_::checkexact(m.as_nonnull()));
}
