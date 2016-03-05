#include <gtest/gtest.h>
#include <Python.h>


int main(int argc, char **argv) {
  int ret;

  testing::InitGoogleTest(&argc, argv);
  Py_Initialize();
  ret = RUN_ALL_TESTS();
  Py_Finalize();
  return ret;
}
