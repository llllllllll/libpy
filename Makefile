CC := g++
MAJOR_VERSION := 1
MINOR_VERSION := 0
MICRO_VERSION := 0
CFLAGS := -std=gnu++14 -Wall -Wextra -O3 -g -fno-strict-aliasing
LDFLAGS :=
SOURCES :=$(wildcard src/*.cc)
OBJECTS :=$(SOURCES:.cc=.o)
DFILES := $(SOURCES:.cc=.d)
PYTHON_INCLUDE := $(shell etc/which_py_include)
PYTHON_LDVERSION := $(shell etc/which_py_so)
INCLUDE_DIRS := include/ $(PYTHON_INCLUDE)
INCLUDE := $(foreach d,$(INCLUDE_DIRS), -I$d)
LIBRARY := libpy
SONAME := $(LIBRARY).so.$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)
TEST_SOURCES := $(wildcard test/*.cc)
TEST_DFILES := $(TEST_SOURCES:.cc=.d)
TEST_OBJECTS := $(TEST_SOURCES:.cc=.o)
TEST_HEADERS := $(wildcard test/*.h)
TESTRUNNER := test/run

.PHONY: all test clean

all: $(SONAME)

$(SONAME): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -shared -Wl,-soname,$(SONAME) -o $(SONAME)
	@touch $(LIBRARY).so
	@rm $(LIBRARY).so
	ln -s $(SONAME) $(LIBRARY).so

%.o : %.cc
	$(CC) $(CFLAGS) $(INCLUDE) -MD -fPIC -c $< -o $@

test: $(TESTRUNNER)
	@LD_LIBRARY_PATH=. $<

$(TESTRUNNER): $(TEST_OBJECTS) $(SONAME)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJECTS) \
		-L. -lpy -lgtest -lpthread -lpython$(PYTHON_LDVERSION)

clean:
	@rm -f $(SONAME) $(LIBRARY).so $(OBJECTS) $(DFILES) \
		$(TESTRUNNER) $(TEST_OBJECTS) $(TEST_DFILES)

-include $(DFILES) $(TEST_DFILES)
