#include <Python.h>

#include "gtest/gtest.h"

// gdb doesn't see the PyLongObject type unless we have a variable with this
// type somewhere in the binary.
static PyLongObject _gdb_helper __attribute__((unused));

int main(int argc, char **argv) {
    int ret;
    testing::InitGoogleTest(&argc, argv);
    Py_Initialize();
    ret = RUN_ALL_TESTS();
    Py_Finalize();
    return ret;
}
