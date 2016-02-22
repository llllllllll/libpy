#pragma once
#include <tuple>

#include "cpp/object.h"

namespace py{
    namespace tuple {
        /**
           The type of Python `tuple` objects.

           This is equivalent to: `tuple`.
        */
        extern const object type;

        /**
           A subclass of `py::object` for optional tuples.
        */
        class object;
        class object : public py::object {
        public:
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
            object &operator=(const object &cpfrom);
            object &operator=(object &&mvfrom) noexcept;

            /**
               Get the length of the object.

               This is equivalent to `len(this)`.

               @return The length of the object or -1 if an exception occured.
            */
            ssize_t len() const;

            /**
               Get the object at `idx` without bounds checking.

               @param idx The integer index into the tuple.
               @return    The object at index `idx`.
            */
            py::object operator[](ssize_t idx) const;

            /**
               Alias for operator[].
            */
            py::object getitem(ssize_t idx) const;

            /**
               Get the object at `idx` with bounds checking.

               When the index is out of bounds this will return
               `py::object(nullptr)` and set a Python `IndexError`.

               @param idx The integer index into the tuple.
               @return    The object at index `idx` if it is in range.
            */
            py::object getitem_checked(ssize_t idx) const;

            /**
               Sets an object into a tuple. This should only be used to fill
               new tuples.

               This method steals a reference to value.
               This does not do bounds checking.

               @param idx   The integer index into the tuple.
               @param value The element to write.
               @return      zero on success, non-zero on failure.
            */
            int setitem(ssize_t idx, const py::object &value) const;

            /**
               Sets an object into a tuple. This should only be used to fill
               new tuples.

               This method steals a reference to value.
               This method does bounds checking.

               @param idx   The integer index into the tuple.
               @param value The element to write.
               @return      zero on success, non-zero on failure.
            */
            int setitem_checked(ssize_t idx, const py::object &value) const;


            class nonnull;

            /**
               Coerce to a `nonnull` object.

               @see nonnull
               @throws pyutil::bad_nonnull Thrown when `ob == nullptr`.
               @return this converted to a `nonnull` object.
            */
            nonnull as_nonnull() const;
        };


        /**
           A `py::tuple::object` where `ob` is known to be nonnull.
           This is used to skip null checks for performance.

           This class should be used where users want to trade the ability to
           write a nested expression for perfomance.
        */
        class object::nonnull : public object {
        private:
            nonnull() = delete;
            explicit nonnull(PyObject *ob);
            nonnull(const nonnull&);
            nonnull(nonnull&&) noexcept;

        public:
            friend class object;
            nonnull &operator=(const nonnull &cpfrom);
            nonnull &operator=(nonnull &&mvfrom) noexcept;

            /**
               Get the length of the object.

               This is equivalent to `len(this)`.

               @return The length of the object or -1 if an exception occured.
            */
            ssize_t len() const;
        };

        /* pull in the tuple templates after defining `object` in this namespace
           so that `py::tuple::pack` and `py::tuple::from_tuple`` will return
           `py::tuple::object` instead of `py::object`.
        #include "cpp/_tuple_templates.h"

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
            return PyTuple_Check(t.ob);
        }

        inline int check(const object::nonnull &t) {
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
            return PyTuple_CheckExact(t.ob);
        }

        inline int checkexact(const object::nonnull &t) {
            return 1;
        }
    }
}
