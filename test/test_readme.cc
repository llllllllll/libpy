#include <gtest/gtest.h>

#include "libpy/libpy.h"

using py::operator""_p;

py::tmpref<py::object> f() {
    return "ayy.lmao"_p.getattr("find"_p)("."_p) + 1_p + 2.5_p;
}

TEST(ReadMe, example) {
    auto ob = f();
    EXPECT_EQ(ob.refcnt(), 1);
    EXPECT_TRUE((f() == 6.5_p).istrue());
}
