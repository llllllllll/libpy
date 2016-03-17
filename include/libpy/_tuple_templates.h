// this header allows us to share these template definiton to break a
// dependency cycle

/**
   Construct a Python `tuple` object from a `std::tuple`.

   @param t The tuple to convert.
   @return  A Python tuple with the same values.
*/
template<typename T>
object from_tuple(const T &t) {
    return pyutils::apply(
        PyTuple_Pack,
        std::tuple_cat(std::make_tuple(std::tuple_size<T>::value),
                       pyutils::apply(pyutils::_to_pyobject, t)));
}

/**
   Pack variadic arguments into a Python `tuple` object.

   @param elems The elements to pack.
   @return      The elements packed as a Python `tuple`.
*/
template<typename... Ts>
tmpref<object> pack(const Ts&... elems) {
    return PyTuple_Pack(sizeof...(Ts), ((PyObject*) elems)...);
}
