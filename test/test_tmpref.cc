#include <gtest/gtest.h>

#include "libpy/object.h"

TEST(TmpRef, refcnt) {
    // the refcnt before we del the tmpref
    ssize_t start_count = py::None.refcnt();
    py::None.incref();  // account for the destruction that should be dropped
    {
        py::tmpref<py::object> tmp(Py_None);
    }
    EXPECT_EQ(start_count, py::None.refcnt());
}
