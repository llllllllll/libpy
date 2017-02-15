#pragma once
#include <ostream>
#include <type_traits>

#include <Python.h>

#include "libpy/utils.h"

#define LIBPY_HAVE_MATMUL (PY_VERSION_HEX >= 0x03500000)

/**
   A namespace to hold all of the C++ adapted CPython API types, functions, and
   constants.
*/
namespace py {

typedef Py_hash_t hash_t;
typedef Py_ssize_t ssize_t;

/**
   The comparison operators for `py::object::richcompare`.
*/
enum compareop {
    LT = Py_LT,
    LE = Py_LE,
    EQ = Py_EQ,
    NE = Py_NE,
    GT = Py_GT,
    GE = Py_GE,
};

constexpr int PRINT_RAW = Py_PRINT_RAW;

class object;

// global singletons

/**
   `py::object` representating `None` from Python.
*/
extern const object None;

/**
   `py::object` representating `NotImplemented` from Python.
*/
extern const object NotImplemented;

/**
   `py::object` representating `Ellipsis` or `...` from Python.
*/
extern const object Ellipsis;

/**
   `py::object` representating `True`.
*/
extern const object True;

/**
   `py::object` representating `False`.
*/
extern const object False;

/**
   An object where `ob` is known to be nonnull.
   This is used to skip null checks for performance.

   This class should be used where users want to trade the ability to
   write a nested expression for perfomance.
*/
template<typename T>
class nonnull : public T {
protected:
    nonnull() = delete;
    explicit nonnull(PyObject *pob) : T(pob) {}
    nonnull(const nonnull &cpfrom) : T(cpfrom) {}

public:
    friend T;

    nonnull<T> &operator=(const nonnull &cpfrom) {
        nonnull<T> tmp(cpfrom);
        return (*this = std::move(tmp));
    }

    nonnull<T> &operator=(nonnull &&mvfrom) noexcept {
        this->ob = mvfrom.ob;
        mvfrom.ob = nullptr;
        return *this;
    }

    inline bool is_nonnull() {
        return true;
    }
};

/**
   An object that holds a temprary reference. This reference will be
   decremented when the object goes out of scope.
*/
template<typename T>
class tmpref : public T {
public:
    friend T;

    tmpref() : T(nullptr) {}
    tmpref(PyObject *pob) : T(pob) {}

    // bring in our root type's constructors.
    using T::T;

    /**
       Copy constructor for tmpref which increfs the input to make the
       refcounts check out when both objects are destroyed.

       @param cpfrom The object to copy.
    */
    tmpref(const tmpref &cpfrom) : T(cpfrom.ob) {
        this->incref();
    }

    tmpref(tmpref &&mvfrom) noexcept : T(mvfrom) {
        mvfrom.ob = nullptr;
    }

    tmpref(T &&mvfrom) : T((PyObject*) mvfrom) {
        mvfrom.ob = nullptr;
    }

    using T::operator=;

    tmpref &operator=(const tmpref &cpfrom) {
        this->ob = cpfrom.ob;
        this->incref();
        return *this;
    }

    tmpref &operator=(tmpref &&mvfrom) {
        this->ob = mvfrom.ob;
        mvfrom.ob = nullptr;
        return *this;
    }

    /**
       Move the internal pointer to a new `tmpref`.

       @return A new `tmpref`.
    */
    tmpref as_tmpref() &&{
        return *this;
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

/**
   An object that represents ownership. When a raw object is given to
   this it is increfed to represent the new ownership.
*/
template<typename T>
class ownedref : public tmpref<T> {
public:
    friend T;

    ownedref() : tmpref<T>(nullptr) {}
    ownedref(PyObject *pob) : tmpref<T>(pob) {
        this->incref();
    }

    /**
       Copy constructor for ownedref which increfs the input to make the
       refcounts check out when both objects are destroyed.

       @param cpfrom The object to copy.
    */
    ownedref(const ownedref &cpfrom) : tmpref<T>(cpfrom.ob) {
        this->incref();
    }

    ownedref(ownedref &&mvfrom) noexcept : tmpref<T>(mvfrom.ob) {
        mvfrom.ob = nullptr;
    }

    ownedref(T &&mvfrom) : T(static_cast<PyObject*>(mvfrom)) {
        mvfrom.ob = nullptr;
    }

    using tmpref<T>::operator=;

    ownedref &operator=(const ownedref &cpfrom) {
        this->ob = cpfrom.ob;
        this->incref();
        return *this;
    }

    ownedref &operator=(ownedref &&mvfrom) {
        this->ob = mvfrom.ob;
        mvfrom.ob = nullptr;
        return *this;
    }

    void invalidate() && {
        this->decref();
        tmpref<T>::invalidate();
    }
};

namespace iter {
    template<typename T>
    class iterator;
}

/**
   A wrapper around `PyObject*` to provide a C++ interface to the
   CPython API.
*/
class alignas(PyObject*) object {
protected:
    /**
       The underlying `PyObject*`.
    */
    PyObject *ob;

    template<compareop opid, typename T>
    inline tmpref<object> t_richcompare(const T &other) const {
        // PyObject_RichCompare does its own nullchecks
        return PyObject_RichCompare(ob, other.ob, opid);
    }

    template<int func(PyObject*)>
    inline int int_unary_func() const {
        if (!is_nonnull()) {
            pyutils::failed_null_check();
            return -1;
        }
        return func(ob);
    }

    template<PyObject *func(PyObject*)>
    inline PyObject *ob_unary_func() const {
        if (!is_nonnull()) {
            pyutils::failed_null_check();
            return nullptr;
        }
        return func(ob);
    }

    template<int func(PyObject*, PyObject*), typename T>
    inline int int_binary_func(const T &other) const {
        if (!pyutils::all_nonnull(*this, other)) {
            pyutils::failed_null_check();
            return -1;
        }
        return func(ob, other.ob);
    }

    template<PyObject *func(PyObject*, PyObject*), typename T>
    inline PyObject *ob_binary_func(const T &other) const {
        if (!pyutils::all_nonnull(*this, other)) {
            pyutils::failed_null_check();
            return nullptr;
        }
        return func(ob, other.ob);
    }

public:
    friend const object &operator""_p(char c);
    friend const object &operator""_p(const char *cs, std::size_t len);
    friend const object &operator""_p(wchar_t c);
    friend const object &operator""_p(const wchar_t *cs, std::size_t len);
    friend const object &operator""_p(long double d);
    friend tmpref<object>;

    /**
       Default constructor. The underyling pointer will be nullptr.
     */
    object();

    /**
       Constructor that wraps a given `PyObject*`.

       `ob` will be set the single argument.
    */
    constexpr inline object(PyObject* ob) : ob(ob) {}
    constexpr inline object(const object& cpfrom) : ob(cpfrom.ob) {}
    constexpr inline object(object&& mvfrom) noexcept : ob(mvfrom.ob) {
        mvfrom.ob = nullptr;
    }

    /**
       Constructor that claims the reference from a tmpref.

       @param claimfrom The temporary reference to claim.
    */
    object(tmpref<object> &&mvfrom) noexcept : ob(mvfrom.ob) {
        mvfrom.ob = nullptr;
    }

    object &operator=(const object&);
    object &operator=(object&&) noexcept;

    // object protocol methods

    /**
       Print the underlying object to a file.

       @param f     The file to write to.
       @param flags The flags to use when writing. Right now this must be
                    PRINT_RAW
       @return      zero on success, non-zero on failure. This will set a
                    python exception when it fails.
    */
    int print(FILE *f, int flags = PRINT_RAW) const;

    /**
       Check if the object has a given attribute.

       @param attr The name of the attribute as a string object.
       @return     > 0 if the attribute exists, 0 if the attribute does
       not exist, or < 0 if an exception occured.
    */
    template<typename T>
    int hasattr(const T &attr) const {
        return int_binary_func<PyObject_HasAttr>(attr);
    }

    /** Check if the object has a given attribute.

        This is equivalent to: `hasattr(this, attr)`.

        @see hasattr
        @param attr The name of the attribute as a string object.
        @return     > 0 if the attribute exists, 0 if the attribute does
                    not exist, or < 0 if an exception occured.
    */
    template<typename T>
    tmpref<object> getattr(const T &attr) const {
        return ob_binary_func<PyObject_GetAttr>(attr);
    }

    /**
       Sets an attribute on the object.

       This is equivalent to: `setattr(this, attr, name)`.

       @see delattr
       @param attr  The name of the attribute as a string object.
       @param value The value to set.
       @return      zero on success, non-zero on failure. This will set a
                    python exception if it fails.
    */
    template<typename T>
    int setattr(const T &attr, const object &value) const {
        // value can be nullptr for delattr
        if (!pyutils::all_nonnull(*this, attr)) {
            pyutils::failed_null_check();
            return -1;
        }
        return PyObject_SetAttr(ob, attr.ob, value.ob);
    }

    /**
       Deletes an attribute from the object.

       This is equivalent to: `delattr(this, attr)` in Python or
       `this.setattr(attr, nullptr)` in C++.

       @see setattr
       @param attr The name of the attribute as a string object.
       @return     zero on success, non-zero on failure. This will set a
                   python exception if it fails.
    */
    template<typename T>
    int delattr(const T &attr) const {
        return setattr(attr, nullptr);
    }

    /**
       Compute a string representation of the object.

       This is equivalent to: `repr(this)`.

       @see str
       @see ascii
       @see bytes
       @return The repr of the object.
    */
    tmpref<object> repr() const;

    /**
       Compute the repr of the object and escape non ascii characters.

       This is equivalent to: `ascii(this)`.

       @see repr
       @see str
       @see bytes
       return The ascii representation of the object.
    */
    tmpref<object> ascii() const;

    /**
       Compute a string representation of the object.

       This is equivalent to: `str(this)`.

       @see repr
       @see ascii
       @see bytes
       return The str of the object.
    */
    tmpref<object> str() const;

    /**
       Compute a bytes representation of the object.

       This is equivalent to: `bytes(this)`  when ob is not an integer.
       Unlike Python, this method will raise a Python `TypeError` when this
       object is an integer instead of returning a zero initialized `bytes`
       object.

       @see repr
       @see ascii
       @see str
       @return The bytes representation of the object.
    */
    tmpref<object> bytes() const;

    /**
       Check if the object is a subclass of `cls`

       This is equivalent to: `issubclass(this, cls)`

       @param cls The class to check against.
       @return    > 0 if this is a subclass of `cls`, 0 if this is not
                  a subclass of `cls`, or < 0 if an exception occured.
    */
    template<typename T>
    int issubclass(const T &cls) const {
        return int_binary_func<PyObject_IsSubclass>(cls);
    }

    /**
       Check if the object is an instance of `cls`.

       This is equivalent to: `isinstance(this, cls)`.

       @param cls The class to check against.
       @return    > 0 if this is an instance of `cls`, 0 if this is not
                  an instance of `cls`, or < 0 if an exception occured.
    */
    template<typename T>
    int isinstance(const T &cls) const {
        return int_binary_func<PyObject_IsInstance>(cls);
    }

    /**
      Check if the object is callable.

      This is equivalent to: `callable(this)`.
      When `ob == nullptr` this will return 0.

      @return Can the object be called with `operator()` or `call`.
    */
    bool iscallable() const;

    /**
       Hash the object.

       This is equivalent to: `hash(this)`.

       @return The hash for the object or -1 if an exception occured.
    */
    hash_t hash() const;

    /**
       Check if the object is truthy.

       This is equivalent to: `bool(this)`.

       @return The truthiness of the object.
    */
    int istrue() const;

    /**
      Get the Python type of the object.

      This is equivalent to `type(this)`.

      @return The Python type of the object.
    */
    object type() const;

    /**
       Get the length of the object.

       This is equivalent to `len(this)`.

       @return The length of the object or -1 if an exception occured.
    */
    ssize_t len() const;

    /**
       Get the length of the object.

       This is equivalent to `len(this)`.
       This alias exists to conform to the look more like a C++ container.

       @return The length of the object or -1 if an exception occured.
    */
    inline ssize_t size() const {
        return len();
    }

    /**
       Get a hit at the length of the object. This does not need to match
       the actual length.

       @param fallback The value to use when no hint can be given.
       @return         A hint at the length.
    */
    ssize_t lenhint(ssize_t fallback) const;

    /**
       Get a list of the attributes of the object.

       This is equivalent to: `dir(this)`.

       @return A sorted list of the attributes of the object.
    */
    tmpref<object> dir() const;

    /**
       Get an iterator over the object.

       This is equivalent to `iter(this)`.

       @return An iterator over the object.
    */
    tmpref<object> iter() const;

    /**
       Get the next object out of an iterator.

       This is equivalent to `next(this)`.

       @return The next object in the stream or nullptr.
    */
    tmpref<object> next() const;

    /**
       Constant iterator over a py::object.
    */
    using const_iterator = iter::iterator<object>;

    /**
       For abstract objects we only have const iterators.
    */
    typedef const_iterator iterator;
    friend const_iterator;

    /**
       Get a constant iterator to the begining of an object.

       For abstract objects begin is equivalent to `cbegin` because we
       can only return a const iterator.
       This is an input iterator, it may only be consumed once and can
       only be traversed going forward.

       @return The iterator.
    */
    iterator begin() const;

    /**
       Get the end marker for traversal.

       For abstract objects end is equivalent to `cend` because we can only
       return a const iterator.
       `const_iterator` is an input iterator so it may only be traversed
       a single time going forward.

       @return The end marker.
    */
    iterator end() const;

    /**
       Get a constant iterator to the begining of an object.

       This is an input iterator, it may only be consumed once and can
       only be traversed going forward.

       @return The iterator.
    */
    const_iterator cbegin() const;

    /**
       Get the end marker for traversal.

       `const_iterator` is an input iterator so it may only be traversed
       a single time going forward.

       @return The end marker.
    */
    const_iterator cend() const;

    // relational operators
    /**
       Compare the object to another object where the `opid` is
       programatically deduced.

       This should only be used when we are parameterized on the actual
       comparison type, otherwise the normal comparison operators should
       be prefered.

       @param other The object to compare this one to.
       @param opid  The comparison operation.
       @return      The result of of comparing `this` to `other`
                    with `opid`.
    */
    tmpref<object> richcompare(const object &other, compareop opid) const;

    tmpref<object> operator<(const object&) const;
    tmpref<object> operator<=(const object&) const;
    tmpref<object> operator==(const  object&) const;
    tmpref<object> operator!=(const object&) const;
    tmpref<object> operator>(const object&) const;
    tmpref<object> operator>=(const object&) const;

    /**
       Check if two objects wrap the same underlying PyObject*.

       This is equivalent to: `this is other`.

       @param other The object to compare this to.
       @return Do these object wrap the same PyObject.
    */
    bool is(const object &other) const;

    // numeric operators
    template<typename T>
    tmpref<object> operator+(const T &other) const {
        return ob_binary_func<PyNumber_Add>(other);
    }

    template<typename T>
    tmpref<object> operator-(const T &other) const {
        return ob_binary_func<PyNumber_Subtract>(other);
    }

    template<typename T>
    tmpref<object> operator*(const T &other) const {
        return ob_binary_func<PyNumber_Multiply>(other);
    }

#if LIBPY_HAVE_MATMUL
    template<typename T>
    tmpref<object> matmul(const T &other) const {
        return ob_binary_func<PyNumber_MatrixMultiple>(other);
    }
#endif // CPP_HAVE_MATMUL

    template<typename T>
    tmpref<object> operator/(const T &other) const {
        return ob_binary_func<PyNumber_TrueDivide>(other);
    }

    template<typename T>
    tmpref<object> operator%(const T &other) const {
        return ob_binary_func<PyNumber_Remainder>(other);
    }

    template<typename T>
    tmpref<object> divmod(const T &other) const {
        return ob_binary_func<PyNumber_Divmod>(other);
    }

    template<typename T, typename U>
    tmpref<object> pow(const T &other, const U &mod) const {
        if (!pyutils::all_nonnull(*this, other, mod)) {
            pyutils::failed_null_check();
            return nullptr;
        }
        return PyNumber_Power(ob, other.ob, mod.ob);
    }

    tmpref<object> operator-() const;
    tmpref<object> operator+() const;
    tmpref<object> abs() const;
    tmpref<object> invert() const;

    template<typename T>
    tmpref<object> operator<<(const T &other) const {
        return ob_binary_func<PyNumber_Lshift>(other);
    }

    template<typename T>
    tmpref<object> operator>>(const T &other) const {
        return ob_binary_func<PyNumber_Rshift>(other);
    }

    template<typename T>
    tmpref<object> operator&(const T &other) const {
        return ob_binary_func<PyNumber_And>(other);
    }

    template<typename T>
    tmpref<object> operator^(const T &other) const {
        return ob_binary_func<PyNumber_Xor>(other);
    }

    template<typename T>
    tmpref<object> operator|(const T &other) const {
        return ob_binary_func<PyNumber_Or>(other);
    }

    // indexing
    /**
       Lookup an item in a collection.

       This is equivalent to: `this.getitem(key)`.
       This does not support setitem syntax like: `this[key] = value`.

       @param key The key to lookup.
       @return    The value for the given key.
    */
    template<typename T>
    tmpref<object> operator[](const T &key) const {
        return ob_binary_func<PyObject_GetItem>(key);
    }

    /**
       Lookup an item in a collection.

       This is equivalent to: `this[key]`.

       @param key The key to lookup.
       @return    The value for the given key.
    */
    template<typename T>
    tmpref<object> getitem(const T &key) const {
        return ob_binary_func<PyObject_GetItem>(key);
    }

    /**
       Set the value for some key in a collection.

       This is equivalent to: `this[key] = value`.

       @param key   The key to set.
       @param value The value to set.
       @return      zero on success, non-zero if an exception occured.
    */
    template<typename T>
    int setitem(const T &key, const object &value) const {
        // value.ob can be nullptr for delitem
        if (!pyutils::all_nonnull(*this, key)) {
            pyutils::failed_null_check();
            return -1;
        }
        return PyObject_SetItem(ob, key.ob, value.ob);
    }

    /**
       Delete a key from a collection.

       This is equivalent to: `del this[key]` in Python or
       `this.setitem(key, nullptr)` in C++.

       @param key The key to delete.
       @return    zero on success, non-zero if an exception occured.
    */
    template<typename T>
    int delitem(const T &key) const {
        return setitem(key, nullptr);
    }

    // calling
    // we need the full definition for the class to define this template
    // so it is done after the class definiton in this header
    /**
       Call an object.

       This is equivalent to: `this(a, b, ...)`.

       @param args The arguments to to pass to this.
       @return     The result of calling the object with the given
                   arguments.
    */
    template<typename... Ts>
    tmpref<object> operator()(const Ts&... args) const;

    /**
       Call an object with a tuple of positional arguments and a mapping
       of keyword arguments.

       This is equivalent to: `this(*args, **kwargs)`.

       @param args   The positional arguments.
       @param kwargs The keyword arguments or nullptr if none are given.
       @return       The result of calling the object with the given
                     arguments.
    */
    template<typename T>
    tmpref<object> call(const T &args,
                        const object &kwargs = nullptr) const {

        // kwargs.ob can be nullptr to mean no keyword arguments
        if (!pyutils::all_nonnull(*this, args)) {
            pyutils::failed_null_check();
            return nullptr;
        }
        return PyObject_Call(ob, args.ob, kwargs.ob);
    }

    // refcounting
    /**
       Increment the reference count of the object.

       @return *this.
    */
    const object &incref() const;
    /**
       Decrement the reference count of the object.

       @return *this.
    */
    const object &decref();

    /**
       Decrement the referece count of the object and set the internal
       object pointer to `nullptr`.

       @return *this.
    */
    const object &clear();

    /**
       Get the ref count of the object.
       If the object is `nullptr` then this returns a negative value.

       @return The number of references to the underlying object.
    */
    ssize_t refcnt() const;

    /**
       coersion to PyObject*
    */
    inline operator PyObject *() const {
        return ob;
    }

    /**
       Check if the underyling PyObject* is nonnull.

       @return true if `ob != nullptr` else false.
    */
    inline bool is_nonnull() const {
        return ob;
    }

    /**
       Coerce to a `nonnull` object.

       @see nonnull
       @throws pyutils::bad_nonnull Thrown when `ob == nullptr`.
       @return this converted to a `nonnull` object.
    */
    nonnull<object> as_nonnull() const;

    /**
       Create a temporary reference. This is a reference that will decref
       the object when it is destroyed.

       @return this converted into a tmpref.
    */
    tmpref<object> as_tmpref() &&;
};

namespace _tuple_templates{
#include "libpy/_tuple_templates.h"
}

template<typename... Ts>
tmpref<object> object::operator()(const Ts&... args) const {
    if (!pyutils::all_nonnull(*this, args...)) {
        pyutils::failed_null_check();
        return nullptr;
    }

    auto pyargs = _tuple_templates::pack(args...);

    if (!pyargs.is_nonnull()) {
        return nullptr;
    }
    return PyObject_Call(ob, pyargs.ob, nullptr);
}

namespace iter {
template<typename T>
class iterator :
    public std::iterator<std::input_iterator_tag, T, void> {
private:
    ownedref<object> it;
    tmpref<object> last;

protected:
    iterator(T &&t)
        : it(t), last(std::move(it.next())) {
        if (!last.is_nonnull()) {
            it.ob = nullptr;
        }
    }

public:
    friend T;

    /**
       Default constructor for cend.
    */
    iterator() : it(nullptr), last(nullptr) {}
    iterator(const iterator &t) : it(t.it), last(t.last.incref()) {}
    iterator(iterator &&t)
        : it(std::move(t.it)), last(std::move(t.last)) {}

    iterator &operator=(const iterator &t) {
        it = t.it;
        last = t.last;
        return *this;
    }

    iterator &operator=(iterator &&t) {
        it = std::move(t.it);
        last = std::move(t.last);
        return *this;
    }

    bool operator==(const iterator &other) const {
        return !(static_cast<PyObject*>(it) ||
                 static_cast<PyObject*>(other.it));
    }

    bool operator!=(const iterator &other) const {
        return !(*this == other);
    }

    const object &operator*() const {
        return last;
    }

    const object *operator->() const {
        return &last;
    }

    iterator &operator++() {
        if (it.is_nonnull()) {
            last.decref();
            last = it.next();
            if (!last.is_nonnull()) {
                it.decref();
                it.ob = nullptr;
            }
        }
        return *this;
    }

    iterator operator++(int) {
        return ++*this;
    }
};
}

/**
   Operator overload for unicode objects.
*/

const object &operator""_p(char c);

/**
   Operator overload for unicode objects.
*/
const object &operator""_p(const char *cs, std::size_t len);

/**
   Operator overload for unicode objects.
*/
const object &operator""_p(wchar_t c);

/**
   Operator overload for unicode objects.
*/
const object &operator""_p(const wchar_t *cs, std::size_t len);

/**
   Operator overload for float objects.
*/
const object &operator""_p(long double d);

/**
   ostream writing for objects.

   This writes the `str()` of the object.
   If the object wraps `nullptr` this will write "<NULL>".

   @param stream The output stream to write to.
   @param ob     The object to write.
   @return       The new output stream.
*/
std::ostream &operator<<(std::ostream &stream, const object &ob);
}

namespace pyutils {
template<typename T, bool>
struct _is_nonnull {
    static bool f(const T&) {
        return true;
    }
};

template<typename T>
struct _is_nonnull<T, true> {
    static bool f(const T &t) {
        return t.is_nonnull();
    }
};

template<typename T>
bool is_nonnull(const T &t) {
    return _is_nonnull<T, std::is_base_of<py::object, T>::value>::f(t);
}
}
