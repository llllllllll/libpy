#pragma once
#include <tuple>

#include "libpy/object.h"
#include "libpy/type.h"

namespace py{
    namespace list {
        /**
           A subclass of `py::object` for optional lists.
        */
        class object : public py::object {
        private:
            /**
               Function called to verify that `ob` is a list and
               correctly raise a python exception otherwies.
            */
            void list_check();
        protected:
            /**
               Convert a nonnull list into a raw C array of objects.
            */
            inline py::object *as_array() const {
            return reinterpret_cast<py::object*>(
                reinterpret_cast<PyListObject* const>(ob)->ob_item);
        }
        public:
            friend class py::tmpref<object>;

            /**
               Default constructor. This will set `ob` to nullptr.
            */
            object();

            /**
               Constructor from `size_t` and `ssize_t`, these allocate new
               lists of the given length.
            */
            object(int len);
            object(std::size_t len);
            object(py::ssize_t len);

            /**
               Constructor from `PyObject*`. If `pob` is not a `list` then
               `ob` will be set to `nullptr`.
            */
            object(PyObject *pob);

            /**
               Constructor from `py::object`. If `pob` is not a `list` then
               `ob` will be set to `nullptr`.
            */
            object(const py::object &pob);

            object(const object &cpfrom);
            object(object &&mvfrom) noexcept;

            using py::object::operator=;

            /**
               `PyListObject`s are actually backed by a C array of `PyObject*`s
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
            py::ssize_t len() const;

            using py::object::operator[];

            /**
               Get the object at `idx` without bounds checking.

               @param idx The integer index into the list.
               @return    The object at index `idx`.
            */
            // this is not a template because it is ambigious with the template
            // defined in the base class
            py::object operator[](int idx) const;
            py::object operator[](py::ssize_t idx) const;
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

               @param idx The integer index into the list.
               @return    The object at index `idx` if it is in range.
            */
            template<typename I,
                     typename = std::enable_if_t<std::is_integral<I>::value>>
            py::object getitem_checked(I idx) const {
                if (!is_nonnull()) {
                    return nullptr;
                }
                return PyList_GetItem(ob, idx);
            }

            /**
               Sets an object into a list. This should only be used to fill
               new lists.

               This method steals a reference to value.
               This does not do bounds checking.

               @param idx   The integer index into the list.
               @param value The element to write.
               @return      zero on success, non-zero on failure.
            */
            template<typename I,
                     typename = std::enable_if_t<std::is_integral<I>::value>>
            int setitem(py::ssize_t idx, const py::object &value) const {
                if (!is_nonnull()) {
                    pyutils::failed_null_check();
                    return -1;
                }
                PyList_SET_ITEM(ob, idx, value);
                return 0;
            }

            /**
               Sets an object into a list. This should only be used to fill
               new lists.

               This method steals a reference to value.
               This method does bounds checking.

               @param idx   The integer index into the list.
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
                return PyList_SetItem(ob, idx, value);
            }

            /**
               Append an element to a list.

               @param elem The element to append.
               @return -1 on failure, otherwise zero.
            */
            template<typename T>
            int append(const T &elem) {
                if (!pyutils::all_nonnull(*this, elem)) {
                    pyutils::failed_null_check();
                    return -1;
                }
                return PyList_Append(*this, elem);
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
           The type of Python `list` objects.

           This is equivalent to: `list`.
        */
        extern const type::object<list::object> type;

        /**
           Check if an object is an instance of `list`.

           @param t The object to check
           @return  1 if `ob` is an instance of `list`, 0 if `ob` is not an
                    instance of `list`, -1 if an exception occured.
        */
        template<typename T>
        inline int check(const T &t) {
            if (!t.is_nonnull()) {
                pyutils::failed_null_check();
                return -1;
            }
            return PyList_Check(t);
        }

        inline int check(const nonnull<object>&) {
            return 1;
        }

        /**
           Check if an object is an instance of `list` but not a subclass.

           @param t The object to check
           @return  1 if `ob` is an instance of `list`, 0 if `ob` is not an
                    instance of `list`, -1 if an exception occured.
        */
        template<typename T>
        inline int checkexact(const T &t) {
            if (!t.is_nonnull()) {
                pyutils::failed_null_check();
                return -1;
            }
            return PyList_CheckExact(t);
        }

        inline int checkexact(const nonnull<object>&) {
            return 1;
        }
    }

    /**
       A `py::list::object` where `ob` is known to be nonnull.
       This is used to skip null checks for performance.

       This class should be used where users want to trade the ability to
       write a nested expression for perfomance.
    */
    template<>
    class nonnull<list::object> : public list::object {
    protected:
        nonnull() = delete;
        explicit nonnull(PyObject *ob) : list::object(ob) {}
    public:
        friend class object;

        nonnull(const nonnull &cpfrom) : list::object(cpfrom) {}
        nonnull(nonnull &&mvfrom) noexcept : list::object(mvfrom.ob) {
            mvfrom.ob = nullptr;
        }

        nonnull &operator=(const nonnull &cpfrom) {
            nonnull<list::object> tmp(cpfrom);
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
        py::ssize_t len() const {
            return PyList_GET_SIZE(ob);
        }

        /**
           Get the object at `idx` without bounds checking.

           If assignment is done to this value it is done without ref
           counting or bounds checking, basically PyList_SET_ITEM.

           @param idx The integer index into the list.
           @return    A reference to object at index `idx`.
        */
        // this is not a template because it is ambigious with the template
        // defined in the base class
        py::object &operator[](int idx) const {
            return as_array()[idx];
        }

        py::object &operator[](py::ssize_t idx) const {
            return as_array()[idx];
        }

        py::object &operator[](std::size_t idx) const {
            return as_array()[idx];
        }
    };

    namespace list {
        /**
           Pack variadic arguments into a Python `list` object.

           @param elems The elements to pack.
           @return      The elements packed as a Python `list`.
        */
        template<typename... Ts>
        tmpref<object> pack(const Ts&... elems) {
            object l(sizeof...(elems));

            if (!l.is_nonnull()) {
                return nullptr;
            }

            py::nonnull<object> m = l.as_nonnull();
            std::size_t n = 0;
            for (const py::object &elem : { elems... }) {
                m[n++] = elem;
            }
            return std::move(l);
        }
    }
}

namespace pyutils {
    template<typename T>
    struct typeformat;

    template<>
    struct typeformat<py::list::object> {
        static char_sequence<'O', '!'> cs;

        template<typename T>
        static inline auto make_arg(T &&t) {
            return std::make_tuple(&PyList_Type, std::forward<T>(t));
        }
    };
}
