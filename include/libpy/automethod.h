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
    constexpr char typeformat;

    template<>
    constexpr char typeformat<const char*> = 'z';

    template<>
    constexpr char typeformat<Py_buffer> = 's';

    template<>
    constexpr char typeformat<char> = 'c';

    template<>
    constexpr char typeformat<unsigned char> = 'b';

    template<>
    constexpr char typeformat<short> = 'h';

    template<>
    constexpr char typeformat<unsigned short> = 'H';

    template<>
    constexpr char typeformat<int> = 'i';

    template<>
    constexpr char typeformat<unsigned int> = 'I';

    template<>
    constexpr char typeformat<long> = 'l';

    template<>
    constexpr char typeformat<unsigned long> = 'k';

    template<>
    constexpr char typeformat<long long> = 'L';

    template<>
    constexpr char typeformat<unsigned long long> = 'K';

    template<>
    constexpr char typeformat<float> = 'f';

    template<>
    constexpr char typeformat<double> = 'd';

    template<>
    constexpr char typeformat<Py_complex> = 'D';

    template<>
    constexpr char typeformat<PyObject*> = 'O';

    template<>
    constexpr char typeformat<py::object> = 'O';

    template<>
    constexpr char typeformat<bool> = 'p';

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
            return std::array<char, arity + 1> {typeformat<Args>..., '\0'};
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
                                      tuple_refs(parsed_args)))) {
                return nullptr;
            }
            return apply(impl,
                         std::tuple_cat(std::make_tuple(self), parsed_args));
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
