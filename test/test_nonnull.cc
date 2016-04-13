#include <gtest/gtest.h>

#include "libpy/object.h"

TEST(NonNull, test_good_nonnull) {
    EXPECT_EQ((PyObject*) py::None.as_nonnull(), (PyObject*) py::None) <<
        "None.as_nonnull() should wrap the same object as None";

    EXPECT_TRUE(py::None.as_nonnull().is_nonnull());
}

TEST(NonNull, test_bad_nonnull) {
    py::object c(nullptr);

    EXPECT_FALSE(c.is_nonnull());

    try {
        c.as_nonnull();
        ASSERT_FALSE(true) << "bad_nonnull was not thrown";
    }
    catch (pyutils::bad_nonnull &e) {
    }
}
