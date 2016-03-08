#include <sstream>

#include <gtest/gtest.h>

#include "libpy/object.h"

using namespace py;

TEST(Object, ostream_nonnull) {
    std::stringstream stream;
    stream << 1_p;
    EXPECT_EQ(stream.str(), "1");
    stream.str("");
    stream << "test"_p;
    EXPECT_EQ(stream.str(), "test");
    stream.str("");
    stream << (1_p).as_nonnull();
    EXPECT_EQ(stream.str(), "1");
    stream.str("");
    stream << ("test"_p).as_nonnull();
    EXPECT_EQ(stream.str(), "test");
}

TEST(Object, ostream_out) {
    std::stringstream stream;
    stream << object(nullptr);
    EXPECT_EQ(stream.str(), "<NULL>");
}
