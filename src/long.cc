#include <unordered_map>
#include <utility>

#include "libpy/long.h"
#include "libpy/utils.h"

using namespace py;

const plong::object &py::operator""_p(unsigned long long l) {
    static std::unordered_map<unsigned long long, plong::object> cache;
    plong::object &ob = cache[l];
    if (!ob.is_nonnull()) {
        ob.ob = PyLong_FromUnsignedLongLong(l);
    }
    return ob;
}

plong::object::object(long l) : py::object(PyLong_FromLong(l)) {}

plong::object::object(unsigned long l) :
    py::object(PyLong_FromUnsignedLong(l)) {}

plong::object::object(long long l) : py::object(PyLong_FromLongLong(l)) {}

plong::object::object(unsigned long long l) :
    py::object(PyLong_FromUnsignedLongLong(l)) {}

plong::object::object() : py::object(nullptr) {}

plong::object::object(PyObject *pob) : py::object(pob) {
    long_check();
}

plong::object::object(const py::object &pob) : py::object(pob) {
    long_check();
}

plong::object::object(const plong::object &cpfrom) :
    py::object((PyObject*) cpfrom) {}

plong::object::object(plong::object &&mvfrom) noexcept :
    py::object((PyObject*) mvfrom) {
    mvfrom.ob = nullptr;
}

void plong::object::long_check() {
    if (ob && !PyLong_Check(ob)) {
        ob = nullptr;
        if (!PyErr_Occurred()) {
            PyErr_SetString(PyExc_TypeError,
                            "cannot make py::long::object from non int");
        }
    }
}

long plong::object::as_long() const {
    return as_t<long, PyLong_AsLong>();
}

long plong::object::as_long_and_overflow(int &overflow) const {
    return as_t_and_overflow<long, PyLong_AsLongAndOverflow>(overflow);
}

long long plong::object::as_long_long() const {
    return as_t<long long, PyLong_AsLongLong>();
}


long long plong::object::as_long_long_and_overflow(int &overflow) const {
    return as_t_and_overflow<long long, PyLong_AsLongLongAndOverflow>(overflow);
}

ssize_t plong::object::as_ssize_t() const {
    return as_t<ssize_t, PyLong_AsSsize_t>();
}

unsigned long plong::object::as_unsigned_long() const {
    return as_t<unsigned long, PyLong_AsUnsignedLong>();
}

std::size_t plong::object::as_size_t() const {
    return as_t<std::size_t, PyLong_AsSize_t>();
}

unsigned long long plong::object::as_unsigned_long_long() const {
    return as_t<unsigned long long, PyLong_AsUnsignedLongLong>();
}

double plong::object::as_double() const {
    return as_t<double, PyLong_AsDouble>();
}
