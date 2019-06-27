CC     = gcc
CXX    = g++
AR     = ar
CXXFLAGS := -g -O2 -std=c++11
CFLAGS 	:= -g

TARGET	:= libengine.a

RM 	= rm -f

INCLUDE += -I./ -I../include -I../include/linux

SOURCE += $(wildcard util/*.cpp)
SOURCE += $(wildcard thread/*.cpp)
SOURCE += $(wildcard timer/*.cpp)
SOURCE += $(wildcard net/*.cpp)
SOURCE += $(wildcard lua/*.cpp)
SOURCE += $(wildcard db/*.cpp)

OBJS_DEF   := $(patsubst %.cpp,%.o,$(SOURCE))

all:$(TARGET)

$(TARGET):$(OBJS_DEF)
	$(AR) -rcs $@ $^

clean:
	$(RM) $(TARGET) $(OBJS_DEF)

install:
	cp $(TARGET) ../lib/

%.o: %.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE) -c -o $@ $<


