PYTHON ?= python3
MAJOR_VERSION := 1
MINOR_VERSION := 0
MICRO_VERSION := 0
# strict-prototypes is for C/ObjC only:
CXXFLAGS := -std=gnu++14 -Wall -Wextra -O3 -g -fno-strict-aliasing \
	$(shell $(PYTHON)-config --cflags | sed s/"-Wstrict-prototypes"//g)
LDFLAGS := $(shell $(PYTHON)-config --ldflags)
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
	AR := libtool
	ARFLAGS := -static -o
else
	SONAME_FLAG := soname
endif

GTEST_ROOT:= gtest
GTEST_DIR := $(GTEST_ROOT)/googletest
GTEST_INSTALLED := $(GTEST_ROOT)/.installed
GTEST_HEADERS := $(wildcard $(GTEST_DIR)/include/gtest/*.h) \
	$(wildcard $(GTEST_DIR)/include/gtest/internal/*.h)
GTEST_SRCS := $(wildcard $(GTEST_DIR)/src/*.cc) \
	$(wildcard $(GTEST_DIR)/src/*.h) $(GTEST_HEADERS)

TEST_SOURCES := $(wildcard test/*.cc)
TEST_DFILES := $(TEST_SOURCES:.cc=.d)
TEST_OBJECTS := $(TEST_SOURCES:.cc=.o)
TEST_HEADERS := $(wildcard test/*.h)  $(GTEST_HEADERS)
TEST_INCLUDE := -I test -I $(GTEST_DIR)/include
TESTRUNNER := test/run


.PHONY: all test clean clean-gtest clean-all gtest-install

all: $(SONAME)

local-install: $(SONAME)
	cp $< ~/lib
	@rm -f ~/lib/$(LIBRARY).so
	ln -s ~/lib/$(SONAME) ~/lib/$(LIBRARY).so
	cp -rf include/libpy ~/include

$(SONAME): $(OBJECTS) $(HEADERS)
	$(CXX) $(OBJECTS) -shared -Wl,-$(SONAME_FLAG),$(SONAME) \
		-o $(SONAME) $(LDFLAGS)
	@touch $(LIBRARY).so
	@rm $(LIBRARY).so
	ln -s $(SONAME) $(LIBRARY).so

src/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) -MD -fPIC -c $< -o $@

test/%.o: test/%.cc $(GTEST_INSTALLED)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(TEST_INCLUDE) -MD -fPIC -c $< -o $@

test: $(TESTRUNNER)
	@LD_LIBRARY_PATH=. $<

$(TESTRUNNER): gtest.a $(TEST_OBJECTS) $(SONAME)
	$(CXX) -o $@ $(TEST_OBJECTS) gtest.a -I $(GTEST_DIR)/include \
		-L. -lpy -lpthread $(LDFLAGS)

$(GTEST_INSTALLED):
	@mkdir -p $(GTEST_ROOT)
	@wget 'https://github.com/google/googletest/archive/release-1.8.0.tar.gz' \
		-O $(GTEST_ROOT)/gtest.tar.gz
	@cd $(GTEST_ROOT) && \
		tar --strip-components=1 -xf gtest.tar.gz
	@touch $(GTEST_INSTALLED)

gtest-install: $(GTEST_INSTALLED)

gtest.o: $(GTEST_INSTALLED) $(GTEST_SRCS)
	$(CXX) $(CXXFLAGS) -I $(GTEST_DIR) -I $(GTEST_DIR)/include -c \
		$(GTEST_DIR)/src/gtest-all.cc -o $@

gtest.a: gtest.o
	$(AR) $(ARFLAGS) $@ $^

clean:
	@rm -f $(SONAME) $(LIBRARY).so $(OBJECTS) $(DFILES) \
		$(TESTRUNNER) $(TEST_OBJECTS) $(TEST_DFILES) \
		gtest.o gtest.a

clean-gtest:
	@rm -r $(GTEST_ROOT)

clean-all: clean clean-gtest

-include $(DFILES) $(TEST_DFILES)

print-%:
	@echo $* = $($*)
