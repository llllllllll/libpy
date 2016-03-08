#include <sstream>
#include <unordered_map>

#include <gtest/gtest.h>

#include "libpy/object.h"

using namespace py;

TEST(Object, ostream_nonnull) {
    std::stringstream stream;
    std::unordered_map<std::string, object> subtests = {
        {"c", 'c'_p},
        {"1", 1_p},
        {"1.5", 1.5_p},
        {"test", "test"_p},

    };

    for (const auto &kv : subtests) {
        for (const auto &as_nonnull : {false, true}) {
            object ob = std::get<1>(kv);
            if (!as_nonnull) {
                stream << ob;
            } else {
                stream << ob.as_nonnull();
            }
            EXPECT_EQ(stream.str(), std::get<0>(kv)) <<
                "as_nonnull = " << as_nonnull << '\n';
            stream.str("");  // clear the stream's contents
        }
    }
}

TEST(Object, ostream_nullptr) {
    std::stringstream stream;
    stream << object(nullptr);
    EXPECT_EQ(stream.str(), "<NULL>");
}
