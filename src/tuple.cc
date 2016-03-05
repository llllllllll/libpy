#include "libpy/tuple.h"
#include "libpy/utils.h"

using namespace py;
namespace t = tuple;

const object t::type = object((PyObject*) &PyTuple_Type);

t::object::object() : py::object() {}

t::object::object(PyObject *pob) :
    py::object(PyTuple_Check(pob) ? pob : nullptr) {}

t::object::object(const py::object &pob) :
    py::object(PyTuple_Check((PyObject*) pob) ? pob : nullptr) {}

t::object::object(const t::object &cpfrom) : py::object((PyObject*) cpfrom) {}

t::object::object(t::object &&mvfrom) noexcept :
    py::object((PyObject*) mvfrom) {
    mvfrom.ob = nullptr;
}

t::object &t::object::operator=(const t::object &cpfrom) {
    t::object tmp(cpfrom);
    return (*this = std::move(tmp));
}

t::object &t::object::operator=(t::object &&mvfrom) noexcept {
    ob = mvfrom.ob;
    mvfrom.ob = nullptr;
    return *this;
}

ssize_t t::object::len() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyTuple_GET_SIZE(ob);
}

PyObject *t::object::ob_getitem(ssize_t idx) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return PyTuple_GET_ITEM(ob, idx);
}

tmpref<object> t::object::operator[](ssize_t idx) const {
    return ob_getitem(idx);
}

tmpref<object> t::object::getitem(ssize_t idx) const {
    return ob_getitem(idx);
}

tmpref<object> t::object::getitem_checked(ssize_t idx) const {
    if (!is_nonnull()) {
        return nullptr;
    }
    return PyTuple_GetItem(ob, idx);
}

int t::object::setitem(ssize_t idx, const py::object &value) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    PyTuple_SET_ITEM(ob, idx, (PyObject*) value);
    return 0;
}

int t::object::setitem_checked(ssize_t idx, const py::object &value) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyTuple_SetItem(ob, idx, (PyObject*) value);
}

nonnull<t::object> t::object::as_nonnull() const {
    if (!is_nonnull()) {
        throw pyutils::bad_nonnull();
    }
    return nonnull<t::object>(ob);

}
