CC = gcc
CFLAGS = -c -g -Wall -Wextra
SDL2FLAGS = $(shell pkg-config sdl2 --cflags --libs) -lm

SOURCEDIR = src
BUILDDIR = build

EXEC = text-editor
SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

$(BUILDDIR)/$(EXEC): $(OBJECTS)
	$(CC) $^ -o $@ $(SDL2FLAGS) -lSDL2_image

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<  $(SDL2FLAGS) -lSDL2_image

all: 
	dir $(BUILDDIR)

dir: 
	mkdir -p $(BUILDDIR)

clean: 
	-rm build/*

rebuild:
	make clean
	make

run: 
	make clean
	make
	clear
	./$(BUILDDIR)/$(EXEC)
