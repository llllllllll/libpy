#include <gtest/gtest.h>

#include "libpy/object.h"

using namespace py;

TEST(TmpRef, refcnt) {
    ssize_t start_count = None.refcnt();  // the refcnt before we del the tmpref
    None.incref();  // account for the destruction that should be dropped
    {
        tmpref<object> tmp(Py_None);
    }
    EXPECT_EQ(start_count, None.refcnt());
}
