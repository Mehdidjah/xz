# Makefile for M-testC test framework

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -I../../src/xz -I../../src/liblzma/api -I../../src/common
LDFLAGS = -L../../src/liblzma/.libs -llzma

STEST_OBJ = stest.o
STEST_SRC = stest.c
STEST_HDR = stest.h

all: libstest.a

libstest.a: $(STEST_OBJ)
	ar rcs $@ $^

$(STEST_OBJ): $(STEST_SRC) $(STEST_HDR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(STEST_SRC) -o $(STEST_OBJ)

clean:
	rm -f $(STEST_OBJ) libstest.a

.PHONY: all clean

