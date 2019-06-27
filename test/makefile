CC     = gcc
CXX    = g++
CXXFLAGS := -g -O2 -std=c++11 -Wl,--no-as-needed -pthread
CFLAGS 	:= -g

TARGET	:= test

RM 	= rm -f

INCLUDE += -I../include -I../include/linux -I../engine

LIB += -L../lib -lengine -llua -ldl -lhiredis

SOURCE += $(wildcard *.cpp)
OBJS_DEF   := $(patsubst %.cpp,%.o,$(SOURCE))

all:$(TARGET)

$(TARGET):$(OBJS_DEF)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB)

clean:
	$(RM) $(TARGET) $(OBJS_DEF)

%.o: %.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE) -c -o $@ $<


