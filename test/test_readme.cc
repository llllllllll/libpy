#include <gtest/gtest.h>

#include "libpy/object.h"

using namespace py;

py::object f() {
    using py::operator""_p;
    return "ayy.lmao"_p.getattr("find"_p)("."_p) + 1_p + 2.5_p;
}

TEST(ReadMe, example) {
    EXPECT_TRUE((f() == 6.5_p).istrue());
}
