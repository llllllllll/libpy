// this header allows us to share these template definiton to break a
// dependency cycle

/**
   Pack variadic arguments into a Python `tuple` object.

   @param elems The elements to pack.
   @return      The elements packed as a Python `tuple`.
*/
template<typename... Ts>
tmpref<object> pack(const Ts&... elems) {
    return PyTuple_Pack(sizeof...(Ts), static_cast<PyObject*>(elems)...);
}
