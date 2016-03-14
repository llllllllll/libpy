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
            friend tmpref<object>;
            friend const object &py::operator""_p(unsigned long long l);

            /**
               Default constructor. This will set `ob` to nullptr.
            */
            object();

            /**
               Constructor from C++ numeric types.

               This constructor is explicit because the user must manually
               decref the object. If an expression was implicitly upcast to
               long_::object there could be a leak.

               @param l The numeric type to coerce into a python `int`.
            */
            template<typename L,
                     typename = std::enable_if_t<std::is_arithmetic<L>::value>>
            explicit object(L l) :
                py::object(!std::is_integral<L>::value ?
                           PyLong_FromDouble(l) :
                           pyutils::is_unsigned_v<L> ?
                           PyLong_FromUnsignedLongLong(l) :
                           PyLong_FromLongLong(l)) {}

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

            nonnull<object> as_nonnull() const;
            tmpref<object> as_tmpref() &&;
        };
    }

    template<>
    class tmpref<long_::object> : public long_::object {
    public:
        friend long_::object;

        tmpref() : long_::object(nullptr) {}
        tmpref(PyObject *pob) : long_::object(pob) {}

        /**
           Copy constructor for tmpref which increfs the input to make the
           refcounts check out when both objects are destroyed.

           @param cpfrom long_::objecthe object to copy.
        */
        tmpref(const tmpref<long_::object> &cpfrom) : long_::object(cpfrom.ob) {
            this->incref();
        }

        tmpref(tmpref<long_::object> &&mvfrom) noexcept
            : long_::object(mvfrom) {
            std::move(mvfrom).invalidate();
        }

        tmpref(tmpref<py::object> &&mvfrom) noexcept
            : long_::object((PyObject*) mvfrom) {
            std::move(mvfrom).invalidate();
        }

        tmpref(long_::object &&mvfrom) : long_::object((PyObject*) mvfrom) {
            mvfrom.ob = nullptr;
        }

        /**
           Constructor from C++ numeric types.

           This constructor is not explicit because the tmpref will properly
           clean itself up if used in an expression.

           @param l The numeric type to coerce into a python `int`.
        */
        template<typename L,
                 typename = std::enable_if_t<std::is_arithmetic<L>::value>>
        tmpref(L l) :
            tmpref<object>(!std::is_integral<L>::value ?
                           PyLong_FromDouble(l) :
                           pyutils::is_unsigned_v<L> ?
                           PyLong_FromUnsignedLongLong(l) :
                           PyLong_FromLongLong(l)) {}

        using long_::object::operator=;

        tmpref &operator=(const tmpref<long_::object> &cpfrom) {
            this->ob = cpfrom.ob;
            this->incref();
            return *this;
        }

        tmpref &operator=(tmpref<long_::object> &&mvfrom) {
            this->ob = mvfrom.ob;
            mvfrom.ob = nullptr;
            return *this;
        }

        tmpref<long_::object> as_tmpref() &&{
            tmpref<long_::object> ret(this->ob);
            this->ob = nullptr;
            return ret;
        }

        /**
           Invalidate a tmpref, setting the internal object to nullptr.
        */
        void invalidate() && {
            this->ob = nullptr;
        }

        ~tmpref() {
            this->decref();
        }

    };
}
