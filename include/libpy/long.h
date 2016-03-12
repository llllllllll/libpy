#pragma once

#include <type_traits>

#include <libpy/object.h>

namespace py {
    namespace long_ {
        class object;
    }

    /**
       Operator overload for long objects.
    */
    const long_::object &operator""_p(unsigned long long l);

    namespace long_ {

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
               Constructor from C++ numeric types.

               @param l The numeric type to coerce into a python `int`.
            */
            template<typename L,
                     typename = std::enable_if<std::is_arithmetic<L>::value> >
            object(L l) :
                py::object(std::is_integral<L>::value ?
                           PyLong_FromUnsignedLongLong((unsigned long long) l) :
                           PyLong_FromDouble((double) l)) {}

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
