#pragma once

#include <type_traits>

#include <libpy/object.h>
#include <libpy/type.h>

namespace py {
namespace long_ {
class object;
}

/**
   Operator overload for long objects.
*/
const long_::object &operator""_p(unsigned long long l);

namespace long_ {

class object;

namespace {
    /**
       Template that selects long_::object if O is long_::object else
       return py::object.

       This will work for subclasses like nonnull or tmpref.
    */
    template<typename O>
    using maybe_long_t = typename std::conditional<
        std::is_base_of<object, O>::value,
        O,
        py::object>::type;
}

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
                   std::is_unsigned<L>::value ?
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

    template<typename T>
    tmpref<maybe_long_t<T>> operator+(const T &other) const {
        return ob_binary_func<PyNumber_Add>(other);
    }

    template<typename T>
    tmpref<maybe_long_t<T>> operator-(const T &other) const {
        return ob_binary_func<PyNumber_Subtract>(other);
    }

    template<typename T>
    tmpref<maybe_long_t<T>> operator*(const T &other) const {
        return ob_binary_func<PyNumber_Multiply>(other);
    }

#if CPP_HAVE_MATMUL
    template<typename T>
    tmpref<maybe_long_t<T>> matmul(const T &other) const {
        return ob_binary_func<PyNumber_MatrixMultiple>(other);
    }
#endif // CPP_HAVE_MATMUL

    // not overriding operator/ because long / long returns a float.

    template<typename T>
    tmpref<maybe_long_t<T>> operator%(const T &other) const {
        return ob_binary_func<PyNumber_Remainder>(other);
    }

    template<typename T>
    tmpref<maybe_long_t<T>> divmod(const T &other) const {
        return ob_binary_func<PyNumber_Divmod>(other);
    }

    template<typename T, typename U>
    tmpref<maybe_long_t<T>> pow(const T &other, const U &mod) const {
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
    tmpref<maybe_long_t<T>> operator<<(const T &other) const {
        return ob_binary_func<PyNumber_Lshift>(other);
    }

    template<typename T>
    tmpref<maybe_long_t<T>> operator>>(const T &other) const {
        return ob_binary_func<PyNumber_Rshift>(other);
    }

    template<typename T>
    tmpref<maybe_long_t<T>> operator&(const T &other) const {
        return ob_binary_func<PyNumber_And>(other);
    }

    template<typename T>
    tmpref<maybe_long_t<T>> operator^(const T &other) const {
        return ob_binary_func<PyNumber_Xor>(other);
    }

    template<typename T>
    tmpref<maybe_long_t<T>> operator|(const T &other) const {
        return ob_binary_func<PyNumber_Or>(other);
    }
};

/**
   The type of Python `long` objects.

   This is equivalent to: `long`.
*/
extern const type::object<long_::object> type;

/**
   Check if an object is an instance of `int`.

   @param t The object to check
   @return  1 if `ob` is an instance of `int`, 0 if `ob` is not an
            instance of `int`, -1 if an exception occured.
*/
template<typename T>
inline int check(const T &t) {
    if (!t.is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyLong_Check(t);
}

inline int check(const nonnull<object>&) {
    return 1;
}

/**
   Check if an object is an instance of `long` but not a subclass.

   @param t The object to check
   @return  1 if `ob` is an instance of `long`, 0 if `ob` is not an
            instance of `long`, -1 if an exception occured.
*/
template<typename T>
inline int checkexact(const T &t) {
    if (!t.is_nonnull()) {
        pyutils::failed_null_check();
        return -1;
    }
    return PyLong_CheckExact(t);
}

inline int checkexact(const nonnull<object>&) {
    return 1;
}
}
}

namespace pyutils {
template<typename T>
struct typeformat;

template<>
struct typeformat<py::long_::object> {
    static char_sequence<'O'> cs;

    template<typename T>
    static inline auto make_arg(T &&t) {
        return std::make_tuple(&PyLong_Type, std::forward<T>(t));
    }
};
}
