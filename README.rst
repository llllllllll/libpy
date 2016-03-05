``libpy``

A C++ framework for working with the CPython API. This is a work in progress.

The general goal of this project is to reduce the boiler plate in CPython
extension modules by building CPython exception handling and reference counting
into the type system. This combined with method chaining and operator
overloading should provide an experience much closer to writing python directly
while still giving us the speed and control of C++.

For example, lets look at the following code in Python and then see how to adapt
this in C and with ``libpy``.

.. code-block:: python

   'ayy.lmao'.find('.') + 1 + 2.5


With normal CPython API.

.. code-block:: c

   PyObject *f() {
       PyObject *al;
       PyObject *dot;
       PyObject *one;
       PyObject *twopfive;
       PyObject *idx;
       PyObject *tmp;
       PyObject *result;

       if (!(al = PyUnicode_FromString("ayy.lmao"))) {
           return NULL;
       }

       if (!(dot = PyUnicode_FromString("."))) {
           Py_DECREF(al);
           return NULL;
       }

       idx = PyObject_CallMethodObjArgs(al, "find", dot, NULL);
       Py_DECREF(al)
       Py_DECREF(dot);

       if (!idx) {
           return NULL;
       }

       if (!(one = PyLong_FromLong(1))) {
           Py_DECREF(idx);
           return NULL;
       }

       tmp = PyNumber_Add(idx, one);
       Py_DECREF(idx);
       Py_DECREF(one);

       if (!tmp) {
           return NULL;
       }

       if (!(twopfive = PyFloat_FromDouble(2.5))) {
           Py_DECREF(tmp);
           return NULL;
       }

       result = PyNumber_Add(tmpa, twopfive);
       Py_DECREF(tmpa);
       Py_DECREF(twopfive);

       result;
   }


With ``libpy`` this is simply:

.. code-block:: c++

   py::object f() {
       using py::operator""_p;
       return "ayy.lmao"_p.getattr("find"_p)("."_p) + 1_p + 2.5_p;
   }
