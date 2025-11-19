CXX = g++
CC = gcc
CFLAGS = -Wall -c
CXXFLAGS = -Wall -D__STDC_LIMIT_MACROS

OUTPUTDIR = ./bin/
MKDIR = mkdir -p $(OUTPUTDIR)

CSOURCES = \
	Sensirion/scd4x_i2c.c \
	Sensirion/sensirion_common.c \
	Sensirion/sensirion_i2c.c \
	Sensirion/sensirion_i2c_hal.c

CXXSOURCES = \
	main.cpp \
	SCD41_Reading.cpp
    
all:
	$(MKDIR)
	$(CC) $(CSOURCES) $(CFLAGS) -o $(OUTPUTDIR)
	$(CXX) $(CXXSOURCES) -o $(OUTPUTDIR)

.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
	$(RM) ./*.gc??
	$(RM) ./*.o