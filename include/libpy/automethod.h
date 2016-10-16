#pragma once
#include <array>
#include <cstdint>
#include <tuple>

#include <Python.h>

#include "libpy/object.h"
#include "libpy/utils.h"

namespace pyutils {
/**
   The format character for the given type. The default case is left
   unitialized to generate a compile-time error if you attempt to use
   auto method on a type that has no format character.
*/
template<typename T>
struct typeformat {};

struct _default_make_arg {
    template<typename T>
    static inline auto make_arg(T &&t) {
        return std::make_tuple(std::forward<T>(t));
    }
};

template<>
struct typeformat<const char*> : public _default_make_arg {
    static char_sequence<'z'> cs;
};

template<>
struct typeformat<Py_buffer> : public _default_make_arg {
    static char_sequence<'s'> cs;
};

template<>
struct typeformat<char> : public _default_make_arg {
    static char_sequence<'c'> cs;
};

template<>
struct typeformat<unsigned char> : public _default_make_arg {
    static char_sequence<'b'> cs;
};

template<>
struct typeformat<short> : public _default_make_arg {
    static char_sequence<'h'> cs;
};

template<>
struct typeformat<unsigned short> : public _default_make_arg {
    static char_sequence<'H'> cs;
};

template<>
struct typeformat<int> : public _default_make_arg {
    static char_sequence<'i'> cs;
};

template<>
struct typeformat<unsigned int> : public _default_make_arg {
    static char_sequence<'I'> cs;
};

template<>
struct typeformat<long> : public _default_make_arg {
    static char_sequence<'l'> cs;
};

template<>
struct typeformat<unsigned long> : public _default_make_arg {
    static char_sequence<'k'> cs;
};

template<>
struct typeformat<long long> : public _default_make_arg {
    static char_sequence<'L'> cs;
};

template<>
struct typeformat<unsigned long long> : public _default_make_arg {
    static char_sequence<'K'> cs;
};

template<>
struct typeformat<float> : public _default_make_arg {
    static char_sequence<'f'> cs;
};

template<>
struct typeformat<double> : public _default_make_arg {
    static char_sequence<'d'> cs;
};

template<>
struct typeformat<Py_complex> : public _default_make_arg {
    static char_sequence<'D'> cs;
};

template<>
struct typeformat<PyObject*> : public _default_make_arg {
    static char_sequence<'O'> cs;
};

template<>
struct typeformat<py::object> : public _default_make_arg {
    static char_sequence<'O'> cs;
};

template<>
struct typeformat<bool> : public _default_make_arg {
    static char_sequence<'p'> cs;
};

/**
   Struct for extracting traits about the function being wrapped.
*/
template<typename F>
struct _function_traits;

template<typename R, typename Self, typename... Args>
struct _function_traits<R(Self, Args...)> {
    static_assert(sizeof(Self) == sizeof(py::object),
                  "self argument is the incorrect size");
    using return_type = R;
    using parsed_args_type = std::tuple<Args...>;

    static constexpr std::size_t arity = sizeof...(Args);
    static constexpr auto flags = arity ? METH_VARARGS : METH_NOARGS;

    static inline auto fmtstr() {
        return char_sequence_to_array(
            char_sequence_cat(typeformat<Args>::cs...));
    }

    template<typename T>
    static inline auto make_args(T &&t) {
        return make_args_impl(std::forward<T>(t),
                              std::index_sequence_for<Args...>{});
    }

private:
    template<typename T, std::size_t... ns>
    static inline auto make_args_impl(T &&t, std::index_sequence<ns...>) {
        return std::tuple_cat(
            typeformat<Args>::make_arg(std::get<ns>(t))...);
    }
};

/**
   Struct which provides a single function `f` which is the actual
   implementation of `_automethod_wrapper` to use. This is implemented
   as a struct to allow for partial template specialization to optimize
   for the `METH_NOARGS` case.
*/
template<std::size_t arity, typename F, const F &impl>
struct _automethodwrapper_impl {
    static inline PyObject *f(PyObject *self, PyObject *args) {
        using f = _function_traits<F>;
        typename f::parsed_args_type parsed_args;

        if (!apply(PyArg_ParseTuple,
                   std::tuple_cat(std::make_tuple(args, f::fmtstr().data()),
                                  f::make_args(tuple_refs(parsed_args))))) {
            return nullptr;
        }
        return apply(impl, std::tuple_cat(std::make_tuple(self), parsed_args));
    }
};

/**
   `METH_NOARGS` handler for `_automethodwrapper_impl`, hit when
   `arity == 0`.
*/
template<typename F, const F &impl>
struct _automethodwrapper_impl<0, F, impl> {
    static inline PyObject *f(PyObject *self, PyObject *args) {
        return impl(self);
    }
};

/**
   The actual funtion that will be registered with the automatically
   created PyMethodDef. This has the signature expected for a python
   function and will handle unpacking the arguments.

   @param self The module or instance this is a method of.
   @param args The arguments to the method as a `PyTupleObject*`.
   @return     The result of calling our method.
*/
template<typename F, const F &impl>
PyObject *_automethodwrapper(PyObject *self, PyObject *args) {
    return _automethodwrapper_impl<_function_traits<F>::arity,
                                   F,
                                   impl>::f(self, args);
}

#define _libpy_automethod_def(name, f, doc)  (PyMethodDef {             \
        name,                                                           \
        (PyCFunction) pyutils::_automethodwrapper<decltype(f), f>,      \
        pyutils::_function_traits<decltype(f)>::flags,                  \
        doc,                                                            \
    })

#define _libpy_automethod_2(f, doc) (PyMethodDef {                      \
        #f,                                                             \
        (PyCFunction) pyutils::_automethodwrapper<decltype(f), f>,      \
        pyutils::_function_traits<decltype(f)>::flags,                  \
        doc,                                                            \
    })
#define _libpy_automethod_1(f) _libpy_automethod_2(f, nullptr)
#define _libpy_automethod_dispatch(n, f, doc, macro, ...)  macro

#define _libpy_named_automethod_3(name, f, doc) (PyMethodDef {          \
        name,                                                           \
        (PyCFunction) pyutils::_automethodwrapper<decltype(f), f>,      \
        METH_VARARGS,                                                   \
        doc,                                                            \
    })
#define _libpy_named_automethod_2(name, f) _libpy_automethod_2(name, f, nullptr)
#define _libpy_named_automethod_dispatch(n, f, doc, macro, ...)  macro

    /**
       Wrap a C++ function as a python `PyMethodDef` structure.

       @param func The function to wrap.
       @param doc  The docstring to use for the function. If this is omitted
                   the docstring will `be None`.
       @return     A `PyMethodDef` structure for the given function.
    */
#define automethod(...)                                                 \
    _libpy_automethod_dispatch(,##__VA_ARGS__,                          \
                               _libpy_automethod_2(__VA_ARGS__),        \
                               _libpy_automethod_1(__VA_ARGS__))

    /**
       Wrap a C++ function as a python PyMethodDef structure but give the
       python function an explicit name.

       This is useful when the function being wrapped is defined in some
       namespace so that the python name does not have the `::` in it.

       @param name The name for the function as it will be seen from python.
       @param func The function to wrap.
       @param doc  The docstring to use for the function. If this is omitted
                   the docstring will be `None`.
       @return     A `PyMethodDef` structure for the given function.
    */
#define named_automethod(...)                                           \
    _libpy_named_automethod_dispatch(name,##__VA_ARGS__,                \
                                     _libpy_named_automethod_3(__VA_ARGS__),  \
                                     _libpy_named_automethod_3(__VA_ARGS__))
}
