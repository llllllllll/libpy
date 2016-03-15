#include <string>
#include <cxxabi.h>

std::string demangle(const char *name) {
    int status;
    char *cs = abi::__cxa_demangle(name, 0, 0, &status);
    std::string ret = cs;
    free(cs);
    return std::move(ret);
}
