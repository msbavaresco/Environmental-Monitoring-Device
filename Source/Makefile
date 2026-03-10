CXX := g++
CC  := gcc

TARGET := bin/project

CFLAGS   := -Wall -O2
CXXFLAGS := -Wall -O2 -std=c++17 -D__STDC_LIMIT_MACROS
LDLIBS   := -lpaho-mqttpp3 -lpaho-mqtt3as -lpthread

C_SOURCES := \
	Sensirion/scd4x_i2c.c \
	Sensirion/sensirion_common.c \
	Sensirion/sensirion_i2c.c \
	Sensirion/sensirion_i2c_hal.c

CPP_SOURCES := \
	main.cpp \
	SCD41_Reading.cpp

C_OBJS   := $(C_SOURCES:.c=.o)
CPP_OBJS := $(CPP_SOURCES:.cpp=.o)

OBJS := $(C_OBJS) $(CPP_OBJS)

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CXX) $(OBJS) -o $(TARGET) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -rf bin

.PHONY: all clean
