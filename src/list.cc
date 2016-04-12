#include "libpy/list.h"
#include "libpy/utils.h"

using namespace py;
namespace l = list;

const type::object<l::object> l::type((PyObject*) &PyList_Type);

l::object::object() : py::object() {}

l::object::object(int len) : py::object(PyList_New(len)) {}

l::object::object(std::size_t len) : py::object(PyList_New(len)) {}

l::object::object(ssize_t len) : py::object(PyList_New(len)) {}

l::object::object(PyObject *pob) : py::object(pob) {
    list_check();
}

l::object::object(const py::object &pob) : py::object(pob) {
    list_check();
}

l::object::object(const l::object &cpfrom) : py::object((PyObject*) cpfrom) {}

l::object::object(l::object &&mvfrom) noexcept :
    py::object((PyObject*) mvfrom) {
    mvfrom.ob = nullptr;
}

void l::object::list_check() {
    if (ob && !PyList_Check(ob)) {
        ob = nullptr;
        if (!PyErr_Occurred()) {
            PyErr_SetString(PyExc_TypeError,
                            "cannot make py::lisl::object from non list");
        }
    }
}

l::object::const_iterator l::object::cbegin() const {
    if (!is_nonnull()) {
        return nullptr;
    }
    // it is safe to cast a PyObject* to a py::object because it has standard
    // layout and only a single field
    return (const py::object*) &((PyListObject*) ob)->ob_item[0];
}

l::object::const_iterator l::object::cend() const {
    if (!is_nonnull()) {
        return nullptr;
    }

    // it is safe to cast a PyObject* to a py::object because it has standard
    // layout and only a single field
    return (const py::object*) &((PyListObject*) ob)->ob_item[Py_SIZE(ob)];
}

l::object::iterator l::object::begin() const {;
    return cbegin();
}

l::object::iterator l::object::end() const {
    return cend();
}

ssize_t l::object::len() const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyList_GET_SIZE(ob);
}

py::object l::object::operator[](int idx) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return PyList_GET_ITEM(ob, idx);
}

py::object l::object::operator[](ssize_t idx) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return PyList_GET_ITEM(ob, idx);
}

py::object l::object::operator[](std::size_t idx) const {
    if (!is_nonnull()) {
        pyutils::failed_null_check();
        return nullptr;
    }
    return PyList_GET_ITEM(ob, idx);
}


nonnull<l::object> l::object::as_nonnull() const {
    if (!is_nonnull()) {
        throw pyutils::bad_nonnull();
    }
    return nonnull<l::object>(ob);

}

tmpref<l::object> l::object::as_tmpref() && {
    tmpref<l::object> ret(ob);
    ob = nullptr;
    return std::move(ret);
}
