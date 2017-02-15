#pragma once
#include <exception>
#include <tuple>
#include <utility>

#include <Python.h>

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
        return std::forward<F>(f)(std::forward<A>(a)...);
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

/**
   Check if all the inputs not nullptr or py::objects that wrap a nullptr.

   For empty argument lists this is just `true`.

   @return true
*/
inline bool all_nonnull() {
    return true;
}

/**
   Check if all the inputs not nullptr or py::objects that wrap a nullptr.

   @return Is the single argument nonnull.
*/
template<typename T>
inline bool all_nonnull(const T &a) {
    return a.is_nonnull();
}

/**
   Check if all the inputs not nullptr or py::objects that wrap a nullptr.

   @return Are all of the arguments nonnull.
*/
template<typename T, typename... Ts>
inline bool all_nonnull(const T &head, const Ts&... tail) {
    return head.is_nonnull() && all_nonnull(tail...);
}

template<char... cs>
using char_sequence = std::integer_sequence<char, cs...>;

template<char... cs, char... ds>
constexpr auto _char_sequence_cat(char_sequence<cs...>,
                                  char_sequence<ds...>) {
    return char_sequence<cs..., ds...>{};
}

constexpr char_sequence<> char_sequence_cat() {
    return char_sequence<>{};
}

template<typename Cs>
constexpr auto char_sequence_cat(Cs cs) {
    return cs;
}

template<typename Cs, typename Ds>
constexpr auto char_sequence_cat(Cs cs, Ds ds) {
    return _char_sequence_cat(cs, ds);
}

template<typename Cs, typename Ds, typename... Ts>
constexpr auto char_sequence_cat(Cs cs, Ds ds, Ts... es) {
    return _char_sequence_cat(_char_sequence_cat(cs, ds),
                              char_sequence_cat(es...));
}

template<char... cs>
constexpr auto char_sequence_to_array(char_sequence<cs...>) {
    return std::array<char, sizeof...(cs) + 1> {cs..., '\0'};
}
}
