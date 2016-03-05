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
HEADERS=$(wildcard include/*.h)
INCLUDE=$(foreach d,$(INCLUDE_DIRS), -I$d)
OBJECTS=$(SOURCES:.cc=.o)
LIBRARY=libpy
SONAME=$(LIBRARY).so.$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)
TESTS=$(wildcard test/test_*.cc) test/main.cc
TESTRUNNER=test/run

all: $(SOURCES) $(SONAME)

$(SONAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -shared -Wl,-soname,$(SONAME) -o $(SONAME)
	touch $(LIBRARY).so
	rm $(LIBRARY).so
	ln -s $(SONAME) $(LIBRARY).so

.cc.o: $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c $< -o $@

test: all
	$(CC) $(CFLAGS) $(INCLUDE) $(TESTS) \
		-L. -lpy -lgtest -lpthread -lpython$(PYTHON_LDVERSION) -o $(TESTRUNNER)
	LD_LIBRARY_PATH=. $(TESTRUNNER)

clean:
	rm $(SONAME) $(LIBRARY).so $(OBJECTS) $(TESTRUNNER)
