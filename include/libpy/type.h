#pragma once

#include "libpy/object.h"

namespace py {
    namespace type {
        /**
           A class that represents a python type.

           This is templated on the C++ type of the instances of this type.
           This allows things like `py::tuple::type(iterable)` to return a
           `py::tuple::object`.
        */
        template<typename Instance = py::object>
        class object : public py::object {
        public:
            object() : py::object(nullptr) {}

            object(PyObject *pob) : py::object(pob) {}

            object(const object &cpfrom) : py::object(cpfrom.ob) {}

            object(object &&mvfrom) noexcept : py::object(mvfrom.ob) {
                mvfrom.ob = nullptr;
            }

            object(tmpref<object> &&mvfrom) noexcept : py::object(mvfrom.ob) {
                mvfrom.ob = nullptr;
            }

            template<typename... Ts>
            tmpref<Instance> operator()(const Ts&... args) const {
                py::object ret(py::object::operator()(args...));
                return static_cast<PyObject*>(ret);
            }
        };
    }
}
