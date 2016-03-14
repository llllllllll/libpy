#include <unordered_map>
#include <utility>

#include "libpy/long.h"
#include "libpy/utils.h"

using namespace py;

const long_::object &py::operator""_p(unsigned long long l) {
    static std::unordered_map<unsigned long long, long_::object> cache;
    long_::object &ob = cache[l];
    if (!ob.is_nonnull()) {
        ob.ob = PyLong_FromUnsignedLongLong(l);
    }
    return ob;
}

long_::object::object() : py::object(nullptr) {}

long_::object::object(PyObject *pob) : py::object(pob) {
    long_check();
}

long_::object::object(const py::object &pob) : py::object(pob) {
    long_check();
}

long_::object::object(const long_::object &cpfrom) :
    py::object((PyObject*) cpfrom) {}

long_::object::object(long_::object &&mvfrom) noexcept :
    py::object((PyObject*) mvfrom) {
    mvfrom.ob = nullptr;
}

void long_::object::long_check() {
    if (ob && !PyLong_Check(ob)) {
        ob = nullptr;
        if (!PyErr_Occurred()) {
            PyErr_SetString(PyExc_TypeError,
                            "cannot make py::long::object from non int");
        }
    }
}

long long_::object::as_long() const {
    return as_t<long, PyLong_AsLong>();
}

long long_::object::as_long_and_overflow(int &overflow) const {
    return as_t_and_overflow<long, PyLong_AsLongAndOverflow>(overflow);
}

long long long_::object::as_long_long() const {
    return as_t<long long, PyLong_AsLongLong>();
}


long long long_::object::as_long_long_and_overflow(int &overflow) const {
    return as_t_and_overflow<long long, PyLong_AsLongLongAndOverflow>(overflow);
}

ssize_t long_::object::as_ssize_t() const {
    return as_t<ssize_t, PyLong_AsSsize_t>();
}

unsigned long long_::object::as_unsigned_long() const {
    return as_t<unsigned long, PyLong_AsUnsignedLong>();
}

std::size_t long_::object::as_size_t() const {
    return as_t<std::size_t, PyLong_AsSize_t>();
}

unsigned long long long_::object::as_unsigned_long_long() const {
    return as_t<unsigned long long, PyLong_AsUnsignedLongLong>();
}

double long_::object::as_double() const {
    return as_t<double, PyLong_AsDouble>();
}

nonnull<long_::object> long_::object::as_nonnull() const {
    if (!is_nonnull()) {
        throw pyutils::bad_nonnull();
    }
    return nonnull<long_::object>(ob);
}

tmpref<long_::object> long_::object::as_tmpref() && {
    tmpref<object> ret(ob);
    ob = nullptr;
    return std::move(ret);
}

tmpref<long_::object> long_::object::operator-() const {
    return ob_unary_func<PyNumber_Negative>();
}

tmpref<long_::object> long_::object::operator+() const {
    return ob_unary_func<PyNumber_Positive>();
}

tmpref<long_::object> long_::object::abs() const {
    return ob_unary_func<PyNumber_Absolute>();
}

tmpref<long_::object> long_::object::invert() const {
    return ob_unary_func<PyNumber_Invert>();
}
