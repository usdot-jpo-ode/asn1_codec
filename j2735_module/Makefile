# JMC: Some improvements to build are possible.

CC=g++

TEST_MODULE_SOURCES+=test.cpp

# This file may be used as an input for make(3)
# Remove the lines below to convert it into a pure .am file
TARGET = test

CFLAGS +=
CPPFLAGS += -std=c++11 
CPPFLAGS += -DASN_CONVERTER_TITLE="J2735 DSRC decoder" -DASN_DISABLE_OER_SUPPORT -DPDU=MessageFrame
CPPFLAGS += -I.
CPPFLAGS += -I../j2735/include

# Make sure you have compiled the ASN file for J2735
CPPFLAGS += -I../asn1c_j2735

LDFLAGS += -L../j2735/lib
LIBS += -lj2735

HEADERS=$(TEST_MODULE_SOURCES:.cpp=.hpp)
OBJS=$(TEST_MODULE_SOURCES:.cpp=.o)

all: $(TARGET)

# Build the objects using new wildcard syntax.
%.o: %.c %.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

# Build the executable.
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)

