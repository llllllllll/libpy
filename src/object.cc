#include <unordered_map>
#include <utility>

#include "libpy/object.h"

const py::object py::None = Py_None;
const py::object py::NotImplemented = Py_NotImplemented;
const py::object py::Ellipsis = Py_Ellipsis;
const py::object py::True = Py_True;
const py::object py::False = Py_False;

const py::object &py::operator""_p(char c) {
    static std::unordered_map<char, py::object> cache;
    py::object &ob = cache[c];
    if (!ob.is_nonnull()) {
        ob.ob = PyUnicode_FromStringAndSize(&c, 1);
    }
    return ob;
}

const py::object &py::operator""_p(const char *cs, std::size_t len) {
    static std::unordered_map<const char*, py::object> cache;
    py::object &ob = cache[cs];
    if (!ob.is_nonnull()) {
        ob.ob = PyUnicode_FromStringAndSize(cs, len);
    }
    return ob;
}

const py::object &py::operator""_p(wchar_t c) {
    static std::unordered_map<wchar_t, py::object> cache;
    py::object &ob = cache[c];
    if (!ob.is_nonnull()) {
        ob.ob = PyUnicode_FromWideChar(&c, 1);
    }
    return ob;
}

const py::object &py::operator""_p(const wchar_t *cs, std::size_t len) {
    static std::unordered_map<const wchar_t*, py::object> cache;
    py::object &ob = cache[cs];
    if (!ob.is_nonnull()) {
        ob.ob = PyUnicode_FromWideChar(cs, len);
    }
    return ob;
}

const py::object &py::operator""_p(long double d) {
    static std::unordered_map<long double, py::object> cache;
    py::object &ob = cache[d];
    if (!ob.is_nonnull()) {
        ob.ob = PyFloat_FromDouble(d);
    }
    return ob;
}

std::ostream &py::operator<<(std::ostream &stream, const py::object &ob) {
    /* We can avoid the null check because this happens in PyUnicode_AsUTF8.
       When ob is nullptr the result is "<NULL>". */
    return stream << PyUnicode_AsUTF8(ob.str());
}

py::object &py::object::operator=(const py::object &cpfrom) {
    py::object tmp(cpfrom);
    return (*this = std::move(tmp));
}

py::object &py::object::operator=(py::object &&mvfrom) noexcept {
    ob = mvfrom.ob;
    mvfrom.ob = nullptr;
    return *this;
}

int py::object::print(FILE *f, int flags) const {
    return PyObject_Print(ob, f, flags);
}

py::tmpref<py::object> py::object::repr() const {
    return PyObject_Repr(ob);
}

py::tmpref<py::object> py::object::ascii() const {
    return PyObject_ASCII(ob);
}

py::tmpref<py::object> py::object::str() const {
    return PyObject_Str(ob);
}

py::tmpref<py::object> py::object::bytes() const {
    return PyObject_Bytes(ob);
}

bool py::object::iscallable() const {
    return PyCallable_Check(ob);
}

py::hash_t py::object::hash() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyObject_Hash(ob);
}

int py::object::istrue() const {
    return int_unary_func<PyObject_IsTrue>();
}

py::object py::object::type() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return reinterpret_cast<PyObject*>(Py_TYPE(ob));
}

ssize_t py::object::len() const {
    // PyObject_Length does its own null checks
    return PyObject_Length(ob);
}

ssize_t py::object::lenhint(ssize_t fallback) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyObject_LengthHint(ob, fallback);
}

py::tmpref<py::object> py::object::dir() const {
    // PyObject_Dir(NULL) has a very different meaning than expected here
    // so we will raise in that case
    return ob_unary_func<PyObject_Dir>();
}

py::tmpref<py::object> py::object::iter() const {
    return ob_unary_func<PyObject_GetIter>();
}

py::tmpref<py::object> py::object::next() const {
    // NULL is a valid input to PyIter_Next
    return PyIter_Next(ob);
}

py::object::iterator py::object::begin() const {
    return cbegin();
}

py::object::iterator py::object::end() const {
    return cend();
}

py::object::const_iterator py::object::cbegin() const {
    return py::object(ob_unary_func<PyObject_GetIter>());
}

py::object::const_iterator py::object::cend() const {
    return py::object::const_iterator();
}

py::tmpref<py::object> py::object::richcompare(const py::object &other, compareop opid) const {
    // PyObject_RichCompare does its own null checks
    return PyObject_RichCompare(ob, other.ob, opid);
}

py::tmpref<py::object> py::object::operator<(const py::object &other) const {
    return t_richcompare<LT>(other);
}

py::tmpref<py::object> py::object::operator<=(const py::object &other) const {
    return t_richcompare<LE>(other);
}

py::tmpref<py::object> py::object::operator==(const py::object &other) const {
    return t_richcompare<EQ>(other);
}

py::tmpref<py::object> py::object::operator!=(const py::object &other) const {
    return t_richcompare<NE>(other);
}

py::tmpref<py::object> py::object::operator>(const py::object &other) const {
    return t_richcompare<GT>(other);
}

py::tmpref<py::object> py::object::operator>=(const py::object &other) const {
    return t_richcompare<GE>(other);
}

bool py::object::is(const py::object &other) const {
    return ob == other.ob;
}

py::tmpref<py::object> py::object::operator-() const {
    return ob_unary_func<PyNumber_Negative>();
}

py::tmpref<py::object> py::object::operator+() const {
    return ob_unary_func<PyNumber_Positive>();
}

py::tmpref<py::object> py::object::abs() const {
    return ob_unary_func<PyNumber_Absolute>();
}

py::tmpref<py::object> py::object::invert() const {
    return ob_unary_func<PyNumber_Invert>();
}

const py::object &py::object::incref() const {
    if (is_nonnull()) {
        Py_INCREF(ob);
    }
    return *this;
}

const py::object &py::object::decref() {
    if (is_nonnull()) {
        // reimplement the Py_DECREF macro here so that we can set ob = nullptr
        // when we dealloc without checking the refcount twice
        if (_Py_DEC_REFTOTAL  _Py_REF_DEBUG_COMMA --(ob)->ob_refcnt != 0) {
            _Py_CHECK_REFCNT(_py_decref_tmp)
        } else {
            _Py_Dealloc(ob);
            ob = nullptr;
        }
    }
    return *this;
}

const py::object &py::object::clear() {
    decref();
    ob = nullptr;
    return *this;
}

py::ssize_t py::object::refcnt() const {
    if (!is_nonnull()) {
        return -1;
    }
    return Py_REFCNT(ob);
}

py::nonnull<py::object> py::object::as_nonnull() const {
    if (!is_nonnull()) {
        throw pyutils::bad_nonnull();
    }
    return py::nonnull<py::object>(ob);
}


py::tmpref<py::object> py::object::as_tmpref() && {
    py::tmpref<py::object> ret(ob);
    ob = nullptr;
    return ret;
}
