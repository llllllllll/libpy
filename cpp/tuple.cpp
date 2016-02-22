#include "cpp/tuple.h"
#include "cpp/utils.h"

using namespace py;
namespace t = tuple;

const object t::type = object((PyObject*) &PyTuple_Type);

t::object::object() : py::object() {}

t::object::object(PyObject *pob) :
    py::object(PyTuple_Check(pob) ? pob : nullptr) {}

t::object::object(const py::object &pob) :
    py::object(PyTuple_Check(pob.ob) ? pob : nullptr) {}

t::object::object(const t::object &cpfrom) : py::object(cpfrom.ob) {}

t::object::object(t::object &&mvfrom) noexcept : py::object(mvfrom.ob) {
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

object t::object::operator[](ssize_t idx) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return PyTuple_GET_ITEM(ob, idx);
}

object t::object::getitem(ssize_t idx) const {
    return this[idx];
}

object t::object::getitem_checked(ssize_t idx) const {
    if (!is_nonnull()) {
        return nullptr;
    }
    PyTuple_GetItem(ob, idx);
}

int t::object::setitem(ssize_t idx, const py::object &value) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    PyTuple_SET_ITEM(ob, idx, value.ob);
    return 0;
}

int t::object::setitem_checked(ssize_t idx, const py::object &value) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyTuple_SetItem(ob, idx, value.ob);
}

t::object::nonnull t::object::as_nonnull() const {
    if (!is_nonnull()) {
        throw pyutils::bad_nonnull();
    }
    return t::object::nonnull(ob);
}

t::object::nonnull::nonnull(PyObject *ob) : t::object(ob) {}
t::object::nonnull::nonnull(const t::object::nonnull &cpfrom) :
    t::object(cpfrom.ob) {}
t::object::nonnull::nonnull(t::object::nonnull &&mvfrom) noexcept :
    t::object(mvfrom.ob) {
    mvfrom.ob = nullptr;
}

t::object::nonnull &t::object::nonnull::operator=(
    const t::object::nonnull &cpfrom) {
    t::object::nonnull tmp(cpfrom);
    return (*this = std::move(tmp));
}

t::object::nonnull &t::object::nonnull::operator=(
    t::object::nonnull &&mvfrom) noexcept{
    ob = mvfrom.ob;
    mvfrom.ob = nullptr;
    return *this;
}

ssize_t t::object::nonnull::len() const {
    return PyTuple_GET_SIZE(ob);
}
