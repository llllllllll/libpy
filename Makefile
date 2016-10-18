CC := g++
MAJOR_VERSION := 1
MINOR_VERSION := 0
MICRO_VERSION := 0
CFLAGS := -std=gnu++14 -Wall -Wextra -O3 -g -fno-strict-aliasing `python-config --cflags`
LDFLAGS := `python-config --ldflags`
SOURCES :=$(wildcard src/*.cc)
OBJECTS :=$(SOURCES:.cc=.o)
DFILES := $(SOURCES:.cc=.d)
INCLUDE_DIRS := include/
INCLUDE := $(foreach d,$(INCLUDE_DIRS), -I$d)
LIBRARY := libpy
SONAME := $(LIBRARY).so.$(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)
OS := $(shell uname)
ifeq ($(OS),Darwin)
	SONAME_FLAG := install_name
else
	SONAME_FLAG := soname
endif
TEST_SOURCES := $(wildcard test/*.cc)
TEST_DFILES := $(TEST_SOURCES:.cc=.d)
TEST_OBJECTS := $(TEST_SOURCES:.cc=.o)
TEST_HEADERS := $(wildcard test/*.h)
TESTRUNNER := test/run

.PHONY: all test clean

all: $(SONAME)

local-install: $(SONAME)
	cp $< ~/lib
	@rm -f ~/lib/$(LIBRARY).so
	ln -s ~/lib/$(SONAME) ~/lib/$(LIBRARY).so
	cp -rf include/libpy ~/include

$(SONAME): $(OBJECTS) $(HEADERS)
	$(CC) $(LDFLAGS) $(OBJECTS) -shared -Wl,-$(SONAME_FLAG),$(SONAME) -o $(SONAME)
	@touch $(LIBRARY).so
	@rm $(LIBRARY).so
	ln -s $(SONAME) $(LIBRARY).so

%.o : %.cc
	$(CC) $(CFLAGS) $(INCLUDE) -MD -fPIC -c $< -o $@

test: $(TESTRUNNER)
	@LD_LIBRARY_PATH=. $<

$(TESTRUNNER): $(TEST_OBJECTS) $(SONAME)
	$(CC) $(LDFLAGS) -o $@ $(TEST_OBJECTS) \
		-L. -lpy -lgtest -lpthread

clean:
	@rm -f $(SONAME) $(LIBRARY).so $(OBJECTS) $(DFILES) \
		$(TESTRUNNER) $(TEST_OBJECTS) $(TEST_DFILES)

-include $(DFILES) $(TEST_DFILES)
