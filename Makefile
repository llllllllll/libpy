CC=g++
MAJOR_VERSION=1
MINOR_VERSION=0
MICRO_VERSION=0
CFLAGS=-std=gnu++14 -Wall -Wextra -O3
LDFLAGS=
SOURCES=$(wildcard src/*.cc)
PYTHON_INCLUDE=$(shell python -c 'import distutils as d;print(d.sysconfig_get_python_inc())')
PYTHON_LDVERSION=$(shell python -c 'import distutils as d;print(d.sysconfig_get_config_vars("LDVERSION")[0])')
INCLUDE_DIRS=include/ $(PYTHON_INCLUDE)
HEADERS=$(wildcard include/libpy/*.h)
INCLUDE=$(foreach d,$(INCLUDE_DIRS), -I$d)
OBJECTS=$(SOURCES:.cc=.o)
LIBRARY=libpy
SONAME=$(LIBRARY).so.$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)
TESTS=$(wildcard test/test_*.cc) test/main.cc
TESTRUNNER=test/run

.PHONY: all test clean

all: $(SONAME)

$(SONAME): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -shared -Wl,-soname,$(SONAME) -o $(SONAME)
	touch $(LIBRARY).so
	rm $(LIBRARY).so
	ln -s $(SONAME) $(LIBRARY).so

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c $< -o $@

test: $(TESTRUNNER)
	LD_LIBRARY_PATH=. $(TESTRUNNER)

$(TESTRUNNER): $(TESTS) $(SONAME)
	$(CC) $(CFLAGS) $(INCLUDE) $(TESTS) \
		-L. -lpy -lgtest -lpthread -lpython$(PYTHON_LDVERSION) -o $(TESTRUNNER)

clean:
	rm $(SONAME) $(LIBRARY).so $(OBJECTS) $(TESTRUNNER)
