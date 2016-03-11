#pragma once

#include <libpy/object.h>

namespace py {
    namespace plong {
        class object;
    }

    /**
       Operator overload for long objects.
    */
    const plong::object &operator""_p(unsigned long long l);

    namespace plong {

        class object : public py::object {
        private:
            /**
               Function called to verify that `ob` is a long and
               correctly raise a python exception otherwies.
            */
            void long_check();

            template<typename T, T func(PyObject*)>
            inline T as_t() const {
                if (!is_nonnull()) {
                    pyutils::failed_null_check();
                    return -1;
                }
                return func(ob);
            }

            template<typename T, T func(PyObject*, int*)>
            inline T as_t_and_overflow(int &overflow) const {
                if (!is_nonnull()) {
                    pyutils::failed_null_check();
                    return -1;
                }
                return func(ob, &overflow);
            }
        public:
            /**
               Default constructor. This will set `ob` to nullptr.
            */
            object();

            /**
               Explicit constructor from C++ longs.

               @param l The long to coerce into a PyLongObject.
            */
            explicit object(long l);
            explicit object(unsigned long l);
            explicit object(long long l);
            explicit object(unsigned long long l);
            explicit object(double l);

            /**
               Constructor from `PyObject*`. If `pob` is not a `tuple` then
               `ob` will be set to `nullptr`.
            */
            object(PyObject *pob);

            /**
               Constructor from `py::object`. If `pob` is not a `tuple` then
               `ob` will be set to `nullptr`.
            */
            object(const py::object &pob);

            object(const object &cpfrom);
            object(object &&mvfrom) noexcept;

            using py::object::operator=;


            long as_long() const;
            long as_long_and_overflow(int &overflow) const;
            long long as_long_long() const;
            long long as_long_long_and_overflow(int &overflow) const;
            ssize_t as_ssize_t() const;
            unsigned long as_unsigned_long() const;
            std::size_t as_size_t() const;
            unsigned long long as_unsigned_long_long() const;
            double as_double() const;

            friend const object &py::operator""_p(unsigned long long l);
        };
    }
}
