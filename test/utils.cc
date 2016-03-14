#include <string>
#include <cxxabi.h>

std::string demangle(const char *name) {
    int status;
    return abi::__cxa_demangle(name, 0, 0, &status);
}
