#pragma once
#include <tuple>

#include "libpy/object.h"
#include "libpy/type.h"

namespace py{
    namespace tuple {
        /**
           A subclass of `py::object` for optional tuples.
        */
        class object : public py::object {
        private:
            /**
               Function called to verify that `ob` is a tuple and
               correctly raise a python exception otherwies.
            */
            void tuple_check();
        protected:
            /**
               Convert a nonnull tuple into a raw C array of objects.
            */
            inline py::object *as_array() const {
            return reinterpret_cast<py::object*>(
                reinterpret_cast<PyTupleObject* const>(ob)->ob_item);
            }
        public:
            friend class py::tmpref<object>;

            /**
               Default constructor. This will set `ob` to nullptr.
            */
            object();

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

            explicit object(size_t len);
            explicit object(py::ssize_t len);

            using py::object::operator=;

            /**
               `PyTupleObject`s are actually backed by a C array of `PyObject*`s
               so we can just use a `py::object*` which points into that
               storage.
            */
            typedef const py::object* const_iterator;
            typedef const_iterator iterator;

            const_iterator cbegin() const;
            const_iterator cend() const;
            iterator begin() const;
            iterator end() const;


            /**
               Get the length of the object.

               This is equivalent to `len(this)`.

               @return The length of the object or -1 if an exception occured.
            */
            ssize_t len() const;

            using py::object::operator[];

            /**
               Get the object at `idx` without bounds checking.

               @param idx The integer index into the tuple.
               @return    The object at index `idx`.
            */
            // this is not a template because it is ambigious with the template
            // defined in the base class
            py::object operator[](int idx) const;
            py::object operator[](ssize_t idx) const;
            py::object operator[](std::size_t idx) const;


            /**
               Alias for operator[].
            */
            template<typename I,
                     typename = std::enable_if_t<std::is_integral<I>::value>>
            py::object getitem(I idx) const {
                return *this[idx];
            }

            /**
               Get the object at `idx` with bounds checking.

               When the index is out of bounds this will return
               `py::object(nullptr)` and set a Python `IndexError`.

               @param idx The integer index into the tuple.
               @return    The object at index `idx` if it is in range.
            */
            template<typename I,
                     typename = std::enable_if_t<std::is_integral<I>::value>>
            py::object getitem_checked(I idx) const {
                if (!is_nonnull()) {
                    return nullptr;
                }
                return PyTuple_GetItem(ob, idx);
            }

            /**
               Sets an object into a tuple. This should only be used to fill
               new tuples.

               This method steals a reference to value.
               This does not do bounds checking.

               @param idx   The integer index into the tuple.
               @param value The element to write.
               @return      zero on success, non-zero on failure.
            */
            template<typename I,
                     typename = std::enable_if_t<std::is_integral<I>::value>>
            int setitem(ssize_t idx, const py::object &value) const {
                if (!is_nonnull()) {
                    pyutils::failed_null_check();
                    return -1;
                }
                PyTuple_SET_ITEM(ob, idx, value);
                return 0;
            }

            /**
               Sets an object into a tuple. This should only be used to fill
               new tuples.

               This method steals a reference to value.
               This method does bounds checking.

               @param idx   The integer index into the tuple.
               @param value The element to write.
               @return      zero on success, non-zero on failure.
            */
            template<typename I,
                     typename = std::enable_if_t<std::is_integral<I>::value>>
            int setitem_checked(I idx, const py::object &value) const {
                if (!is_nonnull()) {
                    pyutils::failed_null_check();
                    return -1;
                }
                return PyTuple_SetItem(ob, idx, value);
            }

            /**
               Coerce to a `nonnull` object.

               @see nonnull
               @throws pyutil::bad_nonnull Thrown when `ob == nullptr`.
               @return this converted to a `nonnull` object.
            */
            nonnull<object> as_nonnull() const;

            /**
               Create a temporary reference. This is a reference that will
               decref the object when it is destroyed.

               @return this converted into a tmpref.
            */
            tmpref<object> as_tmpref() &&;
        };

        /**
           The type of Python `tuple` objects.

           This is equivalent to: `tuple`.
        */
        extern const type::object<tuple::object> type;

        /* pull in the tuple templates after defining `object` in this namespace
           so that `py::tuple::pack` and `py::tuple::from_tuple`` will return
           `py::tuple::object` instead of `py::object`.
        */
        #include "libpy/_tuple_templates.h"

        /**
           Check if an object is an instance of `tuple`.

           @param t The object to check
           @return  1 if `ob` is an instance of `tuple`, 0 if `ob` is not an
                    instance of `tuple`, -1 if an exception occured.
        */
        template<typename T>
        inline int check(const T &t) {
            if (!t.is_nonnull()) {
                pyutils::failed_null_check();
                return -1;
            }
            return PyTuple_Check(t);
        }

        inline int check(const nonnull<object>&) {
            return 1;
        }

        /**
           Check if an object is an instance of `tuple` but not a subclass.

           @param t The object to check
           @return  1 if `ob` is an instance of `tuple`, 0 if `ob` is not an
                    instance of `tuple`, -1 if an exception occured.
        */
        template<typename T>
        inline int checkexact(const T &t) {
            if (!t.is_nonnull()) {
                pyutils::failed_null_check();
                return -1;
            }
            return PyTuple_CheckExact(t);
        }

        inline int checkexact(const nonnull<object>&) {
            return 1;
        }
    }

    /**
       A `py::tuple::object` where `ob` is known to be nonnull.
       This is used to skip null checks for performance.

       This class should be used where users want to trade the ability to
       write a nested expression for perfomance.
    */
    template<>
    class nonnull<tuple::object> : public tuple::object {
    protected:
        nonnull() = delete;
        explicit nonnull(PyObject *ob) : tuple::object(ob) {}

    public:
        friend class object;

        nonnull(const nonnull &cpfrom) : tuple::object(cpfrom) {}
        nonnull(nonnull &&mvfrom) noexcept : tuple::object(mvfrom.ob) {
            mvfrom.ob = nullptr;
        }

        nonnull &operator=(const nonnull &cpfrom) {
            nonnull<tuple::object> tmp(cpfrom);
            return (*this = std::move(tmp));
        }

        nonnull &operator=(nonnull &&mvfrom) noexcept {
            ob = mvfrom.ob;
            mvfrom.ob = nullptr;
            return *this;
        }

        /**
           Get the length of the object.

           This is equivalent to `len(this)`.

           @return The length of the object or -1 if an exception occured.
        */
        ssize_t len() const {
            return PyTuple_GET_SIZE(ob);
        }
    };
}


namespace pyutils {
    template<typename T>
    struct typeformat;

    template<>
    struct typeformat<py::tuple::object> {
        static char_sequence<'O'> cs;

        template<typename T>
        static inline auto make_arg(T &&t) {
            return std::make_tuple(&PyTuple_Type, std::forward<T>(t));
        }
    };
}
