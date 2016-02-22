CC=g++
MAJOR_VERSION=1
MINOR_VERSION=0
MICRO_VERSION=0
CFLAGS=-std=gnu++14 -Wall -Wextra -O3
LDFLAGS=
SOURCES=$(wildcard src/*.cpp)
PYTHON_INCLUDE=$(shell python -c 'import distutils as d;print(d.sysconfig_get_python_inc())')
INCLUDE_DIRS=include/ $(PYTHON_INCLUDE)
INCLUDE=$(foreach d,$(INCLUDE_DIRS), -I$d)
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libpy
SONAME=$(LIBRARY).so.$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)

all: $(SOURCES) $(SONAME)

$(SONAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -shared -Wl,-soname,$(SONAME) -o $(SONAME)
	ln -s $(SONAME) $(LIBRARY).so

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c $< -o $@

clean:
	rm $(SONAME) $(OBJECTS) $(LIBRARY).so
