# Makefile for Astro-CLI

# Compiler
CC = gcc
CFLAGS = -Wall -Isrc

# Directories
SRCDIR = src
BUILDDIR = build
BINDIR = bin
TARGET = $(BINDIR)/astro

# Sources
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(OBJECTS) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(BINDIR)

test: all
	@./tests/smoke.sh
