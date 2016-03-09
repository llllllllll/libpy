=========
``libpy``
=========

A C++ framework for working with the CPython API. This is a work in progress.

The general goal of this project is to reduce the boiler plate in CPython
extension modules by building CPython exception handling and reference counting
into the type system. This combined with method chaining and operator
overloading should provide an experience much closer to writing python directly
while still giving us the speed and control of C++.

For example, lets look at the following code in Python and then see how to adapt
this in ``libpy`` and the normal CPython API.

.. code-block:: python

   'ayy.lmao'.find('.') + 1 + 2.5


With ``libpy`` this is almost as expressive as python:

.. code-block:: c++

   py::object f() {
       using py::operator""_p;
       return "ayy.lmao"_p.getattr("find"_p)("."_p) + 1_p + 2.5_p;
   }


Here we see some of the features of ``libpy`` like user defined literals for
python types and operator overloading. We also see that we can write a normal
expression without manually managing reference counts or explicit null checks.
Compare that to the verbosity of the normal CPython API.

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


The ``libpy`` version automatically handles all of the null checking and will
properly forward exceptions. It also manages decrefing the intermediates
even in the case of failures.


Building
--------

``libpy`` is meant to be built as a shared object to be linked against by
extension modules. To build ``libpy.so`` simply run ``make``. This requires a
C++ compiler capable of building C++14 and has only been tested on GCC 5.3.0 on
GNU+Linux.


Tests
-----

The tests live in the ``test`` directory in the project root. These are broken
into seperate files named ``test_*.cc``. The entry point lives in
``test/main.cc``. To build and run the tests run ``make test``.

License
-------

``libpy`` is dual licensed under the terms of the LGPLv3 and the GPLv2.
You make choose to use ``libpy`` under the terms of either of these two
licenses.
