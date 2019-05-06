CC     = gcc
CXX    = g++
AR     = ar
CXXFLAGS := -g -O2 -std=c++11
CFLAGS 	:= -g

TARGET	:= libengine.a

RM 	= rm -f

INCLUDE += -I./

SOURCE += $(wildcard util/*.cpp)
SOURCE += $(wildcard thread/*.cpp)
SOURCE += $(wildcard timer/*.cpp)
SOURCE += $(wildcard net/*.cpp)

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


