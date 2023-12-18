CFLAGS = -c -g -Wall -Wextra
SDL2FLAGS = $(shell pkg-config sdl2 --cflags --libs) -lm

SOURCEDIR = src
BUILDDIR = build

EXEC = sed
SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

$(EXEC): $(OBJECTS)
	$(CC) $^ -o $@ $(SDL2FLAGS) 

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<  $(SDL2FLAGS) 

all: 
	dir $(BUILDDIR)

dir: 
	mkdir -p $(BUILDDIR)

clean: 
	-rm build/*
	-rm $(EXEC)

rebuild:
	make clean
	make

run: 
	make clean
	make
	clear
	./$(BUILDDIR)/$(EXEC)
