#pragma once
#include <exception>
#include <tuple>
#include <utility>

namespace pyutils {
    template<std::size_t n>
    struct Apply {
        template<typename F, typename T, typename... A>
        static inline auto apply(F &&f, T &&t, A&&... a) {
            return Apply<n - 1>::apply(
                std::forward<F>(f),
                std::forward<T>(t),
                std::get<n - 1>(std::forward<T>(t)), std::forward<A>(a)...);
        }
    };

    template<>
    struct Apply<0> {
        template<typename F, typename T, typename... A>
        static inline auto apply(F &&f, T&&, A&&... a) {
            return ::std::forward<F>(f)(::std::forward<A>(a)...);
        }
    };

    template<typename F, typename T>
    inline auto apply(F &&f, T &&t) {
        return Apply<std::tuple_size<std::decay_t<T>>::value>::apply(
            std::forward<F>(f), std::forward<T>(t));
    }

    template<typename T, std::size_t... Ixs>
    auto _tuple_refs_impl(T &&t, std::index_sequence<Ixs...>) {
        return std::make_tuple(&std::get<Ixs>(std::forward<T>(t))...);
    }

    template<typename T>
    auto tuple_refs(T &&t) {
        return _tuple_refs_impl(
            std::forward<T>(t),
            std::make_index_sequence<
            std::tuple_size<std::remove_reference_t<T>
            >::value>{});
    }

    class bad_nonnull : public std::exception {};

    /**
       This function properly propagates CPython exceptions or raises
       an `AssertionError` if a null check is failed.
    */
    void failed_null_check();

    inline bool all_nonnull() {
        return true;
    }

    template<typename T>
    inline bool all_nonnull(const T &a) {
        return a.is_nonnull();
    }

    template<typename T, typename... Ts>
    inline bool all_nonnull(const T &head, const Ts&... tail) {
        return head.is_nonnull() && all_nonnull(tail...);
    }

    // cast as a function
    template<typename T>
    PyObject *_to_pyobject(const T &a) {
        return (PyObject*) a;
    }

    template<typename T>
    struct is_unsigned {
        static constexpr bool value = false;
    };

    template<>
    struct is_unsigned<unsigned short> {
        static constexpr bool value = true;
    };

    template<>
    struct is_unsigned<unsigned int> {
        static constexpr bool value = true;
    };

    template<>
    struct is_unsigned<unsigned long> {
        static constexpr bool value = true;
    };

    template<>
    struct is_unsigned<unsigned long long> {
        static constexpr bool value = true;
    };

    template<typename T>
    constexpr bool is_unsigned_v = is_unsigned<T>::value;
}
