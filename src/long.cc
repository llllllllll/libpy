#include <unordered_map>
#include <utility>

#include "libpy/long.h"
#include "libpy/utils.h"

const py::long_::object &py::operator""_p(unsigned long long l) {
    static std::unordered_map<unsigned long long, py::long_::object> cache;
    py::long_::object &ob = cache[l];
    if (!ob.is_nonnull()) {
        ob.ob = PyLong_FromUnsignedLongLong(l);
    }
    return ob;
}

const py::type::object<py::long_::object> py::long_::type(&PyList_Type);

py::long_::object::object() : py::object(nullptr) {}

py::long_::object::object(PyObject *pob) : py::object(pob) {
    long_check();
}

py::long_::object::object(const py::object &pob) : py::object(pob) {
    long_check();
}

py::long_::object::object(const py::long_::object &cpfrom) :
    py::object(cpfrom.ob) {}

py::long_::object::object(py::long_::object &&mvfrom) noexcept :
    py::object(mvfrom.ob) {
    mvfrom.ob = nullptr;
}

void py::long_::object::long_check() {
    if (ob && !PyLong_Check(ob)) {
        ob = nullptr;
        if (!PyErr_Occurred()) {
            PyErr_SetString(PyExc_TypeError,
                            "cannot make py::long::object from non int");
        }
    }
}

long py::long_::object::as_long() const {
    return as_t<long, PyLong_AsLong>();
}

long py::long_::object::as_long_and_overflow(int &overflow) const {
    return as_t_and_overflow<long, PyLong_AsLongAndOverflow>(overflow);
}

long long py::long_::object::as_long_long() const {
    return as_t<long long, PyLong_AsLongLong>();
}


long long py::long_::object::as_long_long_and_overflow(int &overflow) const {
    return as_t_and_overflow<long long, PyLong_AsLongLongAndOverflow>(overflow);
}

ssize_t py::long_::object::as_ssize_t() const {
    return as_t<ssize_t, PyLong_AsSsize_t>();
}

unsigned long py::long_::object::as_unsigned_long() const {
    return as_t<unsigned long, PyLong_AsUnsignedLong>();
}

std::size_t py::long_::object::as_size_t() const {
    return as_t<std::size_t, PyLong_AsSize_t>();
}

unsigned long long py::long_::object::as_unsigned_long_long() const {
    return as_t<unsigned long long, PyLong_AsUnsignedLongLong>();
}

double py::long_::object::as_double() const {
    return as_t<double, PyLong_AsDouble>();
}

py::nonnull<py::long_::object> py::long_::object::as_nonnull() const {
    if (!is_nonnull()) {
        throw pyutils::bad_nonnull();
    }
    return py::nonnull<py::long_::object>(ob);
}

py::tmpref<py::long_::object> py::long_::object::as_tmpref() && {
    py::tmpref<object> ret(ob);
    ob = nullptr;
    return ret;
}

py::tmpref<py::long_::object> py::long_::object::operator-() const {
    return ob_unary_func<PyNumber_Negative>();
}

py::tmpref<py::long_::object> py::long_::object::operator+() const {
    return ob_unary_func<PyNumber_Positive>();
}

py::tmpref<py::long_::object> py::long_::object::abs() const {
    return ob_unary_func<PyNumber_Absolute>();
}

py::tmpref<py::long_::object> py::long_::object::invert() const {
    return ob_unary_func<PyNumber_Invert>();
}
