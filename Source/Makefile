CXX = g++
CC = gcc
C_FLAGS = -Wall -c
CXX_FLAGS = -Wall -D__STDC_LIMIT_MACROS -c

OUTPUTDIR = ./bin/
MKDIR = mkdir -p $(OUTPUTDIR)
RM = rm

C_SOURCES = \
	Sensirion/scd4x_i2c.c \
	Sensirion/sensirion_common.c \
	Sensirion/sensirion_i2c.c \
	Sensirion/sensirion_i2c_hal.c

CXX_SOURCES = \
	main.cpp \
	SCD41_Reading.cpp

C_OBJS = $(C_SOURCES:.c=.o)
CXX_OBJS = $(CXX_SOURCES:.cpp=.o)

all: project

project: $(C_OBJS) $(CXX_OBJS)
	$(MKDIR)
	$(CXX) $(C_OBJS) $(CXX_OBJS) -o $(OUTPUTDIR)/project

%.o: %.c
	$(CC) $(C_FLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
	$(RM) ./*.o
	$(RM) ./*.gc??
