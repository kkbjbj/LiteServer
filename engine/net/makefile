CC     = gcc
CXX    = g++
CXXFLAGS := -g -std=c++11
CFLAGS 	:= -g

TARGET	:= test

RM 	= rm -f

LIB += -lpthread

SOURCE += $(wildcard *.cpp)
OBJS_DEF   := $(patsubst %.cpp,%.o,$(SOURCE))

all:$(TARGET)

$(TARGET):$(OBJS_DEF)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB)

clean:
	$(RM) $(TARGET) $(OBJS_DEF)

%.o: %.cpp
	$(CXX) $(CXXFLAGS)  $(INCLUDE) -c -o $@ $<


